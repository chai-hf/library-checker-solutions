#include <common.h>
#include <toy/bit.h>

namespace {

constexpr int N = 2e5;
constexpr int P = 998244353;

struct Node {
  u32 a, b, c;
  auto operator+(const Node &t) const -> Node {
    u32 x = u64(a) * t.a % P;
    u32 y = (u64(a) * t.b + b) % P;
    u32 z = (u64(t.a) * c + t.c) % P;
    return {x, y, z};
  }
} node[N * 2];
auto operator+(const Node &l, u32 r) -> u32 { return (u64(l.a) * r + l.b) % P; }
auto operator+(u32 l, const Node &r) -> u32 { return (u64(r.a) * l + r.c) % P; }
u32 a[N];
u32 b[N];
int head[N];
int size[N];
int depth[N];
int heavy[N];
int ances[N];
int parent[N];
int node2id[N];
int id;
struct {
  int to;
  int next;
} edge[N * 2];

auto build_step_1(int u, int p) -> void {
  size[u] = 1;
  for (int e = head[u]; e; e = edge[e].next) {
    int v = edge[e].to;
    if (v != p) {
      build_step_1(v, u);
      size[u] += size[v];
      if (heavy[u] == 0 || size[v] > size[heavy[u]]) {
        heavy[u] = v;
      }
    }
  }
}

auto build_step_2(int u, int w, int p, int d) -> void {
  int i = id++;
  node2id[u] = i;
  depth[i] = d;
  int anc = ances[i] = node2id[w];
  parent[i] = node2id[p];
  node[i * 2 + 1] = {a[u], b[u], b[u]};
  auto a = node + anc * 2;
  int k = (i - anc) * 2 + 1;
  for (int j = 1; k & (j * 2); k -= j, j *= 2) {
    a[k - j] = a[k - j * 2] + a[k];
  }
  if (int v = heavy[u]; v) {
    build_step_2(v, w, u, d + 1);
  }
  for (int e = head[u]; e; e = edge[e].next) {
    int v = edge[e].to;
    if (v != p && v != heavy[u]) {
      build_step_2(v, v, u, d + 1);
    }
  }
}

auto apply_1(int i, u32 x) -> u32 {
  int anc = ances[i];
  auto a = node + anc * 2;
  int r = (i - anc) * 2 + 2;
  for (; r; r -= r & -r) {
    x = a[r - (r & -r) / 2] + x;
  }
  return x;
}

auto apply_2(int i, u32 x) -> u32 {
  int anc = ances[i];
  auto a = node + anc * 2;
  int r = (i - anc) * 2 + 2;
  for (int k = 0; k < r;) {
    int t = 1 << log(r - k);
    x = x + a[k + t / 2];
    k += t;
  }
  return x;
}

auto apply_1(int i, int j, u32 x) -> u32 {
  int anc = ances[i];
  if (i == anc) return apply_1(j, x);
  auto a = node + anc * 2;
  int l = (i - anc) * 2 - 1;
  int r = (j - anc) * 2 + 3;
  int k = (-1 << log(l ^ r)) & r;
  for (--r; r > k; r -= r & -r) {
    x = a[r - (r & -r) / 2] + x;
  }
  for (++l; k > l;) {
    int t = 1 << log(k - l);
    x = a[k - t / 2] + x;
    k -= t;
  }
  return x;
}

auto apply_2(int i, int j, u32 x) -> u32 {
  int anc = ances[i];
  if (i == anc) return apply_2(j, x);
  auto a = node + anc * 2;
  int l = (i - anc) * 2 - 1;
  int r = (j - anc) * 2 + 3;
  int k = (-1 << log(l ^ r)) & r;
  for (++l; l < k; l += l & -l) {
    x = x + a[l + (l & -l) / 2];
  }
  for (--r; k < r;) {
    int t = 1 << log(r - k);
    x = x + a[k + t / 2];
    k += t;
  }
  return x;
}

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  for (int i = 0; i < n; ++i) a[i] = rd.uw(), b[i] = rd.uw();
  for (int i = 1; i < n; ++i) {
    int u = rd.uh();
    int v = rd.uh();
    edge[i * 2 | 0] = {v, head[u]}, head[u] = i * 2 | 0;
    edge[i * 2 | 1] = {u, head[v]}, head[v] = i * 2 | 1;
  }
  build_step_1(0, 0);
  build_step_2(0, 0, 0, 0);
  std::memset(size, 0, sizeof(int) * n);
  for (int i = 0; i < n; ++i) size[ances[i]]++;
  while (q--) {
    auto t = rd.u1();
    if (t == 0) {
      int i = node2id[rd.uh()];
      u32 c = rd.uw();
      u32 d = rd.uw();
      int anc = ances[i];
      auto a = node + anc * 2;
      int n = size[anc];
      int k = (i - anc) * 2 + 1;
      a[k] = {c, d, d};
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
      int u = node2id[rd.uh()];
      int v = node2id[rd.uh()];
      u32 x = rd.uw();
      int vec[20], top{};
      while (ances[u] != ances[v]) {
        if (depth[ances[u]] > depth[ances[v]]) {
          x = apply_1(u, x);
          u = parent[ances[u]];
        } else {
          vec[top++] = v;
          v = parent[ances[v]];
        }
      }
      if (u > v) {
        x = apply_1(v, u, x);
      } else {
        x = apply_2(u, v, x);
      }
      while (top--) {
        x = apply_2(vec[top], x);
      }
      wt.uw(x);
    }
  }
  return 0;
}
