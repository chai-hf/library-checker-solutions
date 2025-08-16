#include <common.h>
#include <toy/bit.h>
prelude;

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  int m = n / 16 + 1;
  int len = log(m) + 1;
  int table[len][m];
  int val[n];
  int suf[n];
  int pre[n];
  for (int min, i = 0; i < n; ++i) {
    int id = i & 15;
    int value = pre[i] = val[i] = rd.uw();
    suf[i] = min = id ? std::min(min, value) : value;
    if (id == 15) table[0][i / 16] = suf[i];
  }
  for (int min, i = n - 2; i >= 0; --i) {
    int id = ~i & 15;
    int value = pre[i];
    pre[i] = min = id ? std::min(min, value) : value;
  }
  for (int i = 1; i < len; ++i) {
    for (int j = 0, k = 1 << (i - 1); k < m; ++j, ++k) {
      table[i][j] = std::min(table[i - 1][j], table[i - 1][k]);
    }
  }
  while (q--) {
    int l = rd.uh();
    int r = rd.uh() - 1;
    int L = l / 16;
    int R = r / 16;
    int ans;
    if (L == R) {
      ans = val[l];
      for (int i = l + 1; i <= r; ++i) ans = std::min(ans, val[i]);
    } else if (L == R - 1) {
      int p = pre[l];
      int s = suf[r];
      ans = std::min(p, s);
    } else {
      int p = pre[l];
      int s = suf[r];
      int k = log(R - L - 1);
      int a = table[k][L + 1];
      int b = table[k][R - (1 << k)];
      ans = std::min({p, s, a, b});
    }
    wt.ud(ans);
  }
  return 0;
}
