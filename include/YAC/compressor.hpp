#ifndef YAC_COMPRESSOR_HPP
#define YAC_COMPRESSOR_HPP

#include<YAC/byte_sink.hpp>
#include<YAC/byte_source.hpp>
#include<YAC/size_types.hpp>
#include<fstream>
#include<vector>

namespace yac {
struct EntryInfo;

class Compressor {
		using FreqType = unsigned long long;
		using BitCode = std::vector<bool>;
		using Byte = unsigned char;

		struct TreeNode {
			struct Comparator {
				bool operator() (const TreeNode * left, const TreeNode * right) const noexcept;
			};

			bool m_isLeaf = false;
			Byte m_value{};
			FreqType m_freq = 0;
			TreeNode * m_left = nullptr;
			TreeNode * m_right = nullptr;

			TreeNode() = default;
			TreeNode(Byte value, FreqType freq);
			TreeNode(TreeNode * a, TreeNode * b);
			~TreeNode();
		};

		unsigned long long m_fileSize = 0;
		FreqType m_frequency[256]{};
		TreeNode * m_tree = nullptr;
		BitCode m_codes[256];

		void m_calculateFrequency(std::istream & in);
		void m_buildTree();
		void m_generateCodes();
		void m_visitNode(const TreeNode * node, BitCode & buffer);
		HuffmanTreeSize m_writeHeader(EntryInfo & fileInfo, std::ostream & out);

		// returns the count of written bytes
		HuffmanTreeSize m_printNode(const TreeNode * node, std::ostream & out);

		void m_writeCompressedSize(std::ostream & out, std::ostream::pos_type headerStartPos, CompressedSize compressedSize);

		// returns the compressed size (bytes written to out)
		CompressedContentSize m_encode(std::istream & in, std::ostream & out);
		void m_compress(EntryInfo & fileInfo, std::istream & in, std::ostream &out);

	public:
		// accepts directories and files
		void compress(EntryInfo & entry, std::ostream & archive);
	};
}

#endif // YAC_COMPRESSOR_HPP

