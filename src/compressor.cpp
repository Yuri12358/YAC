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

bool yac::Compressor::TreeNode::Comparator::operator()(const TreeNode* left, const TreeNode* right) const noexcept {
	return left->m_freq < right->m_freq;
}

yac::Compressor::TreeNode::~TreeNode() {
	delete m_left;
	delete m_right;
}

void yac::Compressor::compress(ByteSource & in, ByteSink & out) {
	m_calculateFrequency(in);
	if (m_fileSize > 0) {
		m_buildTree();
		m_generateCodes();
		m_writeHeader(out);
		m_encode(in, out);
	}
}

void yac::Compressor::m_calculateFrequency(ByteSource & in) {
	m_fileSize = 0;
	Byte c;
	while (in.getBytes(&c, 1) == 1) {
		++m_fileSize;
		++m_frequency[c];
	}
	in.reset();
}

void yac::Compressor::m_buildTree() {
	std::multiset<TreeNode *, TreeNode::Comparator> nodes;
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

void yac::Compressor::m_writeHeader(ByteSink & out) {
	// endianness-independent write
	auto fileSize = m_fileSize;
	for (int i = 0; i < 8; ++i) {
		Byte buffer = fileSize & 0xff;
		out.putBytes(&buffer, 1);
		fileSize >>= 8;
	}
	m_printNode(m_tree, out);
	delete m_tree;
}

void yac::Compressor::m_printNode(const TreeNode * node, ByteSink & out) {
	out.putBytes(reinterpret_cast<const Byte *>(&node->m_isLeaf), 1);
	if (node->m_isLeaf) {
		out.putBytes(&node->m_value, 1);
	} else {
		m_printNode(node->m_left, out);
		m_printNode(node->m_right, out);
	}
}

void yac::Compressor::m_encode(ByteSource & in, ByteSink & out) {
	int used = 0;
	long long totalBytes = 0;
	int prevProgressPercentage = 0;
	std::cout << "0%";
	bool errorIsPrinted = false;
	long long totalWrites = 0;
	Byte readBuffer;
	Byte writeBuffer = 0;
	while (in.getBytes(&readBuffer, 1) == 1) {
		for (bool b : m_codes[readBuffer]) {
			if (used == 8) {
				out.putBytes(&writeBuffer, 1);
				++totalWrites;
				writeBuffer = 0;
				used = 0;
			}
			writeBuffer |= (b << (7 - used++));
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
	}
	std::cout << '\n';
	std::cout << "Total bytes encoded: " << totalBytes << '\n';
	if (used) {
		out.putBytes(&writeBuffer, 1);
		++totalWrites;
	}
	std::cout << "Total encoded bytes written: " << totalWrites << '\n';
}

