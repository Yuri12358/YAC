#include<YAC/compressor.hpp>
#include<YAC/ostream_byte_sink.hpp>
#include<YAC/istream_byte_source.hpp>
#include"ArchivedFileModel.hpp"
#include<functional>
#include<set>
#include<iostream>

namespace {
	std::string getAbsolutePath(const yac::EntryInfo & fileInfo) {
		if (fileInfo.parent == nullptr) {
			return "";
		}
		std::string result = fileInfo.name.toStdString();
		auto entryPtr = fileInfo.parent;
		while (entryPtr->parent != nullptr) {
			// todo: optimize
			result = std::string(entryPtr->name.toStdString() + "/") + result; // NOLINT(performance-inefficient-string-concatenation)
			entryPtr = entryPtr->parent;
		}
		return result;
	}

	void writeLittleEndian(std::ostream & stream, uint64_t num) {
		for (int i = 0; i < 8; ++i) {
			const yac::Byte buffer = num & 0xff;
			stream.write(reinterpret_cast<const char *>(&buffer), 1);
			num >>= 8;
		}
	}

	void writeLittleEndian(std::ostream & stream, uint32_t num) {
		for (int i = 0; i < 4; ++i) {
			const yac::Byte buffer = num & 0xff;
			stream.write(reinterpret_cast<const char *>(&buffer), 1);
			num >>= 8;
		}
	}
}

yac::Compressor::TreeNode::TreeNode(Byte value, FreqType freq)
	: m_isLeaf(true)
	, m_value(value)
	, m_freq(freq) {
}

yac::Compressor::TreeNode::TreeNode(TreeNode * a, TreeNode * b)
	: m_freq(a->m_freq + b->m_freq)
	, m_left(a)
	, m_right(b) {
}

bool yac::Compressor::TreeNode::Comparator::operator()(const TreeNode* left, const TreeNode* right) const noexcept {
	return left->m_freq < right->m_freq;
}

yac::Compressor::TreeNode::~TreeNode() {
	delete m_left;
	delete m_right;
}

void yac::Compressor::compress(EntryInfo & entry, std::ostream & archive) {
	archive.seekp(0, std::ios::end);
	if (entry.type == EntryType::File) {
		const auto path = entry.fullPath.toStdString();
		std::ifstream fileStream(path, std::ios::binary);
		//IStreamByteSource source(fileStream);
		m_compress(entry, fileStream, archive);
	} else {
		for (auto & child : entry.children) {
			compress(*child, archive);
		}
	}
}

void yac::Compressor::m_compress(EntryInfo & fileInfo, std::istream & in, std::ostream & out) {
	m_calculateFrequency(in);
	if (m_fileSize > 0) {
		m_buildTree();
		m_generateCodes();

		const Size startPosition = out.tellp();
		const auto treeSize = m_writeHeader(fileInfo, out);

		//OStreamByteSink sink(out);
		// add the content info size to the code tree size
		fileInfo.sizeCompressed = m_encode(in, out) + treeSize;
		fileInfo.positionInArchive = PositionInArchive{ startPosition };

		const auto finalPosition = out.tellp();
		m_writeCompressedSize(out, startPosition, fileInfo.sizeCompressed);

		// restore the position after header modification
		out.seekp(finalPosition);
	} else {
		m_writeHeader(fileInfo, out);
	}
}

void yac::Compressor::m_calculateFrequency(std::istream & in) {
	m_fileSize = 0;
	Byte c;

	std::fill(std::begin(m_frequency), std::end(m_frequency), 0);

	while (in.read(reinterpret_cast<char *>(&c), 1), in.good()) {
		++m_fileSize;
		++m_frequency[c];
	}
	in.clear();
	in.seekg(0);
}

void yac::Compressor::m_buildTree() {
	std::multiset<TreeNode *, TreeNode::Comparator> nodes;
	for (int i = 0; i < 256; ++i) {
		if (m_frequency[i] > 0) {
			nodes.insert(new TreeNode(i, m_frequency[i]));
		}
	}
	while (nodes.size() > 1) {
		const auto node1 = *(nodes.begin());
		const auto node2 = *(++nodes.begin());

		nodes.erase(nodes.begin(), ++ ++nodes.begin());
		const auto p = nodes.insert(new TreeNode(node1, node2));
	}
	m_tree = *nodes.begin();
}

