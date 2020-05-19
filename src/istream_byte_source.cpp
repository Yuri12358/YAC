#include<YAC/istream_byte_source.hpp>

namespace yac {
	IStreamByteSource::IStreamByteSource(std::istream & stream)
		: stream(stream) {}

	int IStreamByteSource::getBytes(Byte * dest, int count) {
		const auto before = stream.tellg();
		stream.read(reinterpret_cast<char *>(dest), count);
		const auto after = stream.tellg();

		return after - before;
	}

	void IStreamByteSource::reset() {
		stream.clear();
		stream.seekg(0);
	}
}

