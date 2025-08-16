#include <common.h>
prelude;

namespace {

constexpr int N = 5e5;

struct {
  int to;
  int nxt;
} edge[N * 2];
int head[N];
int size[N];
int depth[N];
int heavy[N];
int ances[N];
int parent[N];
int node2id[N];
int id2node[N];
int id;

def build_step_1(int u, int d, int p) -> void {
  size[u] = 1;
  depth[u] = d;
  parent[u] = p;
  for (int e = head[u]; e; e = edge[e].nxt) {
    if (int v = edge[e].to; v != p) {
      build_step_1(v, d + 1, u);
      size[u] += size[v];
      if (heavy[u] == 0 || size[v] > size[heavy[u]]) {
        heavy[u] = v;
      }
    }
  }
}

def build_step_2(int u, int a, int p) -> void {
  node2id[u] = id;
  id2node[id] = u;
  ++id;
  ances[u] = a;
  if (heavy[u]) build_step_2(heavy[u], a, u);
  for (int e = head[u]; e; e = edge[e].nxt) {
    if (int v = edge[e].to; v != heavy[u] && v != p) {
      build_step_2(v, v, u);
    }
  }
}

def lca(int u, int v) -> int {
  if (depth[u] < 32 && depth[v] < 32) {
    while (depth[u] > depth[v]) {
      u = parent[u];
    }
    while (depth[v] > depth[u]) {
      v = parent[v];
    }
    while (u != v) {
      u = parent[u];
      v = parent[v];
    }
    return u;
  }
  while (ances[u] != ances[v]) {
    if (depth[ances[u]] > depth[ances[v]]) {
      u = parent[ances[u]];
    } else {
      v = parent[ances[v]];
    }
  }
  return depth[u] < depth[v] ? u : v;
}

def jump(int u, int d) -> int {
  if (int t = depth[u] - d; t < 32) {
    while (t--) u = parent[u];
    return u;
  }
  while (depth[ances[u]] > d) {
    u = parent[ances[u]];
  }
  return id2node[node2id[u] - depth[u] + d];
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
  std::memset(parent, 0, 4 * n);
#endif
  for (int i = 1; i < n; ++i) {
    int a = rd.uh();
    int b = rd.uh();
    edge[i * 2 | 0] = {b, head[a]}, head[a] = i * 2 | 0;
    edge[i * 2 | 1] = {a, head[b]}, head[b] = i * 2 | 1;
  }
  build_step_1(0, 0, -1);
  build_step_2(0, 0, -1);
  while (q--) {
    int u = rd.uh();
    int v = rd.uh();
    int k = rd.uh();
    int w = lca(u, v);
    int du = depth[u];
    int dv = depth[v];
    int dw = depth[w];
    if (k <= du - dw) {
      int x = jump(u, du - k);
      wt.uw(x);
    } else if (k <= du + dv - dw - dw) {
      int x = jump(v, dw + dw - du + k);
      wt.uw(x);
    } else {
      wt.puts(" -1", 3);
    }
  }
  return 0;
}
