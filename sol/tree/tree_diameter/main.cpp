#include <common.h>

namespace {

constexpr int N = 5e5;

struct {
  int to;
  int len;
  int nxt;
} edge[N * 2];
int pa[N];
int head[N];

auto tree_diameter(int u, int p) -> std::pair<u64, int> {
  pa[u] = p;
  std::pair<u64, int> ans{0, u};
  for (int e = head[u]; e; e = edge[e].nxt) {
    int v = edge[e].to;
    int len = edge[e].len;
    if (v != p) {
      auto res = tree_diameter(v, u);
      res.first += len;
      ans = std::max(ans, res);
    }
  }
  return ans;
}

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  for (int i = 1; i < n; ++i) {
    int a = rd.uh();
    int b = rd.uh();
    int c = rd.uw();
    edge[i * 2 | 0] = {b, c, head[a]}, head[a] = i * 2 | 0;
    edge[i * 2 | 1] = {a, c, head[b]}, head[b] = i * 2 | 1;
  }
  auto [_, u] = tree_diameter(0, -1);
  auto [d, v] = tree_diameter(u, -1);
  int c = 0;
  for (int i = v; i != -1; i = pa[i]) ++c;
  wt.ud(d);
  wt.uw(c);
  for (int i = v; i != -1; i = pa[i]) wt.uw(i);
  return 0;
}
