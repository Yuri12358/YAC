#ifndef YAC_NON_COPYABLE_HPP
#define YAC_NON_COPYABLE_HPP

namespace yac {
	class NonCopyable {
	public:
		constexpr NonCopyable() noexcept = default;
		constexpr ~NonCopyable() noexcept = default;
		constexpr NonCopyable(const NonCopyable &) noexcept = delete;
		constexpr NonCopyable(NonCopyable &&) noexcept = default;
		constexpr NonCopyable & operator =(const NonCopyable &) noexcept = delete;
		constexpr NonCopyable & operator =(NonCopyable &&) noexcept = default;
	};
}

#endif // YAC_NON_COPYABLE_HPP
