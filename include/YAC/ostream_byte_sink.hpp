#ifndef YAC_OSTREAM_BYTE_SINK_HPP
#define YAC_OSTREAM_BYTE_SINK_HPP

#include<YAC/byte_sink.hpp>
#include<ostream>

namespace yac {
	class OStreamByteSink : public ByteSink {
		std::ostream & m_stream;

	public:
		explicit OStreamByteSink(std::ostream & underlyingStream) noexcept;

		bool putBytes(const Byte* from, int count) override;
	};
}

#endif // YAC_OSTREAM_BYTE_SINK_HPP
