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
  static fun all_digit(u32 x) -> bool {
    x ^= 0x30303030;
    x &= 0xf0f0f0f0;
    return !x;
  }

  static fun all_digit(u64 x) -> bool {
    x ^= 0x3030303030303030;
    x &= 0xf0f0f0f0f0f0f0f0;
    return !x;
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

  def ub() -> u32 {
    u32 x = *p++ - '0';
    for (; *p >= '0'; ++p) x = x * 10 + *p - '0';
    return ++p, x;
  }

  def uh() -> u32 {
    u32 x{};
    u32 a;
    std::memcpy(&a, p, sizeof(a));
    if (all_digit(a)) {
      a ^= 0x30303030;
      a = (a * 10 + (a >> 8)) & 0x00ff00ff;
      a = (a * 100 + (a >> 16)) & 0x0000ffff;
      x = a, p += sizeof(a);
    }
    for (; *p >= '0'; ++p) x = x * 10 + *p - '0';
    return ++p, x;
  }

  def uw() -> u32 {
    u32 x{};
    u64 a;
    std::memcpy(&a, p, sizeof(a));
    if (all_digit(a)) {
      a ^= 0x3030303030303030;
      a = (a * 10 + (a >> 8)) & 0x00ff00ff00ff00ff;
      a = (a * 100 + (a >> 16)) & 0x0000ffff0000ffff;
      a = (a * 10000 + (a >> 32)) & 0x00000000ffffffff;
      x = u32(a), p += sizeof(a);
    }
    for (; *p >= '0'; ++p) x = x * 10 + *p - '0';
    return ++p, x;
  }

  def ud() -> u64 {
    u64 x{};
    union {
      u128 ch;
      u64 d[2];
    };
    std::memcpy(&ch, p, sizeof(ch));
    u64 a = d[0], b = d[1];
    if (all_digit(a) && all_digit(b)) {
      a ^= 0x3030303030303030;
      b ^= 0x3030303030303030;
      a = (a * 10 + (a >> 8)) & 0x00ff00ff00ff00ff;
      b = (b * 10 + (b >> 8)) & 0x00ff00ff00ff00ff;
      a = (a * 100 + (a >> 16)) & 0x0000ffff0000ffff;
      b = (b * 100 + (b >> 16)) & 0x0000ffff0000ffff;
      a = (a * 10000 + (a >> 32)) & 0x00000000ffffffff;
      b = (b * 10000 + (b >> 32)) & 0x00000000ffffffff;
      x = a * 100000000 + b, p += sizeof(ch);
    }
    for (; *p >= '0'; ++p) x = x * 10 + *p - '0';
    return ++p, x;
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
