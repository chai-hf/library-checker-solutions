#pragma once
#include <cstdio>
#include <immintrin.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <toy/common.h>
#include <unistd.h>
using namespace toy;
prelude;

struct rd {
  static def all_digit(u32 x) -> bool {
    x &= 0xf0f0f0f0;
    return !x;
  }

  static def all_digit(u64 x) -> bool {
    x &= 0xf0f0f0f0f0f0f0f0;
    return !x;
  }

  static def all_digit(u128 x) -> bool {
    x &= u128(0xf0f0f0f0f0f0f0f0) << 64 | 0xf0f0f0f0f0f0f0f0;
    return !x;
  }

  static def parse_from(u32 x) -> u32 {
    x = (x * 10 + (x >> 8)) & 0x00ff00ff;
    return (x * 100 + (x >> 16)) & 0x0000ffff;
  }

  static def parse_from(u64 x) -> u32 {
    x = (x * 10 + (x >> 8)) & 0x00ff00ff00ff00ff;
    x = (x * 100 + (x >> 16)) & 0x0000ffff0000ffff;
    return u32(x * 10000 + (x >> 32));
  }

  static def parse_from(u128 x) -> u64 {
    u64 l = u64(x);
    u64 h = x >> 64;
    l = (l * 10 + (l >> 8)) & 0x00ff00ff00ff00ff;
    h = (h * 10 + (h >> 8)) & 0x00ff00ff00ff00ff;
    l = (l * 100 + (l >> 16)) & 0x0000ffff0000ffff;
    h = (h * 100 + (h >> 16)) & 0x0000ffff0000ffff;
    l = (l * 10000 + (l >> 32)) & 0x00000000ffffffff;
    h = (h * 10000 + (h >> 32)) & 0x00000000ffffffff;
    return l * 100000000 + h;
  }

  template <typename T> def parse_remaining(T x) -> T {
    for (; *p >= '0'; ++p) x = x * 10 + *p - '0';
    return ++p, x;
  }

  char *p;
#ifdef LOCAL
  inline static char *addr;
  inline static usize len;
#endif

  rd() noexcept {
    struct stat st;
    fstat(0, &st);
    p = (char *)mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, 0, 0);
#ifdef LOCAL
    addr = p, len = st.st_size;
#endif
  }
#ifdef LOCAL
  ~rd() { munmap(addr, len); }
#endif

  def skip(int n) -> void { p += n; }

  def ch() -> u8 {
    u8 x = *p++;
    return ++p, x;
  }

  def u1() -> u32 {
    u32 x = *p++ - '0';
    return ++p, x;
  }

  def ub() -> u32 { return parse_remaining(*p++ - '0'); }

  def uh() -> u32 {
    u32 x;
    std::memcpy(&x, p, sizeof(x));
    x ^= 0x30303030;
    if (all_digit(x)) {
      p += sizeof(x);
      return parse_remaining(parse_from(x));
    }
    return ub();
  }

  def u26() -> u32 {
    u64 x;
    std::memcpy(&x, p, sizeof(x));
    x ^= 0x3030303030303030;
    let d = std::countr_zero(x & 0xf0f0f0f0f0f0f0f0) >> 3;
    p += d + 1;
    return parse_from(x << ((sizeof(x) - d) << 3));
  }

  def uw() -> u32 {
    u64 x;
    std::memcpy(&x, p, sizeof(x));
    x ^= 0x3030303030303030;
    if (all_digit(x)) {
      p += sizeof(x);
      return parse_remaining(parse_from(x));
    }
    return u26();
  }

  def u53() -> u64 {
    u128 x;
    std::memcpy(&x, p, sizeof(x));
    x ^= u128(0x3030303030303030) << 64 | 0x3030303030303030;
    let d = (all_digit(u64(x))
                 ? std::countr_zero(0xf0f0f0f0f0f0f0f0 & u64(x >> 64)) + 64
                 : std::countr_zero(0xf0f0f0f0f0f0f0f0 & u64(x))) >>
            3;
    p += d + 1;
    return parse_from(x << ((sizeof(x) - d) << 3));
  }

  def ud() -> u64 {
    u128 x;
    std::memcpy(&x, p, sizeof(x));
    x ^= u128(0x3030303030303030) << 64 | 0x3030303030303030;
    if (all_digit(x)) {
      p += sizeof(x);
      return parse_remaining(parse_from(x));
    }
    return u53();
  }

  def i1() -> i32 { return *p == '-' ? ++p, -u1() : u1(); }
  def ib() -> i32 { return *p == '-' ? ++p, -ub() : ub(); }
  def ih() -> i32 { return *p == '-' ? ++p, -uh() : uh(); }
  def iw() -> i32 { return *p == '-' ? ++p, -uw() : uw(); }
  def id() -> i64 { return *p == '-' ? ++p, -ud() : ud(); }
};

