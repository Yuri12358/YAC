#ifndef YAC_EXTRACTOR_HPP
#define YAC_EXTRACTOR_HPP

#include<YAC/byte_source.hpp>
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

		void m_fail();
		void m_readHeader(ByteSource & in);
		TreeNode * m_readNode(ByteSource & in);
		void m_decode(ByteSource & in, std::ofstream & out);
	public:
		void extract(ByteSource & in, std::ofstream & out);
	};
}

#endif // YAC_EXTRACTOR_HPP

