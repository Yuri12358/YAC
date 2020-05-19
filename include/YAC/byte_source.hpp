#ifndef YAC_BYTE_SOURCE_HPP
#define YAC_BYTE_SOURCE_HPP

#include<YAC/byte.hpp>

namespace yac {
	class ByteSource {
	public:
		// returns the count of actually given bytes
		virtual int getBytes(Byte * dest, int count) = 0;
		virtual void reset() = 0;
		virtual ~ByteSource() = default;
	};
}

#endif // YAC_BYTE_SOURCE_HPP

