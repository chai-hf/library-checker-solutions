#include "common.h"
prelude;

namespace {

constexpr int N = 1e6 + 3;
constexpr u32 P = 998244353;
struct Node {
  u32 val;
  u32 sum;
  u32 a = 1;
  u32 b;
  int rev;
  int size = 1;
  int l, r;
} node[N];

def maintain(int x) -> void {
  [[assume(x != 0)]];
  node[x].size = node[node[x].l].size + 1 + node[node[x].r].size;
  node[x].sum = (node[node[x].l].sum + node[x].val + node[node[x].r].sum) % P;
}

def build(int l, int r) -> int {
  if (l > r) return 0;
  int m = (l + r) / 2;
  node[m].l = build(l, m - 1);
  node[m].r = build(m + 1, r);
  maintain(m);
  return m;
}

def reverse(int x) -> void {
  if (x == 0) return;
  node[x].rev ^= 1;
}

def affine(int x, u64 a, u64 b) -> void {
  if (x == 0) return;
  node[x].a = (a * node[x].a) % P;
  node[x].b = (a * node[x].b + b) % P;
  node[x].val = (a * node[x].val + b) % P;
  node[x].sum = (a * node[x].sum + b * node[x].size) % P;
}

def pushdown(int x) -> void {
  [[assume(x != 0)]];
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

def zig(int &x) -> void {
  [[assume(x != 0)]];
  int l = node[x].l;
  [[assume(l != 0)]];
  node[x].l = node[l].r;
  maintain(x);
  node[l].r = x;
  x = l;
}

def zag(int &x) -> void {
  [[assume(x != 0)]];
  int r = node[x].r;
  [[assume(r != 0)]];
  node[x].r = node[r].l;
  maintain(x);
  node[r].l = x;
  x = r;
}

def splay(int &x, int k) -> void {
  [[assume(x != 0)]];
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
  node[0].size = 0;
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  std::vector<int> vec;
  for (int i = 2; i <= n + 1; ++i) node[i].val = rd.uw();
  int x = build(1, n += 2);
  for (int i = 1; i <= q; ++i) {
    switch (rd.u1()) {
    case 0: {
      ++n;
      int k = rd.uh();
      node[n].val = node[n].sum = rd.uw();
      splay(x, k);
      splay(node[x].r, 0);
      node[node[x].r].l = n;
      break;
    }
    case 1: {
      int k = rd.uh();
      splay(x, k);
      splay(node[x].r, 1);
      node[node[x].r].l = 0;
      break;
    }
    case 2: {
      int l = rd.uh();
      int r = rd.uh();
      splay(x, l);
      splay(node[x].r, r - l);
      reverse(node[node[x].r].l);
      break;
    }
    case 3: {
      int l = rd.uh();
      int r = rd.uh();
      splay(x, l);
      splay(node[x].r, r - l);
      int a = rd.uw();
      int b = rd.uw();
      affine(node[node[x].r].l, a, b);
      break;
    }
    case 4: {
      int l = rd.uh();
      int r = rd.uh();
      splay(x, l);
      splay(node[x].r, r - l);
      wt.ud(node[node[node[x].r].l].sum);
      break;
    }
    default:
      std::unreachable();
    }
  }
  return 0;
}
