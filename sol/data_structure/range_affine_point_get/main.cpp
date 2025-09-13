#include <common.h>
#include <toy/bit.h>

namespace {

constexpr int N = 1e6;
constexpr int P = 998244353;

struct node {
  u32 a, b;
  auto operator+(node t) -> node {
    return {u32(u64(t.a) * a % P), u32((u64(t.a) * b + t.b) % P)};
  }
  auto operator+(u32 x) -> u32 { return (u64(a) * x + b) % P; }
  auto operator+=(node t) -> void { *this = *this + t; }
} a[N];

void pushdown(int k) {
  for (int i = log(k) - 1; i >= 0; --i) {
    node t = a[k >> i >> 1];
    a[k >> i >> 1] = node{1, 0};
    a[k >> i ^ 0] += t;
    a[k >> i ^ 1] += t;
  }
}

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  for (int i = 0; i < n; ++i) a[i] = {1, 0}, a[n + i].b = rd.uw();
  while (q--) {
    auto t = rd.u1();
    if (t == 0) {
      int l = n + rd.uh();
      int r = n + rd.uh() - 1;
      node x = {rd.uw(), rd.uw()};
      pushdown(l--);
      pushdown(r++);
      int k = log(l ^ r);
      for (int t = ~l & ~(-1 << k), i; t > 0; t -= 1 << i) {
        i = log(t);
        a[l >> i ^ 1] += x;
      }
      for (int t = +r & ~(-1 << k), i; t > 0; t -= 1 << i) {
        i = log(t);
        a[r >> i ^ 1] += x;
      }
    }
    if (t == 1) {
      int k = n + rd.uh();
      u32 x = a[k].b;
      for (k /= 2; k > 0; k /= 2) x = a[k] + x;
      wt.uw(x);
    }
  }
  return 0;
}
