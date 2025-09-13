#include <common.h>

namespace {

u64 a[500001];

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  for (int i = 1; i <= n; ++i) a[i] = a[i - 1] + rd.uw();
  while (q--) {
    auto l = rd.uh();
    auto r = rd.uh();
    auto sum = a[r] - a[l];
    wt.ud(sum);
  }
  return 0;
}
