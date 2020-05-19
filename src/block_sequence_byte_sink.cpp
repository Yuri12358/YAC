#include<YAC/block_sequence_byte_sink.hpp>

namespace yac {
	BlockSequenceByteSink::BlockSequenceByteSink(std::ostream & stream)
		: m_stream(stream)
		, m_blockStart(stream.tellp()) {
		// skip some bytes to reserve the place for the block header
		m_stream.seekp(sizeof(m_totalBytesWritten), std::ios::cur);
	}

	bool BlockSequenceByteSink::putBytes(const Byte* from, int count) {
		if (m_finished) {
			return false;
		}

		const auto positionBefore = m_stream.tellp();
		m_stream.write(reinterpret_cast<const char *>(from), count);
		const auto positionAfter = m_stream.tellp();

		m_totalBytesWritten += count;

		return (positionAfter - positionBefore) == count;
	}

	void BlockSequenceByteSink::finish() {
		if (m_finished) {
			return;
		}

		// go back to the block header
		const auto finishPosition = m_stream.tellp();
		m_stream.seekp(m_blockStart);

		// write down the block size
		for (unsigned i = 0; i < sizeof(m_totalBytesWritten); ++i) {
			const auto byte = static_cast<unsigned char>(m_totalBytesWritten & 0xff);
			m_stream.write(reinterpret_cast<const char *>(byte), 1);
			m_totalBytesWritten >>= 8;
		}

		// restore the finish position
		m_stream.seekp(finishPosition);

		m_finished = true;
	}
}
