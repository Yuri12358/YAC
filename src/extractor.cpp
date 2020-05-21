#include<YAC/extractor.hpp>
#include<YAC/ostream_byte_sink.hpp>
#include<YAC/istream_byte_source.hpp>
#include<stdexcept>
#include<iostream>
#include<fstream>
// ReSharper disable once CppUnusedIncludeDirective
#include<type_traits>
// ReSharper disable once CppUnusedIncludeDirective
#include<QFileInfo>
#include<QDir>

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

	void locateContent(std::istream & archive, yac::PositionInArchive headerPosition) {
		// locate the file and skip the [un]compressed size data
		archive.seekg(headerPosition.value + 16);

		// skip the internal path data
		yac::IStreamByteSource source(archive);
		const auto pathLen = readLittleEndian<uint32_t>(source);
		std::string path;
		path.resize(pathLen);
		archive.read(path.data(), pathLen);
	}

	QString toQString(std::string_view str) {
		return QString::fromUtf8(str.data(), str.size());
	}
}

void yac::Extractor::TreeNode::dump(std::ostream& to) {
	if (m_isLeaf) {
		to << std::hex << static_cast<int>(m_value) << std::dec;
	} else {
		to << '(';
		m_left->dump(to);
		to << '-';
		m_right->dump(to);
		to << ')';
	}
}

yac::Extractor::TreeNode::TreeNode() = default;

yac::Extractor::TreeNode::TreeNode(Byte value)
	: m_size(HuffmanTreeSize{ 2 })
	, m_isLeaf(true)
	, m_value(value) {
}

yac::Extractor::TreeNode::TreeNode(TreeNode * a, TreeNode * b)
	: m_left(a)
	, m_right(b)
	, m_size(HuffmanTreeSize{ 1 } + a->m_size + b->m_size) {
}

yac::Extractor::TreeNode::~TreeNode() {
	delete m_left;
	delete m_right;
}

void yac::Extractor::extract(const EntryInfo& entry, std::istream & archive, const std::string & extractTo) {
	if (entry.type == EntryType::Folder) {
		QString folderPath = QString::fromStdString(extractTo);
		if (entry.parent != nullptr) {
			folderPath += '/';
			folderPath += entry.name;
		}
		const QFileInfo info(folderPath);
		bool isDir = info.isDir();
		if (!info.exists()) {
			(void) QDir(folderPath).mkpath("."); // todo: error handling
			isDir = true;
		}
		if (isDir) {
			for (auto child : entry.children) {
				extract(*child, archive, folderPath.toStdString());
			}
		} else {
			// todo: all has fucked up, there is already a file with that name
			std::cout << "warning: omitting a directory because of a file with the same name!\n";
		}
	} else {
		const std::string filePath = extractTo + '/' + entry.name.toStdString();
		const auto qpath = QString::fromStdString(filePath);
		const QFileInfo fileInfo(qpath);
		if (!fileInfo.exists()) {
			std::ofstream file(filePath, std::ios::binary);
			m_extract(entry, archive, file);
		} else {
			// todo: that file already exists
			std::cout << "warning: trying to extract a file on top of an existing one";
		}
	}
}

yac::EntryInfo * yac::Extractor::extractMetaInfo(std::istream & archive) {
	archive.seekg(0, std::ios::end);
	const auto archiveEnd = archive.tellg();

	archive.seekg(0);

	IStreamByteSource source(archive);
	EntryInfo * metadataTree = EntryInfo::newFolder(nullptr, "root");
	if (archiveEnd == -1) {
		std::cout << "failed to open the archive for reading\n";
		return metadataTree;
	}

	while (archive.tellg() != archiveEnd) {
		const PositionInArchive startPosition{ static_cast<Size>(archive.tellg()) };
		const auto header = m_readFileHeader(source);
		std::cout
			<< "extracted metadata about '" << header.path
			<< "' compressed from " << header.originalSize.value
			<< " to " << header.compressedSize.value << "\n";

		const auto treePosition = archive.tellg();
		auto tree = m_readNode(source);
		std::cout << "tree dump: ";
		tree->dump(std::cout);
		std::cout << "\n";
		archive.seekg(treePosition);
		delete tree;

		archive.seekg(header.compressedSize.value, std::ios::cur);
		m_addMetadata(*metadataTree, header, startPosition);
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
	std::string finalText = "Error while extracting: ";
	finalText.append(errorText.data(), errorText.size());
	throw std::runtime_error(finalText);
}

yac::Extractor::FileHeader yac::Extractor::m_readFileHeader(ByteSource & in) {
	FileHeader header;
	header.originalSize = UncompressedSize{ static_cast<Size>(readLittleEndian<uint64_t>(in)) };
	header.compressedSize = CompressedSize{ static_cast<Size>(readLittleEndian<uint64_t>(in)) };

	const auto pathLength = readLittleEndian<uint32_t>(in);
	header.path.resize(pathLength);
	in.getBytes(reinterpret_cast<Byte *>(header.path.data()), pathLength);

	return header;
}

void yac::Extractor::m_addMetadata(EntryInfo & metadataRoot, const FileHeader & fileInfo, PositionInArchive positionInArchive) {
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
			const auto nextNode = EntryInfo::newFolder(currentNode, toQString(pathComponent));
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
		currentNode->children.push_back(EntryInfo::newFile(currentNode, toQString(filename), fileInfo.originalSize, fileInfo.compressedSize, positionInArchive));
	} else {
		// todo: fuck: file already exists
		std::cout << "warning: two files with the same name!\n";
	}
}

void yac::Extractor::m_extract(const EntryInfo & entryInfo, std::istream & archive, std::ostream & to) {
	if (entryInfo.sizeUncompressed.value == 0) {
		return;
	}

	locateContent(archive, entryInfo.positionInArchive);

	IStreamByteSource source(archive);
	m_tree = m_readNode(source);

	OStreamByteSink sink(to);
	m_decode(source, sink, entryInfo.sizeUncompressed);

	std::cout << "finished decoding from " << entryInfo.sizeCompressed.value << " to " << entryInfo.sizeUncompressed.value << '\n';
	std::cout << "tree dump: ";
	m_tree->dump(std::cout);
	std::cout << "\n";

	delete m_tree;
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

void yac::Extractor::m_decode(ByteSource & in, ByteSink & out, UncompressedSize finalSize) {
	if (m_tree->m_isLeaf) {
		for (Size i = 0; i < finalSize.value; ++i) {
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
	for (Size i = 0; i < finalSize.value; ++i) {
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

