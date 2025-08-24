#include <common.h>
#include <toy/bit.h>

namespace {

constexpr int N = 1e6;
constexpr int P = 998244353;

struct node {
  u32 a, b;
  auto operator+(node t) -> node {
    return {u32(u64(a) * t.a % P), u32((u64(a) * t.b + b) % P)};
  }
  auto operator+(u32 x) -> u32 { return (u64(a) * x + b) % P; }
} a[N];

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  for (int i = 0; i < n; ++i) a[n + n - 1 - i] = {rd.uw(), rd.uw()};
  for (int i = n - 1; i >= 1; --i) a[i] = a[i * 2] + a[i * 2 + 1];
  while (q--) {
    auto t = rd.u1();
    if (t == 0) {
      int k = n + n - 1 - rd.uh();
      a[k] = {rd.uw(), rd.uw()};
      for (k /= 2; k > 0; k /= 2) a[k] = a[k * 2] + a[k * 2 + 1];
    }
    if (t == 1) {
      int r = n + n - rd.uh();
      int l = n + n - 1 - rd.uh();
      u32 x = rd.uw();
      int k = log(l ^ r);
      int R = r >> k;
      for (r = r >> __builtin_ctz(r) ^ 1; r > R; r = r >> __builtin_ctz(r) ^ 1)
        x = a[r] + x;
      for (int t = ~l & ~(-1 << k), i; t > 0; t -= 1 << i) {
        i = log(t);
        x = a[l >> i ^ 1] + x;
      }
      wt.uw(x);
    }
  }
  return 0;
}
