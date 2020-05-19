#include<YAC/ostream_byte_sink.hpp>

namespace yac {
	OStreamByteSink::OStreamByteSink(std::ostream & underlyingStream) noexcept
		: m_stream(underlyingStream) {}

	bool OStreamByteSink::putBytes(const Byte* from, int count) {
		return m_stream.write(reinterpret_cast<const char *>(from), count).good();
	}
}
