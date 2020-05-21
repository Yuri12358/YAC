#ifndef YAC_STRONG_TYPEDEF_HPP
#define YAC_STRONG_TYPEDEF_HPP

namespace yac {
	template<class T, class Tag>
	struct StrongTypedef {
		using ValueType = T;
		using TagType = Tag;

		T value;
	};
}

#endif // YAC_STRONG_TYPEDEF_HPP
