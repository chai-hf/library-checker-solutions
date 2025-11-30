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
} a[N];

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  for (int i = 0; i < n; ++i) {
    int k = i * 2 + 1;
    a[k] = {rd.uw(), rd.uw()};
    for (int j = 1; k & (j * 2); k -= j, j *= 2) {
      a[k - j] = a[k - j * 2] + a[k];
    }
  }
  while (q--) {
    auto t = rd.u1();
    if (t == 0) {
      int k = rd.uh() * 2 + 1;
      a[k] = {rd.uw(), rd.uw()};
      for (int j = 1;; j *= 2) {
        if (k & (j * 2)) {
          a[k - j] = a[k - j * 2] + a[k];
          k -= j;
        } else {
          if (k + j * 3 > n * 2) break;
          a[k + j] = a[k] + a[k + j * 2];
          k += j;
        }
      }
    }
    if (t == 1) {
      int l = rd.uh() * 2 + 1;
      int r = rd.uh() * 2 + 1;
      u32 x = a[l] + rd.uw();
      int k = (-1 << log(l ^ r)) & r;
      for (++l; l < k; l += l & -l) {
        x = a[l + (l & -l) / 2] + x;
      }
      for (--r; k < r;) {
        int t = 1 << log(r - k);
        x = a[k + t / 2] + x;
        k += t;
      }
      wt.uw(x);
    }
  }
  return 0;
}
