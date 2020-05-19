#ifndef YAC_BYTE_SINK_HPP
#define YAC_BYTE_SINK_HPP

#include<YAC/byte.hpp>

namespace yac {
	class ByteSink {
	public:
		virtual ~ByteSink() = default;

		// returns whether the operation has succeeded
		virtual bool putBytes(const Byte * from, int count) = 0;

		virtual void finish() {}
	};
}

#endif // YAC_BYTE_SINK_HPP
