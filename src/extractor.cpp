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

void yac::Extractor::extract(std::ifstream & in, std::ofstream & out) {
	m_readHeader(in);
	m_decode(in, out);
}

void yac::Extractor::m_checkFile(std::ifstream & in) {
	if (!in.good()) {
		throw std::runtime_error("Error while reading file");
	}
}

void yac::Extractor::m_readHeader(std::ifstream & in) {
	m_fileSize = 0;
	for (int i = 0; i < 8; ++i) {
		m_fileSize += (((unsigned long long) in.get()) << (8 * i));
	}
	m_checkFile(in);
	m_tree = m_readNode(in);
}

yac::Extractor::TreeNode * yac::Extractor::m_readNode(std::ifstream & in) {
	bool isLeaf = in.get();
	m_checkFile(in);
	if (isLeaf) {
		Byte c = in.get();
		m_checkFile(in);
		return new TreeNode(c);
	}
	return new TreeNode(m_readNode(in), m_readNode(in));
}

void yac::Extractor::m_decode(std::ifstream & in, std::ofstream & out) {
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
				buf = in.get();
				m_checkFile(in);
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

