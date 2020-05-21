#include<YAC/extractor.hpp>
#include<stdexcept>
#include<iostream>
#include<type_traits>
#include "YAC/istream_byte_source.hpp"
#include <QFileInfo>
#include <QDir>
#include "YAC/ostream_byte_sink.hpp"

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

	yac::EntryInfo * newFile(yac::EntryInfo * parent, std::string_view name, unsigned long long originalSize, unsigned long long compressedSize, unsigned long long positionInArchive) {
		const auto file = new yac::EntryInfo;
		file->parent = parent;
		file->type = yac::EntryType::File;
		file->name = QString::fromUtf8(name.data(), name.size());
		file->sizeCompressed = originalSize;
		file->sizeUncompressed = compressedSize;
		file->positionInArchive = positionInArchive;
		return file;
	};

	yac::EntryInfo * newFolder(yac::EntryInfo * parent, std::string_view name) {
		const auto folder = new yac::EntryInfo;
		folder->parent = parent;
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

	void locateContent(std::istream & archive, unsigned long long headerPosition) {
		// locate the file and skip the [un]compressed size data
		archive.seekg(headerPosition + 16);

		// skip the internal path data
		yac::IStreamByteSource source(archive);
		const auto pathLen = readLittleEndian<uint32_t>(source);
		std::string path;
		path.resize(pathLen);
		archive.read(path.data(), pathLen);
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
	delete m_left;
	delete m_right;
}

void yac::Extractor::extract(const EntryInfo& entry, std::istream & archive, std::string extractTo) {
	if (entry.type == EntryType::Folder) {
		QString folderPath = QString::fromStdString(extractTo);
		if (entry.parent != nullptr) {
			folderPath += '/';
			folderPath += entry.name;
		}
		QFileInfo info(folderPath);
		bool isDir = info.isDir();
		if (!info.exists()) {
			QDir(folderPath).mkpath(".");
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
		std::string filePath = extractTo + '/' + entry.name.toStdString();
		const auto qpath = QString::fromStdString(filePath);
		QFileInfo fileInfo(qpath);
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
	EntryInfo * metadataTree = newFolder(nullptr, "root");
	while (archive.tellg() != archiveEnd) {
		const auto startPosition = archive.tellg();
		const auto header = m_readFileHeader(source);
		std::cout << "extracted metadata about '" << header.path << "' compressed from " << header.originalSize << " to " << header.compressedSize << "\n";
		archive.seekg(header.compressedSize, std::ios::cur);
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

void yac::Extractor::m_addMetadata(EntryInfo & metadataRoot, const FileHeader & fileInfo, unsigned long long positionInArchive) {
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
			const auto nextNode = newFolder(currentNode, pathComponent);
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
		currentNode->children.push_back(newFile(currentNode, filename, fileInfo.originalSize, fileInfo.compressedSize, positionInArchive));
	} else {
		// todo: fuck: file already exists
		std::cout << "warning: two files with the same name!\n";
	}
}

void yac::Extractor::m_extract(const EntryInfo & entryInfo, std::istream & archive, std::ostream & to) {
	if (entryInfo.sizeUncompressed == 0) {
		return;
	}

	locateContent(archive, entryInfo.positionInArchive);

	IStreamByteSource source(archive);
	m_tree = m_readNode(source);

	OStreamByteSink sink(to);
	m_decode(source, sink);

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

