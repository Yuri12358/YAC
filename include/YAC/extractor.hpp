#ifndef YAC_EXTRACTOR_HPP
#define YAC_EXTRACTOR_HPP

#include<YAC/byte_source.hpp>
#include<YAC/byte_sink.hpp>
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

		unsigned long long m_fileSize = 0;
		TreeNode * m_tree = nullptr;

		void m_fail(std::string_view error);
		void m_readHeader(ByteSource & in);
		TreeNode * m_readNode(ByteSource & in);
		void m_decode(ByteSource & in, ByteSink & out);
	public:
		void extract(ByteSource & in, ByteSink & out);
	};
}

#endif // YAC_EXTRACTOR_HPP

