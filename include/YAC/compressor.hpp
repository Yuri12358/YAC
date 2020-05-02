#ifndef YAC_COMPRESSOR_HPP
#define YAC_COMPRESSOR_HPP

#include<fstream>
#include<vector>

namespace yac {
	class Compressor {
		typedef unsigned char Byte;
		typedef unsigned long long FreqType;
		typedef std::vector<bool> BitCode;

		struct TreeNode {
			bool m_isLeaf;
			Byte m_value;
			FreqType m_freq;
			TreeNode * m_left;
			TreeNode * m_right;

			TreeNode();
			TreeNode(Byte value, FreqType freq);
			TreeNode(TreeNode * a, TreeNode * b);
			static bool cmp(const TreeNode * a, const TreeNode * b);
			~TreeNode();
		};

		unsigned long long m_fileSize;
		FreqType m_frequency[256];
		TreeNode * m_tree;
		BitCode m_codes[256];

		void m_calculateFrequency(std::ifstream & in);
		void m_buildTree();
		void m_generateCodes();
		void m_visitNode(const TreeNode * node, BitCode & buffer);
		void m_writeHeader(std::ofstream & out);
		void m_printNode(const TreeNode * node, std::ofstream & out);
		void m_encode(std::ifstream & in, std::ofstream & out);

	public:
		void compress(std::ifstream & in, std::ofstream & out);
	};
}

#endif // YAC_COMPRESSOR_HPP

