#ifndef YAC_BLOCK_SEQUENCE_BYTE_SOURCE_HPP
#define YAC_BLOCK_SEQUENCE_BYTE_SOURCE_HPP

#include<YAC/byte_source.hpp>
#include<YAC/non_copyable.hpp>
#include<istream>

namespace yac {
	class BlockSequenceByteSource
		: public ByteSource
		, public NonCopyable {
		std::istream & m_stream;
		uint64_t m_bytesLeft = 0;
		std::istream::pos_type m_contentStart;

		uint64_t m_readBlockSize();

	public:
		BlockSequenceByteSource(std::istream & underlyingStream);

		void reset() override;
		int getBytes(Byte* dest, int count) override;
	};
}

#endif // YAC_BLOCK_SEQUENCE_BYTE_SOURCE_HPP
