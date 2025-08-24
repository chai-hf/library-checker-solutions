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
std::pair<u64, int> queue[N];

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
  std::pair<u64, int> ans;
  auto bfs = [&](int u) {
    pa[u] = -1;
    queue[0] = {0, u};
    ans = {};
    for (int i = 0, j = 0; i < n; ++i) {
      auto [d, u] = queue[i];
      for (int e = head[u]; e; e = edge[e].nxt) {
        int v = edge[e].to;
        int len = edge[e].len;
        if (v != pa[u]) {
          pa[v] = u;
          ans = std::max(ans, queue[++j] = {d + len, v});
        }
      }
    }
  };
  bfs(0);
  int u = ans.second;
  bfs(u);
  int v = ans.second;
  u64 d = ans.first;
  wt.ud(d);
  int c = 0;
  for (int i = v; i != -1; i = pa[i]) ++c;
  wt.uw(c);
  for (int i = v; i != -1; i = pa[i]) wt.uw(i);
  return 0;
}
