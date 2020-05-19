#ifndef YAC_BLOCK_SEQUENCE_BYTE_SINK_HPP
#define YAC_BLOCK_SEQUENCE_BYTE_SINK_HPP

#include<YAC/byte_sink.hpp>
#include<YAC/non_copyable.hpp>
#include<ostream>

namespace yac {
	class BlockSequenceByteSink
		: public ByteSink
		, public NonCopyable {
		uint64_t m_totalBytesWritten = 0;
		std::ostream & m_stream;
		std::ostream::pos_type m_blockStart;
		bool m_finished = false;

	public:
		BlockSequenceByteSink(std::ostream & underlyingStream);

		bool putBytes(const Byte* from, int count) override;
		void finish() override;
	};
}

#endif // YAC_BLOCK_SEQUENCE_BYTE_SINK_HPP