struct wt {
  static fun low = [] {
    std::array<std::array<char, 4>, 10000> res;
    for (int i = 0; i < 10000; ++i) {
      res[i][0] = '0' + i / 1000 % 10;
      res[i][1] = '0' + i / 100 % 10;
      res[i][2] = '0' + i / 10 % 10;
      res[i][3] = '0' + i % 10;
    }
    return res;
  }();
  static fun pos = [] {
    std::array<std::array<char, 4>, 10000> res;
    for (int i = 0; i < 10000; ++i) {
      res[i][0] = '0' + i / 1000 % 10;
      res[i][1] = '0' + i / 100 % 10;
      res[i][2] = '0' + i / 10 % 10;
      res[i][3] = '0' + i % 10;
      if (i < 1000) res[i][0] = ' ';
      if (i < 100) res[i][1] = ' ';
      if (i < 10) res[i][2] = ' ';
    }
    return res;
  }();

  inline static char buf[1 << 20];
  static fun sentinel = buf + sizeof(buf) - 50;
  char *p = buf;
  def flush() -> void { write(1, buf, p - buf), p = buf; }
  def print_low(u64 x) -> void { std::memcpy(p, &low[x], 4), p += 4; }
  def print_pos(u64 x) -> void { std::memcpy(p, &pos[x], 4), p += 4; }
  ~wt() { flush(); }

  def puts(const char *src, int n) -> void {
    if (sentinel < p + n) flush();
    std::memcpy(p, src, n), p += n;
  }

  def u1(u8 x) -> void {
    toy_assert(x < 10);
    *p++ = ' ';
    *p++ = '0' + x;
  }

  def uw(u32 x) -> void {
    if (sentinel < p) flush();
    *p++ = ' ';
    if (x > 9999'9999) {
      print_pos(x / 10000 / 10000);
      print_low(x / 10000 % 10000);
      print_low(x % 10000);
    } else if (x > 9999) {
      print_pos(x / 10000);
      print_low(x % 10000);
    } else {
      print_pos(x);
    }
  }

  def ud(u64 x) -> void {
    if (sentinel < p) flush();
    *p++ = ' ';
    if (x > 9999'9999'9999'9999) {
      print_pos(x / 10000 / 10000 / 10000 / 10000);
      print_low(x / 10000 / 10000 / 10000 % 10000);
      print_low(x / 10000 / 10000 % 10000);
      print_low(x / 10000 % 10000);
      print_low(x % 10000);
    } else if (x > 9999'9999'9999) {
      print_pos(x / 10000 / 10000 / 10000);
      print_low(x / 10000 / 10000 % 10000);
      print_low(x / 10000 % 10000);
      print_low(x % 10000);
    } else if (x > 9999'9999) {
      print_pos(x / 10000 / 10000);
      print_low(x / 10000 % 10000);
      print_low(x % 10000);
    } else if (x > 9999) {
      print_pos(x / 10000);
      print_low(x % 10000);
    } else {
      print_pos(x);
    }
  }
};

#ifdef LOCAL
int main(int argc, char *argv[]) {
  extern int Main();
  if (argc < 2) return Main();
  std::string last = argv[argc - 1];
  if (last.ends_with("out")) {
    int half = (argc - 1) / 2;
    for (int i = 0; i < half; ++i) {
      freopen(argv[1 + i], "r", stdin);
      freopen(argv[1 + half + i], "w", stdout);
      ensure(!Main());
    }
  } else {
    for (int i = 1; i < argc; ++i) {
      freopen(argv[i], "r", stdin);
      ensure(!Main());
    }
  }
  return 0;
}
#define main Main
#endif
