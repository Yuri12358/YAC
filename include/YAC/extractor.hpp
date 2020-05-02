#ifndef YAC_EXTRACTOR_HPP
#define YAC_EXTRACTOR_HPP

#include<fstream>

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

		unsigned long long m_fileSize;
		TreeNode * m_tree;

		void m_checkFile(std::ifstream & in);
		void m_readHeader(std::ifstream & in);
		TreeNode * m_readNode(std::ifstream & in);
		void m_decode(std::ifstream & in, std::ofstream & out);
	public:
		void extract(std::ifstream & in, std::ofstream & out);
	};
}

#endif // YAC_EXTRACTOR_HPP

