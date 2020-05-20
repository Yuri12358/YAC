#include<YAC/block_sequence_byte_source.hpp>
#include<algorithm>

namespace yac {
	uint64_t BlockSequenceByteSource::m_readBlockSize() {
		uint64_t blockSize = 0;

		char buffer[8];
		m_stream.read(buffer, 8);
		for (int i = 7; i >= 0; ++i) {
			const unsigned char byte = buffer[i];
			blockSize <<= 8;
			blockSize += byte;
		}

		return blockSize;
	}

	BlockSequenceByteSource::BlockSequenceByteSource(std::istream& stream)
		: m_stream(stream)
		, m_bytesLeft(m_readBlockSize())
		, m_contentStart(stream.tellg()) {}

	void BlockSequenceByteSource::reset() {
		m_stream.clear();
		m_stream.seekg(m_contentStart);
	}

	int BlockSequenceByteSource::getBytes(Byte* dest, int count) {
		if (m_bytesLeft <= 0) {
			return 0;
		}

		const auto actualBytesCount = std::min<unsigned long long>(count, m_bytesLeft);
		const auto positionBefore = m_stream.tellg();
		m_stream.read(reinterpret_cast<char *>(dest), actualBytesCount);
		const auto positionAfter = m_stream.tellg();

		m_bytesLeft -= actualBytesCount;

		return positionAfter - positionBefore;
	}
}
