#include "common.h"
#include "toy/bit.h"
prelude;

namespace {

constexpr int N = 2e5;
constexpr u32 P = 998244353;
struct Node {
  u32 a, b, c;
  Node operator+(const Node &t) const {
    u32 x = u64(a) * t.a % P;
    u32 y = (u64(a) * t.b + b) % P;
    u32 z = (u64(t.a) * c + t.c) % P;
    return {x, y, z};
  }
} node[N * 2];
u32 operator+(const Node &l, u32 r) { return (u64(l.a) * r + l.b) % P; }
u32 operator+(u32 l, const Node &r) { return (u64(r.a) * l + r.c) % P; }
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

def build_step_1(int u, int p) -> void {
  size[u] = 1;
  for (int e = head[u]; e; e = edge[e].next) {
    int v = edge[e].to;
    if (v != p) {
      build_step_1(v, u);
      size[u] += size[v];
      if (heavy[u] == 0 || size[v] > size[heavy[u]]) { heavy[u] = v; }
    }
  }
}

def build_step_2(int u, int w, int p, int d) -> void {
  int i = id++;
  node2id[u] = i;
  node[i] = {a[u], b[u], b[u]};
  depth[i] = d;
  ances[i] = node2id[w];
  parent[i] = node2id[p];
  if (int v = heavy[u]; v) { build_step_2(v, w, u, d + 1); }
  for (int e = head[u]; e; e = edge[e].next) {
    int v = edge[e].to;
    if (v != p && v != heavy[u]) { build_step_2(v, v, u, d + 1); }
  }
}

} // namespace

int main() {
  rd rd;
  wt wt;
  let n = rd.uh();
  let q = rd.uh();
  for (int i = 0; i < n; ++i) a[i] = rd.uw(), b[i] = rd.uw();
  for (int i = 1; i < n; ++i) {
    int u = rd.uh();
    int v = rd.uh();
    edge[i * 2 | 0] = {v, head[u]}, head[u] = i * 2 | 0;
    edge[i * 2 | 1] = {u, head[v]}, head[v] = i * 2 | 1;
  }
  build_step_1(0, 0);
  build_step_2(0, 0, 0, 0);
  std::memcpy(node + n, node, sizeof(Node) * n);
  for (int i = n - 1; i > 0; --i) node[i] = node[i * 2] + node[i * 2 + 1];
  let apply_1 = [&](int l, int r, u32 x) -> u32 {
    l += n - 1;
    r += n + 1;
    int k = toy::log(l ^ r);
    int R = r >> k;
    for (r = r >> __builtin_ctz(r) ^ 1; r > R; r = r >> __builtin_ctz(r) ^ 1)
      x = node[r] + x;
    for (int t = ~l & ~(-1 << k), i; t > 0; t -= 1 << i) {
      i = toy::log(t);
      x = node[l >> i ^ 1] + x;
    }
    return x;
  };
  let apply_2 = [&](int l, int r, u32 x) -> u32 {
    l += n - 1;
    r += n + 1;
    int k = toy::log(l ^ r);
    int R = r >> k;
    for (l = l >> __builtin_ctz(~l) ^ 1; l > R; l = l >> __builtin_ctz(~l) ^ 1)
      x = x + node[l];
    for (int t = r & ~(-1 << k), i; t > 0; t -= 1 << i) {
      i = toy::log(t);
      x = x + node[r >> i ^ 1];
    }
    return x;
  };
  for (int i = 0; i < q; ++i) {
    switch (rd.u1()) {
    case 0: {
      int k = n + node2id[rd.uh()];
      u32 c = rd.uw();
      u32 d = rd.uw();
      node[k] = {c, d, d};
      for (k /= 2; k > 0; k /= 2) { node[k] = node[k * 2] + node[k * 2 + 1]; }
      break;
    }
    case 1: {
      int u = node2id[rd.uh()];
      int v = node2id[rd.uh()];
      u32 x = rd.uw();
      std::pair<int, int> vec[20];
      int c = 0;
      while (ances[u] != ances[v]) {
        if (depth[ances[u]] > depth[ances[v]]) {
          x = apply_1(ances[u], u, x);
          u = parent[ances[u]];
        } else {
          vec[++c] = {ances[v], v};
          v = parent[ances[v]];
        }
      }
      if (u > v) {
        x = apply_1(v, u, x);
      } else {
        x = apply_2(u, v, x);
      }
      for (; c > 0; --c) {
        def[l, r] = vec[c];
        x = apply_2(l, r, x);
      }
      wt.uw(x);
      break;
    }
    default:
      std::unreachable();
    }
  }
  return 0;
}
