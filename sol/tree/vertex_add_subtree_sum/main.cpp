#include <common.h>
prelude;

namespace {

constexpr int N = 5e5;

int a[N];
int p[N];
int l[N];
u64 b[N + 1];

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
#ifdef LOCAL
  std::memset(l, 0, 4 * n);
#endif
  for (int i = 0; i < n; ++i) a[i] = rd.uw();
  for (int i = 1; i < n; ++i) p[i] = rd.uh();
  for (int i = n - 1; i > 0; --i) l[p[i]] += (l[i] += 1);
  l[0] = p[0] = n;
  for (int i = 1; i < n; ++i) {
    int j = p[i];
    int li = l[i];
    int lj = l[j];
    l[i] = p[i] = lj;
    l[j] -= li;
  }
  for (int i = 0; i < n; ++i) b[l[i]] = a[i];
  for (int i = 1; i <= n; ++i) b[i] += b[i - 1];
  for (int i = n; i >= 1; --i) b[i] -= b[i - (i & -i)];
  while (q--) {
    let t = rd.u1();
    if (t == 0) {
      int k = l[rd.uh()];
      int x = rd.uw();
      for (; k <= n; k += k & -k) b[k] += x;
    }
    if (t == 1) {
      int x = rd.uh();
      int L = l[x] - 1;
      int R = p[x];
      u64 sum = 0;
      for (; L > 0; L -= L & -L) sum -= b[L];
      for (; R > 0; R -= R & -R) sum += b[R];
      wt.ud(sum);
    }
  }
  return 0;
}
