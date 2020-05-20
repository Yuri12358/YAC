#include<YAC/extractor.hpp>
#include<stdexcept>
#include<iostream>
#include<type_traits>
#include "YAC/istream_byte_source.hpp"

namespace {
	template<class T>
	T readLittleEndian(yac::ByteSource & in)
	{
		static_assert(std::is_integral_v<T>);
		yac::Byte buffer[sizeof(T)];
		in.getBytes(buffer, sizeof(T));
		T result{};
		for (unsigned i = 0; i < sizeof(T); ++i) {
			result += static_cast<T>(buffer[i]) << (8 * i);
		}
		return result;
	}

	yac::EntryInfo * newFile(std::string_view name, unsigned long long originalSize, unsigned long long compressedSize) {
		const auto folder = new yac::EntryInfo;
		folder->type = yac::EntryType::File;
		folder->name = QString::fromUtf8(name.data(), name.size());
		folder->sizeCompressed = originalSize;
		folder->sizeUncompressed = compressedSize;
		return folder;
	};

	yac::EntryInfo * newFolder(std::string_view name) {
		const auto folder = new yac::EntryInfo;
		folder->type = yac::EntryType::Folder;
		folder->name = QString::fromUtf8(name.data(), name.size());
		folder->sizeCompressed = 0;
		folder->sizeUncompressed = 0;
		return folder;
	};

	bool operator ==(QString lhs, std::string_view rhs) {
		return static_cast<std::size_t>(lhs.size()) == rhs.size()
			&& std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	yac::EntryInfo * findChild(const yac::EntryInfo & folder, std::string_view wantedName) {
		const auto childIt = std::find_if(folder.children.begin(), folder.children.end(), [wantedName] (const yac::EntryInfo * child) {
			return child->name == wantedName;
		});
		if (childIt == folder.children.end()) {
			return nullptr;
		}
		return *childIt;
	}
}

yac::Extractor::TreeNode::TreeNode()
	: m_left(nullptr)
	, m_right(nullptr)
	, m_value(0)
	, m_isLeaf(false) {
}

yac::Extractor::TreeNode::TreeNode(Byte value)
	: m_left(nullptr)
	, m_right(nullptr)
	, m_value(value)
	, m_isLeaf(true) {
}

yac::Extractor::TreeNode::TreeNode(TreeNode * a, TreeNode * b)
	: m_left(a)
	, m_right(b)
	, m_value(0)
	, m_isLeaf(false) {
}

yac::Extractor::TreeNode::~TreeNode() {
	if (m_left) {
		delete m_left;
	}
	if (m_right) {
		delete m_right;
	}
}

void yac::Extractor::extract(ByteSource & in, ByteSink & out) {
	//m_readHeader(in);
	//m_decode(in, out);
}

yac::EntryInfo * yac::Extractor::extractMetaInfo(std::istream & archive) {
	archive.seekg(0, std::ios::end);
	const auto archiveEnd = archive.tellg();

	archive.seekg(0);

	IStreamByteSource source(archive);
	EntryInfo * metadataTree = newFolder("root");
	while (archive.tellg() != archiveEnd) {
		const auto header = m_readFileHeader(source);
		archive.seekg(header.compressedSize, std::ios::cur);
		m_addMetadata(*metadataTree, header);
	}

	return metadataTree;
}

std::string operator +(std::string_view lhs, std::string_view rhs) {
	std::string result;
	result.reserve(lhs.size() + rhs.size());
	for (auto i : { lhs, rhs })
		result.append(i.data(), i.size());
	return result;
}

void yac::Extractor::m_fail(std::string_view errorText) {
	throw std::runtime_error("Error while extracting: " + errorText);
}

yac::Extractor::FileHeader yac::Extractor::m_readFileHeader(ByteSource & in) {
	FileHeader header;
	header.originalSize = readLittleEndian<uint64_t>(in);
	header.compressedSize = readLittleEndian<uint64_t>(in);

	const auto pathLength = readLittleEndian<uint32_t>(in);
	header.path.resize(pathLength);
	in.getBytes(reinterpret_cast<Byte *>(header.path.data()), pathLength);

	return header;
	m_tree = m_readNode(in);
}

void yac::Extractor::m_addMetadata(EntryInfo & metadataRoot, const FileHeader & fileInfo) {
	auto currentNode = &metadataRoot;

	const std::string_view path = fileInfo.path;
	std::size_t currentPosition = 0;
	auto delimPosition = path.find('/', currentPosition);

	while (delimPosition != std::string_view::npos) {
		const auto pathComponent = path.substr(currentPosition, delimPosition - currentPosition);
		currentPosition = delimPosition + 1;
		delimPosition = path.find('/', currentPosition);

		const auto child = findChild(*currentNode, pathComponent);
		if (child == nullptr) {
			const auto nextNode = newFolder(pathComponent);
			currentNode->children.push_back(nextNode);
			currentNode = nextNode;
		} else if (child->type == EntryType::Folder) {
			currentNode = child;
		} else {
			// todo: fuck: the entry is a file and a folder at the same time!
			std::cout << "warning: the entry is a file and a folder at the same time!\n";
		}
	}

	const auto filename = path.substr(currentPosition, delimPosition - currentPosition);
	const auto file = findChild(*currentNode, filename);
	if (file == nullptr) {
		currentNode->children.push_back(newFile(filename, fileInfo.originalSize, fileInfo.compressedSize));
	} else {
		// todo: fuck: file already exists
		std::cout << "warning: two files with the same name!\n";
	}
}

yac::Extractor::TreeNode * yac::Extractor::m_readNode(ByteSource & in) {
	bool isLeaf;
	if (in.getBytes(reinterpret_cast<Byte *>(&isLeaf), 1) < 1) {
		m_fail("failed to read whether the huffman table tree node is a leaf");
	}
	if (isLeaf) {
		Byte c;
		if (in.getBytes(&c, 1)) {
			return new TreeNode(c);
		} else {
			m_fail("failed to read the huffman table tree node's corresponding byte");
		}
	}
	const auto left = m_readNode(in);
	const auto right = m_readNode(in);
	return new TreeNode(left, right);
}

void yac::Extractor::m_decode(ByteSource & in, ByteSink & out) {
	if (m_tree->m_isLeaf) {
		for (unsigned long long i = 0; i < m_fileSize; ++i) {
			if (!out.putBytes(&m_tree->m_value, 1)) {
				m_fail("failed to write extracted data");
			}
		}
		out.finish();
		return;
	}

	Byte buf;
	int used = 8;
	long long totalReads{};
	for (unsigned long long i = 0; i < m_fileSize; ++i) {
		TreeNode * node = m_tree;
		while (!node->m_isLeaf) {
			if (used == 8) {
				used = 0;
				++totalReads;
				if (in.getBytes(&buf, 1) < 1) {
					std::cout << "Failed while attempting to perform the read #" << totalReads << '\n';
					std::cout.flush();
					m_fail("failed to read some data to decode");
				}
			}
			if (buf & (1 << (7 - used++))) {
				node = node->m_right;
			} else {
				node = node->m_left;
			}
		}
		if (!out.putBytes(&node->m_value, 1)) {
			m_fail("failed to write some decoded data");
		};
	}
	out.finish();
}

