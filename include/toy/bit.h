#pragma once
#include "toy/common.h"

namespace toy {

using std::bit_cast;
template <std::integral T> constexpr auto bit_ceil(T t) noexcept {
  return std::bit_ceil(std::make_unsigned_t<T>(t));
}
template <std::integral T>
[[nodiscard]] constexpr auto bit_floor(T t) noexcept {
  return std::bit_floor(std::make_unsigned_t<T>(t));
}
template <std::integral T>
[[nodiscard]] constexpr auto bit_width(T t) noexcept {
  return std::bit_width(std::make_unsigned_t<T>(t));
}
template <std::integral T>
[[nodiscard]] constexpr auto countl_one(T t) noexcept {
  return std::countl_one(std::make_unsigned_t<T>(t));
}
template <std::integral T>
[[nodiscard]] constexpr auto countl_zero(T t) noexcept {
  return std::countl_zero(std::make_unsigned_t<T>(t));
}
template <std::integral T>
[[nodiscard]] constexpr auto countr_one(T t) noexcept {
  return std::countr_one(std::make_unsigned_t<T>(t));
}
template <std::integral T>
[[nodiscard]] constexpr auto countr_zero(T t) noexcept {
  return std::countr_zero(std::make_unsigned_t<T>(t));
}
template <std::integral T>
[[nodiscard]] constexpr auto has_single_bit(T t) noexcept {
  return std::has_single_bit(std::make_unsigned_t<T>(t));
}
template <std::integral T> [[nodiscard]] constexpr auto popcount(T t) noexcept {
  return std::popcount(std::make_unsigned_t<T>(t));
}
template <std::integral T> [[nodiscard]] constexpr auto log(T t) noexcept {
  return std::bit_width(std::make_unsigned_t<T>(ensure(t))) - 1;
}
template <std::integral T>
[[nodiscard]] constexpr auto rotl(T t, int cnt) noexcept {
  return std::rotl(std::make_unsigned_t<T>(t), cnt);
}
template <std::integral T>
[[nodiscard]] constexpr auto rotr(T t, int cnt) noexcept {
  return std::rotr(std::make_unsigned_t<T>(t), cnt);
}

} // namespace toy
