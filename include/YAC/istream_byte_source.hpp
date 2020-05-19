#ifndef YAC_ISTREAM_BYTE_SOURCE_HPP
#define YAC_ISTREAM_BYTE_SOURCE_HPP

#include<YAC/byte_source.hpp>
#include<istream>

namespace yac {
	class IStreamByteSource : public ByteSource {
		std::istream & stream;

	public:
		IStreamByteSource(std::istream & underlyingStream);
		int getBytes(Byte * dest, int count) override;
		void reset() override;
	};
}

#endif // YAC_ISTREAM_BYTE_SOURCE_HPP

