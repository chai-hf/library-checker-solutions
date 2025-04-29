#include <common.h>
#include <toy/bit.h>
prelude;

namespace {

constexpr int N = 5e5;

int parent[N];
int node2id[N];
int id2node[N];

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
#ifdef LOCAL
  std::memset(node2id, 0, 4 * n);
#endif
  for (int i = 1; i < n; ++i) parent[i] = rd.uh();
  for (int i = n - 1; i > 0; --i) node2id[parent[i]] += node2id[i] + 1;
  for (int i = 1; i < n; ++i) {
    int p = parent[i];
    int &u = node2id[i];
    int &w = node2id[p];
    int t = w - u - 1;
    u = w, w = t;
  }
  for (int i = 1; i < n; ++i) id2node[node2id[i]] = parent[i];
  int m = n / 64 + 1;
  int len = log(m) + 1;
  int table[len][m];
  int suf[n];
  int pre[n];
  for (int min, i = 0; i < n; ++i) {
    int id = i & 63;
    int value = pre[i] = id2node[i];
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
    int u = node2id[rd.uh()];
    int v = node2id[rd.uh()];
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
      w = id2node[l];
      for (int i = l + 1; i <= r; ++i) w = std::min(w, id2node[i]);
    }
    wt.uw(w);
  }
  return 0;
}
