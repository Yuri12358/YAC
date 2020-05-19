#ifndef YAC_COMPRESSOR_HPP
#define YAC_COMPRESSOR_HPP

#include<YAC/byte_sink.hpp>
#include<YAC/byte_source.hpp>
#include<fstream>
#include<vector>
#include<cstddef>

namespace yac {
	class Compressor {
		using FreqType = unsigned long long;
		using BitCode = std::vector<bool>;
		using Byte = unsigned char;

		struct TreeNode {
			struct Comparator {
				bool operator() (const TreeNode * left, const TreeNode * right) const noexcept;
			};

			bool m_isLeaf;
			Byte m_value;
			FreqType m_freq;
			TreeNode * m_left;
			TreeNode * m_right;

			TreeNode();
			TreeNode(Byte value, FreqType freq);
			TreeNode(TreeNode * a, TreeNode * b);
			~TreeNode();
		};

		unsigned long long m_fileSize = 0;
		FreqType m_frequency[256]{};
		TreeNode * m_tree = nullptr;
		BitCode m_codes[256];

		void m_calculateFrequency(ByteSource & in);
		void m_buildTree();
		void m_generateCodes();
		void m_visitNode(const TreeNode * node, BitCode & buffer);
		void m_writeHeader(ByteSink & out);
		void m_printNode(const TreeNode * node, ByteSink & out);
		void m_encode(ByteSource & in, ByteSink & out);

	public:
		void compress(ByteSource & in, ByteSink & out);
	};
}

#endif // YAC_COMPRESSOR_HPP

