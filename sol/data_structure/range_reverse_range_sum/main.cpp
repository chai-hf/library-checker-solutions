#include <common.h>
prelude;

namespace {

constexpr int N = 200003;

struct Node {
  int val;
  u64 sum;
  int rev;
  int size;
  int l, r;
} node[N];

def maintain(int x) -> void {
  node[x].size = node[node[x].l].size + 1 + node[node[x].r].size;
  node[x].sum = node[node[x].l].sum + node[x].val + node[node[x].r].sum;
}

def build(int l, int r) -> int {
  if (l > r) return 0;
  int m = (l + r) / 2;
  node[m].l = build(l, m - 1);
  node[m].r = build(m + 1, r);
  maintain(m);
  return m;
}

def pushdown(int x) -> void {
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
}

def zig(int &x) -> void {
  int l = node[x].l;
  node[x].l = node[l].r;
  maintain(x);
  node[l].r = x;
  x = l;
}

def zag(int &x) -> void {
  int r = node[x].r;
  node[x].r = node[r].l;
  maintain(x);
  node[r].l = x;
  x = r;
}

def splay(int &x, int k) -> void {
  pushdown(x);
  if (int &l = node[x].l, &r = node[x].r, size = node[l].size; k == size) {
    return;
  } else if (k < size) {
    pushdown(l);
    if (int &ll = node[l].l, &lr = node[l].r, size = node[ll].size; k == size) {
      zig(x);
    } else if (k < size) {
      splay(ll, k);
      zig(x);
      zig(x);
    } else {
      splay(lr, k - size - 1);
      zag(l);
      zig(x);
    }
  } else {
    pushdown(r);
    k -= size + 1;
    if (int &rl = node[r].l, &rr = node[r].r, size = node[rl].size; k == size) {
      zag(x);
    } else if (k < size) {
      splay(rl, k);
      zig(r);
      zag(x);
    } else {
      splay(rr, k - size - 1);
      zag(x);
      zag(x);
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
  std::memset(node, 0, sizeof(Node) * (n + 3));
#endif
  for (int i = 2; i <= n + 1; ++i) node[i].val = rd.uw();
  int x = build(1, n + 2);
  if (n == 0) rd.skip(1);
  while (q--) {
    int t = rd.u1();
    int l = rd.uh();
    int r = rd.uh();
    splay(x, l);
    splay(node[x].r, r - l);
    if (t == 0) {
      reverse(node[node[x].r].l);
    }
    if (t == 1) {
      wt.ud(node[node[node[x].r].l].sum);
    }
  }
  return 0;
}
