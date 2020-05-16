#include<YAC/extractor.hpp>
#include<stdexcept>

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
	if (m_left) {
		delete m_left;
	}
	if (m_right) {
		delete m_right;
	}
}

void yac::Extractor::extract(ByteSource & in, std::ofstream & out) {
	m_readHeader(in);
	m_decode(in, out);
}

void yac::Extractor::m_fail() {
	throw std::runtime_error("Error while reading file");
}

void yac::Extractor::m_readHeader(ByteSource & in) {
	Byte buffer[8];
	if (in.getBytes(buffer, 8) < 8) {
		m_fail();
	}
	m_fileSize = 0;
	for (int i = 0; i < 8; ++i) {
		m_fileSize += (((unsigned long long) buffer[i]) << (8 * i));
	}
	m_tree = m_readNode(in);
}

yac::Extractor::TreeNode * yac::Extractor::m_readNode(ByteSource & in) {
	bool isLeaf;
	if (in.getBytes(reinterpret_cast<Byte *>(&isLeaf), 1) < 1) {
		m_fail();
	}
	if (isLeaf) {
		Byte c;
		if (in.getBytes(&c, 1)) {
			return new TreeNode(c);
		} else {
			m_fail();
		}
	}
	return new TreeNode(m_readNode(in), m_readNode(in));
}

void yac::Extractor::m_decode(ByteSource & in, std::ofstream & out) {
	if (m_tree->m_isLeaf) {
		for (unsigned long long i = 0; i < m_fileSize; ++i) {
			out.put(m_tree->m_value);
		}
		return;
	}

	Byte buf;
	int used = 8;
	for (unsigned long long i = 0; i < m_fileSize; ++i) {
		TreeNode * node = m_tree;
		while (!node->m_isLeaf) {
			if (used == 8) {
				used = 0;
				if (in.getBytes(&buf, 1) < 1) {
					m_fail();
				}
			}
			if (buf & (1 << (7 - used++))) {
				node = node->m_right;
			} else {
				node = node->m_left;
			}
		}
		out.put(node->m_value);
	}
}

