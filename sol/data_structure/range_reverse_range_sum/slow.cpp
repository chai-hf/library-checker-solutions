#include "common.h"
// #include "random.h"
prelude;

namespace {

constexpr int N = 2e5 + 1;
struct Node {
  int pri;
  int val;
  u64 sum;
  int rev;
  int size;
  int l, r;
} node[N];

def maintain(int x) -> void {
  [[assume(x != 0)]];
  node[x].size = node[node[x].l].size + 1 + node[node[x].r].size;
  node[x].sum = node[node[x].l].sum + node[x].val + node[node[x].r].sum;
}

def pushdown(int x) -> void {
  [[assume(x != 0)]];
  if (node[x].rev) {
    std::swap(node[x].l, node[x].r);
    node[node[x].l].rev ^= 1;
    node[node[x].r].rev ^= 1;
    node[x].rev = 0;
  }
}

def reverse(int x) -> void {
  if (x == 0) return;
  node[x].rev ^= 1;
  pushdown(x);
}

def merge(int x, int y) -> int {
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

def split(int x, int k) -> std::pair<int, int> {
  if (x == 0) return {0, 0};
  pushdown(x);
  if (int size = node[node[x].l].size; k <= size) {
    let[a, b] = split(node[x].l, k);
    node[x].l = b;
    maintain(x);
    return {a, x};
  } else {
    let[a, b] = split(node[x].r, k - size - 1);
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
  std::vector<int> vec;
  int x = 0;
  for (int i = 1; i <= n; ++i) {
    node[i].size = 1;
    node[i].sum = node[i].val = rd.uw();
    node[i].pri = node[i].val; // global_gen.uniform(0, INT_MAX);
    x = merge(x, i);
  }
  if (n == 0) rd.skip(1);
  for (int i = 1; i <= q; ++i) {
    switch (rd.u1()) {
    case 0: {
      int l = rd.uh();
      int r = rd.uh();
      let[a, b] = split(x, l);
      let[c, d] = split(b, r - l);
      reverse(c);
      x = merge(a, merge(c, d));
      break;
    }
    case 1: {
      int l = rd.uh();
      int r = rd.uh();
      let[a, b] = split(x, l);
      let[c, d] = split(b, r - l);
      wt.ud(node[c].sum);
      x = merge(a, merge(c, d));
      break;
    }
    default:
      std::unreachable();
    }
  }
  return 0;
}
