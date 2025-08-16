#include <common.h>
prelude;

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
    let l = rd.uh();
    let r = rd.uh();
    let sum = a[r] - a[l];
    wt.ud(sum);
  }
  return 0;
}
