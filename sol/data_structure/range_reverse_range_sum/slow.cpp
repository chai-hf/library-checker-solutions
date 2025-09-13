#include <common.h>

namespace {

constexpr int N = 200001;

struct Node {
  int pri;
  int val;
  u64 sum;
  int rev;
  int size;
  int l, r;
} node[N];

auto maintain(int x) -> void {
  node[x].size = node[node[x].l].size + 1 + node[node[x].r].size;
  node[x].sum = node[node[x].l].sum + node[x].val + node[node[x].r].sum;
}

auto pushdown(int x) -> void {
  if (node[x].rev) {
    std::swap(node[x].l, node[x].r);
    node[node[x].l].rev ^= 1;
    node[node[x].r].rev ^= 1;
    node[x].rev = 0;
  }
}

auto reverse(int x) -> void {
  if (x == 0) return;
  node[x].rev ^= 1;
  pushdown(x);
}

auto merge(int x, int y) -> int {
  if (x == 0) return y;
  if (y == 0) return x;
  if (node[x].pri > node[y].pri) {
    pushdown(x);
    node[x].r = merge(node[x].r, y);
    maintain(x);
    return x;
  } else {
    pushdown(y);
    node[y].l = merge(x, node[y].l);
    maintain(y);
    return y;
  }
}

auto split(int x, int k) -> std::pair<int, int> {
  if (x == 0) return {0, 0};
  pushdown(x);
  if (int size = node[node[x].l].size; k <= size) {
    auto [a, b] = split(node[x].l, k);
    node[x].l = b;
    maintain(x);
    return {a, x};
  } else {
    auto [a, b] = split(node[x].r, k - size - 1);
    node[x].r = a;
    maintain(x);
    return {x, b};
  }
}

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  int x = 0;
  for (int i = 1; i <= n; ++i) {
    node[i].size = 1;
    node[i].pri = node[i].sum = node[i].val = rd.uw();
    x = merge(x, i);
  }
  if (n == 0) rd.skip(1);
  while (q--) {
    int t = rd.u1();
    int l = rd.uh();
    int r = rd.uh();
    auto [a, b] = split(x, l);
    auto [c, d] = split(b, r - l);
    if (t == 0) {
      reverse(c);
    }
    if (t == 1) {
      wt.ud(node[c].sum);
    }
    x = merge(a, merge(c, d));
  }
  return 0;
}
