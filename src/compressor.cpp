#include<YAC/compressor.hpp>
#include<functional>
#include<set>
#include<cstdio>
#include<iostream>

yac::Compressor::TreeNode::TreeNode()
	: m_freq(0)
	, m_isLeaf(false)
	, m_value(0)
	, m_left(nullptr)
	, m_right(nullptr) {
}

yac::Compressor::TreeNode::TreeNode(Byte value, FreqType freq)
	: m_freq(freq)
	, m_isLeaf(true)
	, m_value(value)
	, m_left(nullptr)
	, m_right(nullptr) {
}

yac::Compressor::TreeNode::TreeNode(TreeNode * a, TreeNode * b)
	: m_isLeaf(false)
	, m_value(0)
	, m_freq(a->m_freq + b->m_freq)
	, m_left(a)
	, m_right(b) {
}

bool yac::Compressor::TreeNode::cmp(const TreeNode * a,
	const TreeNode * b) {
	return a->m_freq <= b->m_freq;
}

yac::Compressor::TreeNode::~TreeNode() {
	if (m_left) {
		delete m_left;
	}
	if (m_right) {
		delete m_right;
	}
}

void yac::Compressor::compress(std::ifstream & in, std::ofstream & out) {
	m_calculateFrequency(in);
	if (m_fileSize > 0) {
		m_buildTree();
		m_generateCodes();
		m_writeHeader(out);
		m_encode(in, out);
	}
}

void yac::Compressor::m_calculateFrequency(std::ifstream & in) {
	m_fileSize = 0;
	Byte c = in.get();
	while (in.good()) {
		++m_fileSize;
		++m_frequency[c];
		c = in.get();
	}
	in.seekg(0, in.beg);
	in.clear();
}

void yac::Compressor::m_buildTree() {
	std::function<decltype(TreeNode::cmp)> cmp = TreeNode::cmp;
	std::set<TreeNode *, decltype(cmp)> nodes(cmp);
	for (int i = 0; i < 256; ++i) {
		if (m_frequency[i] > 0) {
			nodes.insert(new TreeNode(i, m_frequency[i]));
		}
	}
	while (nodes.size() > 1) {
		auto node1 = *(nodes.begin());
		auto node2 = *(++nodes.begin());
		nodes.erase(nodes.begin(), ++ ++nodes.begin());
		nodes.insert(new TreeNode(node1, node2));
	}
	m_tree = *nodes.begin();
}

void yac::Compressor::m_generateCodes() {
	BitCode buffer;
	m_visitNode(m_tree, buffer);
}

void yac::Compressor::m_visitNode(const TreeNode * node, BitCode & buffer) {
	if (node->m_isLeaf) {
		if (node == m_tree) {
			buffer.push_back(false);
		}
		m_codes[node->m_value] = buffer;
		return;
	}
	buffer.push_back(false);
	m_visitNode(node->m_left, buffer);
	buffer.back() = true;
	m_visitNode(node->m_right, buffer);
	buffer.pop_back();
}

void yac::Compressor::m_writeHeader(std::ofstream & out) {
	// endianness-independent write
	auto fileSize = m_fileSize;
	for (int i = 0; i < 8; ++i) {
		out.put(fileSize & 0xff);
		fileSize >>= 8;
	}
	m_printNode(m_tree, out);
	delete m_tree;
}

void yac::Compressor::m_printNode(const TreeNode * node, std::ofstream & out) {
	out.put(node->m_isLeaf);
	if (node->m_isLeaf) {
		out.put(node->m_value);
	} else {
		m_printNode(node->m_left, out);
		m_printNode(node->m_right, out);
	}
}

void yac::Compressor::m_encode(std::ifstream & in, std::ofstream & out) {
	in.clear();
	in.seekg(0, in.beg);
	Byte c = in.get();
	Byte buf = 0;
	int used = 0;
	long long totalBytes = 0;
	int prevProgressPercentage = 0;
	std::cout << "0%";
	bool errorIsPrinted = false;
	while (in.good()) {
		for (bool b : m_codes[c]) {
			if (used == 8) {
				out.put(buf);
				buf = 0;
				used = 0;
			}
			buf |= (b << (7 - used++));
		}
		++totalBytes;
		if (m_fileSize > 0) {
			const int progressPercentage = static_cast<int>(totalBytes * 100 / m_fileSize);
			if (progressPercentage != prevProgressPercentage) {
				prevProgressPercentage = progressPercentage;
				std::cout << "\r" << progressPercentage << "%        ";
				std::cout.flush();
			}
		} else if (not errorIsPrinted) {
			errorIsPrinted = true;
			std::cout << "\rinternal error: filesize is 0";
		}
		c = in.get();
	}
	std::cout << '\n';
	if (used) {
		out.put(buf);
	}
}

