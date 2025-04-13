#include <common.h>
prelude;

namespace {

constexpr int N = 200001;

struct Node {
  u64 val;
  u64 sum;
  int p;
  int ch[2];
  i8 rev;
  i8 k;
} node[N];

def reverse(int x) -> void { node[x].rev ^= 1; }

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
  node[x].sum = node[node[x].ch[0]].sum + node[x].val + node[node[x].ch[1]].sum;
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
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
#ifdef LOCAL
  std::memset(head, 0, 4 * n);
#endif
  for (int i = 1; i <= n; ++i)
    node[i] = {.k = -1}, node[i].sum = node[i].val = rd.uw();
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
      u32 x = rd.uw();
      splay(u);
      node[u].val += x;
      node[u].sum += x;
    }
    if (t == 2) {
      int u = rd.uh() + 1;
      int v = rd.uh() + 1;
      access(u);
      reverse(u);
      access(v);
      wt.ud(node[node[v].ch[0]].sum + node[v].val);
    }
  }
  return 0;
}
