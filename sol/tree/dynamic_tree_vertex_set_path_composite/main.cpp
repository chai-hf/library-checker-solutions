#include <common.h>
prelude;

namespace {

constexpr int N = 200001;
constexpr u32 P = 998244353;

struct Affine {
  u32 a, b, c;
  auto operator+(const Affine &t) const -> Affine {
    u32 x = (u64(a) * t.a) % P;
    u32 y = (u64(a) * t.b + b) % P;
    u32 z = (u64(t.a) * c + t.c) % P;
    return {x, y, z};
  }
  auto operator+(const u32 &t) const -> u32 { return (u64(a) * t + b) % P; }
};
struct Node {
  u32 a;
  u32 b;
  Affine aff;
  int p;
  int ch[2];
  i8 rev;
  i8 k;
} node[N];

def reverse(int x) -> void {
  node[x].rev ^= 1;
  std::swap(node[x].aff.b, node[x].aff.c);
}

def pushdown(int x) -> void {
  if (node[x].rev) {
    std::swap(node[x].ch[0], node[x].ch[1]);
    node[node[x].ch[0]].k = 0;
    node[node[x].ch[1]].k = 1;
    reverse(node[x].ch[0]);
    reverse(node[x].ch[1]);
    node[x].rev = 0;
  }
}

def maintain(int x) -> void {
  node[x].aff = node[node[x].ch[0]].aff +
                Affine{node[x].a, node[x].b, node[x].b} +
                node[node[x].ch[1]].aff;
}

def rotateup(int x) -> void {
  int p = node[x].p;
  int g = node[p].p;
  int k = node[x].k;
  int t = node[p].k;
  node[node[x].ch[k ^ 1]].p = p;
  node[node[x].ch[k ^ 1]].k = k;
  node[p].ch[k] = node[x].ch[k ^ 1];
  node[p].p = x;
  node[p].k = k ^ 1;
  node[x].ch[k ^ 1] = p;
  node[x].p = g;
  node[x].k = t;
  if (t != -1) {
    node[g].ch[t] = x;
  }
  maintain(p);
}

def is_root(int x) -> bool { return node[x].k == -1; }

def splay(int x) -> void {
  pushdown(x);
  while (!is_root(x)) {
    if (int p = node[x].p; is_root(p)) {
      pushdown(p);
      pushdown(x);
      rotateup(x);
    } else {
      int g = node[p].p;
      pushdown(g);
      pushdown(p);
      pushdown(x);
      if (node[x].k == node[p].k) {
        rotateup(p);
        rotateup(x);
      } else {
        rotateup(x);
        rotateup(x);
      }
    }
  }
}

def access(int x) -> void {
  splay(x);
  node[node[x].ch[1]].k = -1;
  node[x].ch[1] = 0;
  while (int p = node[x].p) {
    splay(p);
    node[node[p].ch[1]].k = -1;
    node[p].ch[1] = x;
    node[x].k = 1;
    rotateup(x);
  }
}

int head[N];
struct {
  int to;
  int next;
} edge[N * 2];

def build(int u, int p) -> void {
  for (int e = head[u]; e; e = edge[e].next) {
    int v = edge[e].to;
    if (v != p) {
      build(v, u);
      node[v + 1].p = u + 1;
    }
  }
}

} // namespace

int main() {
  node[0].aff.a = 1;
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
#ifdef LOCAL
  std::memset(head, 0, 4 * n);
#endif
  for (int i = 1; i <= n; ++i) {
    node[i] = {.k = -1};
    u32 a = node[i].a = rd.uw();
    u32 b = node[i].b = rd.uw();
    node[i].aff = {a, b, b};
  }
  for (int i = 1; i != n; ++i) {
    int u = rd.uh();
    int v = rd.uh();
    edge[i * 2 | 0] = {v, head[u]}, head[u] = i * 2 | 0;
    edge[i * 2 | 1] = {u, head[v]}, head[v] = i * 2 | 1;
  }
  build(0, 0);
  while (q--) {
    let t = rd.u1();
    if (t == 0) {
      int u = rd.uh() + 1;
      int v = rd.uh() + 1;
      access(u);
      reverse(u);
      access(v);
      node[node[v].ch[0]].p = 0;
      node[node[v].ch[0]].k = -1;
      node[v].ch[0] = 0;
      int w = rd.uh() + 1;
      int x = rd.uh() + 1;
      access(w);
      reverse(w);
      node[w].p = x;
    }
    if (t == 1) {
      int u = rd.uh() + 1;
      node[u].a = rd.uw();
      node[u].b = rd.uw();
      splay(u);
    }
    if (t == 2) {
      int u = rd.uh() + 1;
      int v = rd.uh() + 1;
      u32 x = rd.uw();
      access(v);
      reverse(v);
      access(u);
      x = Affine{node[u].a, node[u].b, node[u].b} + x;
      x = node[node[u].ch[0]].aff + x;
      wt.ud(x);
    }
  }
  return 0;
}
