#pragma once

#include <bits/extc++.h>

#ifdef NDEBUG
#define toy_assert(cond) [[assume(cond)]]
#else
#define toy_assert(cond) assert(cond)
#endif

namespace toy {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using i128 = __int128;
using isize = std::ptrdiff_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using u128 = unsigned __int128;
using usize = std::size_t;

using f32 = float;
using f64 = double;
using f80 = long double;

template <typename T> constexpr auto ensure(T &&t) noexcept -> decltype(auto) {
  toy_assert(t);
  return std::forward<T>(t);
}

} // namespace toy
