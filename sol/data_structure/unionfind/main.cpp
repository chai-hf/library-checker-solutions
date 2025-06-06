#include <common.h>
prelude;

namespace {

int pa[200000];

def find(int u) -> int {
  int w = pa[u], v;
  if (w < 0) return u;
  while (pa[w] >= 0) w = pa[w];
  while (pa[u] != w) v = u, u = pa[u], pa[v] = w;
  return w;
};

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  std::memset(pa, -1, 4 * n);
  while (q--) {
    let t = rd.u1();
    let u = find(rd.uh());
    let v = find(rd.uh());
    if (t == 0 && u != v) {
      if (pa[u] < pa[v])
        pa[v] = u;
      else if (pa[v] < pa[u])
        pa[u] = v;
      else
        pa[u] = v, --pa[v];
    }
    if (t == 1) {
      wt.u1(u == v);
    }
  }
  return 0;
}
