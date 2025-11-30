#include <common.h>
#include <toy/bit.h>

namespace {

constexpr int N = 5e5;

int head[N];
struct {
  int to;
  int next;
} edge[N * 2];
int p[N];
int l[N];
int r[N];
int a[N];
u64 b[N + 1];
u64 c[N + 2];

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  for (int i = 0; i < n; ++i) a[i] = rd.uw();
  for (int i = 1; i < n; ++i) {
    int u = rd.uh();
    int v = rd.uh();
    edge[i * 2 | 0] = {v, head[u]}, head[u] = i * 2 | 0;
    edge[i * 2 | 1] = {u, head[v]}, head[v] = i * 2 | 1;
  }
  p[0] = -1, l[0] = 1, b[1] = c[1] = a[0];
  for (int u = 0, i = 1; u >= 0;) {
    if (int e = head[u]; e) {
      auto [v, x] = edge[e];
      head[u] = x;
      if (v == p[u]) continue;
      p[v] = u;
      l[v] = ++i;
      c[i] += b[i] = a[v];
      u = v;
    } else {
      c[r[u] = i + 1] -= a[u];
      u = p[u];
    }
  }
  u64 sum = 0;
  for (int i = 1; i <= n; ++i) c[i] = sum += c[i];
  for (int i = n; i >= 1; --i) c[i] -= c[i - (i & -i)];
  int val[n + 1];
  for (int i = 1; i < n; ++i) val[l[i]] = l[p[i]];
  ++n;
  int m = n / 64 + 1;
  int len = log(m) + 1;
  int table[len][m];
  int suf[n];
  int pre[n];
  for (int min, i = 0; i < n; ++i) {
    int id = i & 63;
    int value = pre[i] = val[i];
    suf[i] = min = id ? std::min(min, value) : value;
    if (id == 63) table[0][i / 64] = suf[i];
  }
  for (int min, i = n - 2; i >= 0; --i) {
    int id = ~i & 63;
    int value = pre[i];
    pre[i] = min = id ? std::min(min, value) : value;
  }
  for (int i = 1; i < len; ++i) {
    for (int j = 0, k = 1 << (i - 1); k < m; ++j, ++k) {
      table[i][j] = std::min(table[i - 1][j], table[i - 1][k]);
    }
  }
  while (q--) {
    auto t = rd.u1();
    if (t == 0) {
      int k = rd.uh();
      int x = rd.uw();
      int u = l[k];
      int v = r[k];
      b[u] += x;
      for (; u <= n; u += u & -u) c[u] += x;
      for (; v <= n; v += v & -v) c[v] -= x;
    }
    if (t == 1) {
      int u = l[rd.uh()];
      int v = l[rd.uh()];
      if (u == v) {
        wt.ud(b[u]);
        continue;
      }
      int l = std::min(u, v) + 1;
      int r = std::max(u, v);
      int L = l / 64;
      int R = r / 64;
      int w;
      if (L < R - 1) {
        int p = pre[l];
        int s = suf[r];
        w = std::min(p, s);
        int k = log(R - L - 1);
        int a = table[k][L + 1];
        int b = table[k][R - (1 << k)];
        int tmp = std::min(a, b);
        w = std::min(w, tmp);
      } else if (L == R - 1) {
        int p = pre[l];
        int s = suf[r];
        w = std::min(p, s);
      } else {
        w = val[l];
        for (int i = l + 1; i <= r; ++i) w = std::min(w, val[i]);
      }
      u64 sum = b[w];
      --l;
      for (; l; l -= l & -l) sum += c[l];
      for (; r; r -= r & -r) sum += c[r];
      for (; w; w -= w & -w) sum -= c[w] * 2;
      wt.ud(sum);
    }
  }
  return 0;
}
