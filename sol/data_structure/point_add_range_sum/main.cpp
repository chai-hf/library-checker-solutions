#include <common.h>

u64 a[500001];

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  for (int i = 1; i <= n; ++i) a[i] = a[i - 1] + rd.uw();
  for (int i = n; i >= 1; --i) a[i] -= a[i - (i & -i)];
  while (q--) {
    auto t = rd.u1();
    if (t == 0) {
      int k = rd.uh() + 1;
      int x = rd.uw();
      for (; k <= n; k += k & -k) a[k] += x;
    }
    if (t == 1) {
      int l = rd.uh();
      int r = rd.uh();
      u64 ans = 0;
      for (int k = l; k > 0; k -= k & -k) ans -= a[k];
      for (int k = r; k > 0; k -= k & -k) ans += a[k];
      wt.ud(ans);
    }
  }
  return 0;
}
