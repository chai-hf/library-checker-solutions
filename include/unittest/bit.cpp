#include <toy/bit.h>

namespace toy {

static_assert(byteswap(u16{0x0001}) == 0x0100);
static_assert(byteswap(u16{0x0100}) == 0x0001);
static_assert(byteswap(u16{0x0203}) == 0x0302);
static_assert(byteswap(u16{0x0405}) == 0x0504);
static_assert(byteswap(u16{0x1234}) == 0x3412);

static_assert(bit_ceil(0) == 1);
static_assert(bit_ceil(1) == 1);
static_assert(bit_ceil(2) == 2);
static_assert(bit_ceil(3) == 4);
static_assert(bit_ceil(4) == 4);

static_assert(bit_floor(0) == 0);
static_assert(bit_floor(1) == 1);
static_assert(bit_floor(2) == 2);
static_assert(bit_floor(3) == 2);
static_assert(bit_floor(4) == 4);

static_assert(bit_width(0) == 0);
static_assert(bit_width(1) == 1);
static_assert(bit_width(2) == 2);
static_assert(bit_width(3) == 2);
static_assert(bit_width(4) == 3);

static_assert(countl_zero(0) == 32);
static_assert(countl_zero(1) == 31);
static_assert(countl_zero(2) == 30);
static_assert(countl_zero(3) == 30);
static_assert(countl_zero(4) == 29);

static_assert(countl_one(-0) == 0);
static_assert(countl_one(-1) == 32);
static_assert(countl_one(-2) == 31);
static_assert(countl_one(-3) == 30);
static_assert(countl_one(-4) == 30);

static_assert(countr_zero(0) == 32);
static_assert(countr_zero(1) == 0);
static_assert(countr_zero(2) == 1);
static_assert(countr_zero(3) == 0);
static_assert(countr_zero(4) == 2);

static_assert(countr_one(0) == 0);
static_assert(countr_one(1) == 1);
static_assert(countr_one(2) == 0);
static_assert(countr_one(3) == 2);
static_assert(countr_one(4) == 0);

static_assert(!has_single_bit(0));
static_assert(+has_single_bit(1));
static_assert(+has_single_bit(2));
static_assert(!has_single_bit(3));
static_assert(+has_single_bit(4));

static_assert(popcount(0) == 0);
static_assert(popcount(1) == 1);
static_assert(popcount(2) == 1);
static_assert(popcount(3) == 2);
static_assert(popcount(4) == 1);

static_assert(log(1) == 0);
static_assert(log(2) == 1);
static_assert(log(3) == 1);
static_assert(log(4) == 2);

static_assert(rotl(u8{1}, 0) == 0b00000001);
static_assert(rotl(u8{1}, 1) == 0b00000010);
static_assert(rotl(u8{1}, 2) == 0b00000100);
static_assert(rotl(u8{1}, 3) == 0b00001000);
static_assert(rotl(u8{1}, 4) == 0b00010000);

static_assert(rotr(u8{1}, 0) == 0b00000001);
static_assert(rotr(u8{1}, 1) == 0b10000000);
static_assert(rotr(u8{1}, 2) == 0b01000000);
static_assert(rotr(u8{1}, 3) == 0b00100000);
static_assert(rotr(u8{1}, 4) == 0b00010000);

}; // namespace toy
