#pragma once
#include "toy/common.h"
prelude;

namespace toy {

using std::bit_cast;
template <std::integral T> fun bit_ceil(T t) noexcept {
  return std::bit_ceil(std::make_unsigned_t<T>(t));
}
template <std::integral T> [[nodiscard]] fun bit_floor(T t) noexcept {
  return std::bit_floor(std::make_unsigned_t<T>(t));
}
template <std::integral T> [[nodiscard]] fun bit_width(T t) noexcept {
  return std::bit_width(std::make_unsigned_t<T>(t));
}
template <std::integral T> [[nodiscard]] fun countl_one(T t) noexcept {
  return std::countl_one(std::make_unsigned_t<T>(t));
}
template <std::integral T> [[nodiscard]] fun countl_zero(T t) noexcept {
  return std::countl_zero(std::make_unsigned_t<T>(t));
}
template <std::integral T> [[nodiscard]] fun countr_one(T t) noexcept {
  return std::countr_one(std::make_unsigned_t<T>(t));
}
template <std::integral T> [[nodiscard]] fun countr_zero(T t) noexcept {
  return std::countr_zero(std::make_unsigned_t<T>(t));
}
template <std::integral T> [[nodiscard]] fun has_single_bit(T t) noexcept {
  return std::has_single_bit(std::make_unsigned_t<T>(t));
}
template <std::integral T> [[nodiscard]] fun popcount(T t) noexcept {
  return std::popcount(std::make_unsigned_t<T>(t));
}
template <std::integral T> [[nodiscard]] fun log(T t) noexcept {
  return std::bit_width(std::make_unsigned_t<T>(ensure(t))) - 1;
}
template <std::integral T> [[nodiscard]] fun rotl(T t, int cnt) noexcept {
  return std::rotl(std::make_unsigned_t<T>(t), cnt);
}
template <std::integral T> [[nodiscard]] fun rotr(T t, int cnt) noexcept {
  return std::rotr(std::make_unsigned_t<T>(t), cnt);
}

} // namespace toy
