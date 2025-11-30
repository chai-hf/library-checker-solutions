#include <common.h>

namespace {

constexpr int N = 5e5;

int next[N];
int head[N];
int size[N];
int depth[N];
int heavy[N];
int ances[N];
int parent[N];

auto build_step_1(int u, int d) -> void {
  size[u] = 1;
  depth[u] = d;
  for (int v = head[u]; v; v = next[v]) {
    build_step_1(v, d + 1);
    size[u] += size[v];
    if (heavy[u] == 0 || size[v] > size[heavy[u]]) {
      heavy[u] = v;
    }
  }
}

auto build_step_2(int u, int a) -> void {
  ances[u] = a;
  for (int v = head[u]; v; v = next[v]) {
    if (v != heavy[u]) {
      build_step_2(v, v);
    }
  }
  if (heavy[u]) build_step_2(heavy[u], a);
}

auto lca(int u, int v) -> int {
  while (ances[u] != ances[v]) {
    if (depth[ances[u]] > depth[ances[v]]) {
      u = parent[ances[u]];
    } else {
      v = parent[ances[v]];
    }
  }
  return depth[u] < depth[v] ? u : v;
}

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  for (int i = 1; i < n; ++i) {
    int p = rd.uh();
    parent[i] = p;
    next[i] = head[p];
    head[p] = i;
  }
  build_step_1(0, 0);
  build_step_2(0, 0);
  while (q--) {
    int u = rd.uh();
    int v = rd.uh();
    int w = lca(u, v);
    wt.uw(w);
  }
  return 0;
}
