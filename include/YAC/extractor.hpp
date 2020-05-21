#ifndef YAC_EXTRACTOR_HPP
#define YAC_EXTRACTOR_HPP

#include<ostream>
#include<istream>
#include "ArchivedFileModel.hpp"

namespace yac {
	class Extractor {
		typedef unsigned char Byte;

		struct TreeNode {
			TreeNode * m_left{ nullptr };
			TreeNode * m_right{ nullptr };
			HuffmanTreeSize m_size{};
			bool m_isLeaf{ false };
			Byte m_value{};

			void dump(std::ostream & to);

			TreeNode();
			TreeNode(Byte value);
			TreeNode(TreeNode * a, TreeNode * b);
			~TreeNode();
		};

		struct FileHeader {
			UncompressedSize originalSize{ 0 };
			CompressedSize compressedSize{ 0 };
			std::string path;
		};

		TreeNode * m_tree = nullptr;

		void m_fail(std::string_view error);
		FileHeader m_readFileHeader(std::istream & in);
		TreeNode * m_readNode(std::istream & in);
		void m_decode(std::istream & in, std::ostream & out, UncompressedSize finalSize);
		void m_addMetadata(EntryInfo & metadataRoot, const FileHeader & fileInfo, PositionInArchive pos);
		void m_extract(const EntryInfo & what, std::istream & from, std::ostream & to);

	public:
		void extract(const EntryInfo & what, std::istream & from, const std::string & where);

		EntryInfo * extractMetaInfo(std::istream & archive);
	};
}

#endif // YAC_EXTRACTOR_HPP

