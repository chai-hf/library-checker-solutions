#include <common.h>
#include <toy/bit.h>
prelude;

namespace {

constexpr int N = 1e6;
constexpr int P = 998244353;

struct affine {
  u32 a, b;
  auto operator+(affine t) -> affine {
    return {u32(u64(t.a) * a % P), u32((u64(t.a) * b + t.b) % P)};
  }
  auto operator+=(affine t) -> void { *this = *this + t; }
};

struct node {
  u32 siz;
  u32 sum;
  affine aff;
  auto operator+=(affine t) -> void {
    aff += t;
    sum = (u64(t.a) * sum + u64(t.b) * siz) % P;
  }
} a[N];

void pushdown(int k) {
  for (int i = log(k) - 1; i >= 0; --i) {
    affine t = a[k >> i >> 1].aff;
    a[k >> i >> 1].aff = affine{1, 0};
    a[k >> i ^ 0] += t;
    a[k >> i ^ 1] += t;
  }
}

u32 mod(u32 x) { return x < P ? x : x - P; }

void pushup(int k) {
  for (k /= 2; k > 0; k /= 2) {
    a[k].sum = mod(a[k * 2].sum + a[k * 2 + 1].sum);
  }
}

}; // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  for (int i = 0; i < n; ++i) a[n + i].siz = 1, a[n + i].sum = rd.uw();
  for (int i = n - 1; i >= 0; --i) {
    a[i].aff = {1, 0};
    a[i].siz = a[2 * i].siz + a[2 * i + 1].siz;
    a[i].sum = mod(a[2 * i].sum + a[2 * i + 1].sum);
  }
  while (q--) {
    let t = rd.u1();
    if (t == 0) {
      int l = n + rd.uh();
      int r = n + rd.uh() - 1;
      pushdown(l--);
      pushdown(r++);
      int k = log(l ^ r);
      affine x = {rd.uw(), rd.uw()};
      int t, i;
      for (t = ~l & ~(-1 << k), i = 31; t > 0; t -= 1 << i) {
        i = log(t);
        a[l >> i ^ 1] += x;
      }
      pushup(l >> i);
      for (t = +r & ~(-1 << k), i = 31; t > 0; t -= 1 << i) {
        i = log(t);
        a[r >> i ^ 1] += x;
      }
      pushup(r >> i);
    }
    if (t == 1) {
      int l = n + rd.uh();
      int r = n + rd.uh() - 1;
      u64 sizL = 0, sumL = 0;
      u64 sizR = 0, sumR = 0;
      for (--l, ++r; l ^ r ^ 1;) {
        if (~l & 1) sizL += a[l ^ 1].siz, sumL += a[l ^ 1].sum;
        if (+r & 1) sizR += a[r ^ 1].siz, sumR += a[r ^ 1].sum;
        l /= 2, r /= 2;
        sumL = (a[l].aff.a * sumL + a[l].aff.b * sizL) % P;
        sumR = (a[r].aff.a * sumR + a[r].aff.b * sizR) % P;
      }
      sumL = mod(u32(sumL + sumR));
      sizL += sizR;
      for (l /= 2; l > 0; l /= 2) {
        sumL = (a[l].aff.a * sumL + a[l].aff.b * sizL) % P;
      }
      wt.uw(u32(sumL));
    }
  }
  return 0;
}
