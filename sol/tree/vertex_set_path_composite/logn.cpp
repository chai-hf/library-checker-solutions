#include <common.h>
#include <toy/bit.h>
prelude;

namespace {

constexpr int N = 200001;
constexpr int P = 998244353;

struct Vertex {
  u32 a, b;
};
struct Segment {
  u32 a, b, c;
  Segment() = default;
  Segment(Vertex v) : a{v.a}, b{v.b}, c{v.b} {}
  Segment(u32 a, u32 b, u32 c) : a{a}, b{b}, c{c} {}
};
struct Node {
  Vertex v;
  Segment s, t;
  int l, r, p;
} node[N];
auto operator+(const Segment &l, const Segment &r) -> Segment {
  u32 x = u64(l.a) * r.a % P;
  u32 y = (u64(l.a) * r.b + l.b) % P;
  u32 z = (u64(r.a) * l.c + r.c) % P;
  return {x, y, z};
}
auto operator+(const Segment &l, u32 r) -> u32 {
  return (u64(l.a) * r + l.b) % P;
}
auto operator+(u32 l, const Segment &r) -> u32 {
  return (u64(r.a) * l + r.c) % P;
}
u32 a[N];
u32 b[N];
int head[N];
int size[N];
int depth[N];
int heavy[N];
int ances[N];
int parent[N];
int node2id[N];
u8 priority[N];
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
      if (heavy[u] == 0 || size[v] > size[heavy[u]]) {
        heavy[u] = v;
      }
    }
  }
}

def build_step_2(int u, int w, int p, int d) -> void {
  int i = ++id;
  node2id[u] = i;
  depth[i] = d;
  ances[i] = node2id[w];
  parent[i] = node2id[p];
  node[i].v = {a[u], b[u]};
  for (int e = head[u]; e; e = edge[e].next) {
    int v = edge[e].to;
    if (v != p && v != heavy[u]) {
      build_step_2(v, v, u, d + 1);
    }
  }
  priority[i] = u8(log(i ^ (id + 1)));
  if (u != w) {
    int p = parent[i];
    int l = 0;
    while (p && priority[p] < priority[i]) {
      l = p;
      p = node[p].p;
    }
    node[p].r = i;
    node[i].p = p;
    node[i].l = l;
    node[l].p = i;
  }
  if (int v = heavy[u]; v) {
    build_step_2(v, w, u, d + 1);
  }
}

def maintain(int u) -> void {
  node[u].s = (node[u].t = node[node[u].l].s + node[u].v) + node[node[u].r].s;
}

template <typename T> T apply(int k, T x) {
  for (int u = k; u; u = node[u].p) {
    if (u <= k) {
      x = node[u].t + x;
    }
  }
  return x;
}

template <typename T> T apply(int l, int r, T x) {
  Segment t = {1, 0, 0};
  int u = l;
  int v = r;
  while (u != v) {
    if (priority[u] < priority[v]) {
      if (l <= u) {
        t = (t + node[u].v) + node[node[u].r].s;
      }
      u = node[u].p;
    } else {
      if (v <= r) {
        x = node[v].t + x;
      }
      v = node[v].p;
    }
  }
  x = t + (node[u].v + x);
  return x;
}

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
#ifdef LOCAL
  id = 0;
  std::memset(head, 0, 4 * n);
  std::memset(heavy, 0, 4 * n);
  std::memset(node, 0, (n + 1) * sizeof(Node));
#endif
  node[0].s.a = 1;
  for (int i = 0; i < n; ++i) a[i] = rd.uw(), b[i] = rd.uw();
  for (int i = 1; i < n; ++i) {
    int u = rd.uh();
    int v = rd.uh();
    edge[i * 2 | 0] = {v, head[u]}, head[u] = i * 2 | 0;
    edge[i * 2 | 1] = {u, head[v]}, head[v] = i * 2 | 1;
  }
  build_step_1(0, 0);
  build_step_2(0, 0, 0, 0);
  for (int i = 1; i <= n; ++i) {
    if (node[i].r == 0) {
      for (int u = i; u && u <= i; u = node[u].p) {
        maintain(u);
      }
    }
  }
  while (q--) {
    let t = rd.u1();
    if (t == 0) {
      int u = node2id[rd.uh()];
      node[u].v = {rd.uw(), rd.uw()};
      for (; u; u = node[u].p) {
        maintain(u);
      }
    }
    if (t == 1) {
      int u = node2id[rd.uh()];
      int v = node2id[rd.uh()];
      u32 x = rd.uw();
      Segment f = {1, 0, 0};
      while (ances[u] != ances[v]) {
        if (depth[ances[u]] >= depth[ances[v]]) {
          x = apply(u, x);
          u = parent[ances[u]];
        } else {
          f = apply(v, f);
          v = parent[ances[v]];
        }
      }
      if (u >= v) {
        x = apply(v, u, x);
      } else {
        f = apply(u, v, f);
      }
      x = x + f;
      wt.uw(x);
    }
  }
  return 0;
}
