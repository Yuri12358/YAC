#ifndef YAC_EXTRACTOR_HPP
#define YAC_EXTRACTOR_HPP

#include<YAC/byte_source.hpp>
#include<YAC/byte_sink.hpp>
#include<fstream>
#include "ArchivedFileModel.hpp"

namespace yac {
	class Extractor {
		typedef unsigned char Byte;

		struct TreeNode {
			TreeNode * m_left;
			TreeNode * m_right;
			bool m_isLeaf;
			Byte m_value;

			TreeNode();
			TreeNode(Byte value);
			TreeNode(TreeNode * a, TreeNode * b);
			~TreeNode();
		};

		struct FileHeader {
			unsigned long long originalSize = 0;
			unsigned long long compressedSize = 0;
			std::string path;
		};

		unsigned long long m_fileSize = 0;
		TreeNode * m_tree = nullptr;

		void m_fail(std::string_view error);
		FileHeader m_readFileHeader(ByteSource & in);
		TreeNode * m_readNode(ByteSource & in);
		void m_decode(ByteSource & in, ByteSink & out);
		void m_addMetadata(EntryInfo & metadataRoot, const FileHeader & fileInfo, unsigned long long positionInArchive);
		void m_extract(const EntryInfo & what, std::istream & from, std::ostream & to);

	public:
		void extract(const EntryInfo & what, std::istream & from, std::string where);

		EntryInfo * extractMetaInfo(std::istream & archive);
	};
}

#endif // YAC_EXTRACTOR_HPP

