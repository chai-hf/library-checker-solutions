#include <common.h>

namespace {

constexpr int N = 1000003;
constexpr u32 P = 998244353;

struct Node {
  u32 val;
  u32 sum;
  u32 a;
  u32 b;
  int rev;
  int size;
  int l, r;
} node[N];

auto maintain(int x) -> void {
  node[x].size = node[node[x].l].size + 1 + node[node[x].r].size;
  node[x].sum = (node[node[x].l].sum + node[x].val + node[node[x].r].sum) % P;
}

auto build(int l, int r) -> int {
  if (l > r) return 0;
  int m = (l + r) / 2;
  node[m].l = build(l, m - 1);
  node[m].r = build(m + 1, r);
  maintain(m);
  return m;
}

auto reverse(int x) -> void { node[x].rev ^= 1; }

auto affine(int x, u64 a, u64 b) -> void {
  if (x == 0) return;
  node[x].a = (a * node[x].a) % P;
  node[x].b = (a * node[x].b + b) % P;
  node[x].val = (a * node[x].val + b) % P;
  node[x].sum = (a * node[x].sum + b * node[x].size) % P;
}

auto pushdown(int x) -> void {
  if (node[x].rev) {
    std::swap(node[x].l, node[x].r);
    node[node[x].l].rev ^= 1;
    node[node[x].r].rev ^= 1;
    node[x].rev = 0;
  }
  if ((node[x].a != 1) | (node[x].b != 0)) {
    affine(node[x].l, node[x].a, node[x].b);
    affine(node[x].r, node[x].a, node[x].b);
    node[x].a = 1;
    node[x].b = 0;
  }
}

auto zig(int &x) -> void {
  int l = node[x].l;
  node[x].l = node[l].r;
  maintain(x);
  node[l].r = x;
  x = l;
}

auto zag(int &x) -> void {
  int r = node[x].r;
  node[x].r = node[r].l;
  maintain(x);
  node[r].l = x;
  x = r;
}

auto splay(int &x, int k) -> void {
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
  for (int i = 1; i <= n + q + 1; ++i) node[i] = Node{.a = 1, .size = 1};
  for (int i = 2; i <= n + 1; ++i) node[i].val = rd.uw();
  int x = build(1, n += 2);
  while (q--) {
    auto t = rd.u1();
    if (t == 0) {
      ++n;
      int k = rd.uh();
      node[n].val = node[n].sum = rd.uw();
      splay(x, k);
      splay(node[x].r, 0);
      node[node[x].r].l = n;
    }
    if (t == 1) {
      int k = rd.uh();
      splay(x, k);
      splay(node[x].r, 1);
      node[node[x].r].l = 0;
    }
    if (t == 2) {
      int l = rd.uh();
      int r = rd.uh();
      splay(x, l);
      splay(node[x].r, r - l);
      reverse(node[node[x].r].l);
    }
    if (t == 3) {
      int l = rd.uh();
      int r = rd.uh();
      splay(x, l);
      splay(node[x].r, r - l);
      int a = rd.uw();
      int b = rd.uw();
      affine(node[node[x].r].l, a, b);
    }
    if (t == 4) {
      int l = rd.uh();
      int r = rd.uh();
      splay(x, l);
      splay(node[x].r, r - l);
      wt.ud(node[node[node[x].r].l].sum);
    }
  }
  return 0;
}
