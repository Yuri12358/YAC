#ifndef YAC_SIZE_TYPES_HPP
#define YAC_SIZE_TYPES_HPP

#include<YAC/strong_typedef.hpp>

#define CONCAT_IMPL(X, Y) X ## Y
#define CONCAT(X, Y) CONCAT_IMPL(X, Y)

#define GENERATE_BINARY_OPERATOR(Op, LHS, RHS, Result) \
	inline Result operator Op(LHS lhs, RHS rhs) { \
		return Result{ lhs.value Op rhs.value }; \
	}

#define GENERATE_OPERATORS_WITH_OFFSET(Op, SomeStrongTypedef, OffsetType) \
	GENERATE_BINARY_OPERATOR(Op, SomeStrongTypedef, OffsetType, SomeStrongTypedef) \
	inline SomeStrongTypedef & operator CONCAT(Op, =) (SomeStrongTypedef & lhs, OffsetType rhs) { \
		lhs.value CONCAT(Op, =) rhs.value; \
		return lhs; \
	}

#define GENERATE_OPERATORS(Op, SomeStrongTypedef) GENERATE_OPERATORS_WITH_OFFSET(Op, SomeStrongTypedef, SomeStrongTypedef)

namespace yac {
	namespace tags {
		struct CompressedSize;
		struct UncompressedSize;
		struct PositionInArchive;
		struct HuffmanTreeSize;
		struct CompressedContentSize;
	}

	using Size = unsigned long long;

	using CompressedSize = StrongTypedef<Size, tags::CompressedSize>;
	GENERATE_OPERATORS(+, CompressedSize)

	using UncompressedSize = StrongTypedef<Size, tags::UncompressedSize>;
	GENERATE_OPERATORS(+, UncompressedSize)

	using PositionInArchive = StrongTypedef<Size, tags::PositionInArchive>;

	using HuffmanTreeSize = StrongTypedef<Size, tags::HuffmanTreeSize>;
	GENERATE_OPERATORS(+, HuffmanTreeSize)

	using CompressedContentSize = StrongTypedef<Size, tags::CompressedContentSize>;
	GENERATE_OPERATORS(+, CompressedContentSize)

	GENERATE_BINARY_OPERATOR(+, HuffmanTreeSize, CompressedContentSize, CompressedSize)
	GENERATE_BINARY_OPERATOR(+, CompressedContentSize, HuffmanTreeSize, CompressedSize)
	GENERATE_BINARY_OPERATOR(-, CompressedSize, CompressedContentSize, HuffmanTreeSize)
	GENERATE_BINARY_OPERATOR(-, CompressedSize, HuffmanTreeSize, CompressedContentSize)
}

#endif // YAC_SIZE_TYPES_HPP
