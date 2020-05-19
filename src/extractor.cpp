#include<YAC/extractor.hpp>
#include<stdexcept>
#include <iostream>

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

void yac::Extractor::extract(ByteSource & in, ByteSink & out) {
	m_readHeader(in);
	m_decode(in, out);
}

std::string operator +(std::string_view lhs, std::string_view rhs) {
	std::string result;
	result.reserve(lhs.size() + rhs.size());
	for (auto i : { lhs, rhs })
		result.append(i.data(), i.size());
	return result;
}

void yac::Extractor::m_fail(std::string_view errorText) {
	throw std::runtime_error("Error while extracting: " + errorText);
}

void yac::Extractor::m_readHeader(ByteSource & in) {
	Byte buffer[8];
	if (in.getBytes(buffer, 8) < 8) {
		m_fail("failed to read file size");
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
		m_fail("failed to read whether the huffman table tree node is a leaf");
	}
	if (isLeaf) {
		Byte c;
		if (in.getBytes(&c, 1)) {
			return new TreeNode(c);
		} else {
			m_fail("failed to read the huffman table tree node's corresponding byte");
		}
	}
	const auto left = m_readNode(in);
	const auto right = m_readNode(in);
	return new TreeNode(left, right);
}

void yac::Extractor::m_decode(ByteSource & in, ByteSink & out) {
	if (m_tree->m_isLeaf) {
		for (unsigned long long i = 0; i < m_fileSize; ++i) {
			if (!out.putBytes(&m_tree->m_value, 1)) {
				m_fail("failed to write extracted data");
			}
		}
		return;
	}

	Byte buf;
	int used = 8;
	long long totalReads{};
	for (unsigned long long i = 0; i < m_fileSize; ++i) {
		TreeNode * node = m_tree;
		while (!node->m_isLeaf) {
			if (used == 8) {
				used = 0;
				++totalReads;
				if (in.getBytes(&buf, 1) < 1) {
					std::cout << "Failed while attempting to perform the read #" << totalReads << '\n';
					std::cout.flush();
					m_fail("failed to read some data to decode");
				}
			}
			if (buf & (1 << (7 - used++))) {
				node = node->m_right;
			} else {
				node = node->m_left;
			}
		}
		if (!out.putBytes(&node->m_value, 1)) {
			m_fail("failed to write some decoded data");
		};
	}
}