void yac::Compressor::m_generateCodes() {
	BitCode buffer;
	m_visitNode(m_tree, buffer);
}

void yac::Compressor::m_visitNode(const TreeNode * node, BitCode & buffer) {
	if (node->m_isLeaf) {
		if (node == m_tree) {
			buffer.push_back(false);
		}
		m_codes[node->m_value] = buffer;
		return;
	}
	buffer.push_back(false);
	m_visitNode(node->m_left, buffer);
	buffer.back() = true;
	m_visitNode(node->m_right, buffer);
	buffer.pop_back();
}

yac::HuffmanTreeSize yac::Compressor::m_writeHeader(EntryInfo & fileInfo, std::ostream & out) {
	writeLittleEndian(out, static_cast<uint64_t>(fileInfo.sizeUncompressed.value));

	// skip some place for the compressed size
	writeLittleEndian(out, static_cast<uint64_t>(0));

	// write the filename and its 4-byte len
	const auto pathToStore = getAbsolutePath(fileInfo);
	writeLittleEndian(out, static_cast<uint32_t>(pathToStore.size()));
	out.write(pathToStore.data(), pathToStore.size());

	if (m_fileSize > 0) {
		const auto treeSize = m_printNode(m_tree, out);
		delete m_tree;
		return treeSize;
	}
	return {};
}

// ReSharper disable once CppMemberFunctionMayBeStatic
yac::HuffmanTreeSize yac::Compressor::m_printNode(const TreeNode * node, std::ostream & out) {
	out.put(node->m_isLeaf);
	HuffmanTreeSize writtenBytes{ 1 };
	if (node->m_isLeaf) {
		out.put(node->m_value);
		//out.putBytes(&node->m_value, 1);
		++writtenBytes.value;
	} else {
		writtenBytes += m_printNode(node->m_left, out);
		writtenBytes += m_printNode(node->m_right, out);
	}
	return writtenBytes;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void yac::Compressor::m_writeCompressedSize(std::ostream & out, std::ostream::pos_type headerStartPos, CompressedSize compressedSize) {
	out.seekp(headerStartPos.operator long long() + 8);
	writeLittleEndian(out, static_cast<uint64_t>(compressedSize.value));
}

yac::CompressedContentSize yac::Compressor::m_encode(std::istream & in, std::ostream & out) {
	int used = 0;
	long long totalBytes = 0;
	int prevProgressPercentage = 0;
	std::cout << "0%";
	bool errorIsPrinted = false;
	CompressedContentSize totalWrites{ 0 };
	Byte writeBuffer = 0;
	char readBuffer;
	for (Size totalReads = 0; (in.read(&readBuffer, 1), in.good()) && totalReads < m_fileSize; ++totalReads) {
		for (bool b : m_codes[static_cast<Byte>(readBuffer)]) {
			if (used == 8) {
				//out.putBytes(&writeBuffer, 1);
				out.put(writeBuffer);
				++totalWrites.value;
				writeBuffer = 0;
				used = 0;
			}
			writeBuffer |= (b << (7 - used++));
		}
		++totalBytes;
		if (m_fileSize > 0) {
			const int progressPercentage = static_cast<int>(totalBytes * 100 / m_fileSize);
			if (progressPercentage != prevProgressPercentage) {
				prevProgressPercentage = progressPercentage;
				std::cout << "\r" << progressPercentage << "%        ";
				std::cout.flush();
			}
		} else if (not errorIsPrinted) {
			errorIsPrinted = true;
			std::cout << "\rinternal error: filesize is 0";
		}
	}
	std::cout << '\n';
	std::cout << "Total bytes encoded: " << totalBytes << '\n';
	if (used) {
		//out.putBytes(&writeBuffer, 1);
		out.put(writeBuffer);
		++totalWrites.value;
	}
	//out.finish();
	std::cout << "Total encoded bytes written: " << totalWrites.value << '\n';
	return totalWrites;
}
