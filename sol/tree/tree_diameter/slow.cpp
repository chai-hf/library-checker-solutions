#include <common.h>
prelude;

namespace {

constexpr int N = 5e5;

struct edge {
  int to;
  int len;
};

int pa[N];
std::vector<edge> ch[N];

def tree_diameter(int u, int p) -> std::pair<u64, int> {
  pa[u] = p;
  std::pair<u64, int> ans{0, u};
  for (let[v, len] : ch[u]) {
    if (v != p) {
      def res = tree_diameter(v, u);
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
    ch[a].emplace_back(b, c);
    ch[b].emplace_back(a, c);
  }
  let[_, u] = tree_diameter(0, -1);
  let[d, v] = tree_diameter(u, -1);
  int c = 0;
  for (int i = v; i != -1; i = pa[i]) ++c;
  wt.ud(d);
  wt.uw(c);
  for (int i = v; i != -1; i = pa[i]) wt.uw(i);
#ifdef LOCAL
  for (int i = 0; i < n; ++i) ch[i].clear();
#endif
  return 0;
}
