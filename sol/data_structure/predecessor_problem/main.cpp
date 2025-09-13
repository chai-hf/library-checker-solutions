#include <common.h>

namespace {

u64 root;
u64 lv18[39];
u64 lv12[2442];
u64 leaf[156250];

void set(u32 x) {
  leaf[x >> 6] |= 1ull << (x & 63);
  lv12[x >> 12] |= 1ull << ((x >> 6) & 63);
  lv18[x >> 18] |= 1ull << ((x >> 12) & 63);
  root |= 1ull << (x >> 18);
}

void unset(u32 x) {
  if (!(leaf[x >> 6] &= ~(1ull << (x & 63))))
    if (!(lv12[x >> 12] &= ~(1ull << ((x >> 6) & 63))))
      if (!(lv18[x >> 18] &= ~(1ull << ((x >> 12) & 63))))
        root &= ~(1ull << (x >> 18));
}

bool get(u32 x) { return leaf[x >> 6] & (1ull << (x & 63)); }

u32 nxt(u32 x) {
  if (u64 tmp = leaf[x >> 6] & (-1ull << (x & 63)); !tmp) {
    if (u64 tmp = lv12[x >> 12] & (-2ull << ((x >> 6) & 63)); !tmp) {
      if (u64 tmp = lv18[x >> 18] & (-2ull << ((x >> 12) & 63)); !tmp) {
        if (u64 tmp = root & (-2ull << (x >> 18)); !tmp) {
          return -1;
        } else {
          u32 ans = __builtin_ctzll(tmp);
          ans = ans << 6 | __builtin_ctzll(lv18[ans]);
          ans = ans << 6 | __builtin_ctzll(lv12[ans]);
          ans = ans << 6 | __builtin_ctzll(leaf[ans]);
          return ans;
        }
      } else {
        u32 ans = (x >> 18) << 6 | __builtin_ctzll(tmp);
        ans = ans << 6 | __builtin_ctzll(lv12[ans]);
        ans = ans << 6 | __builtin_ctzll(leaf[ans]);
        return ans;
      }
    } else {
      u32 ans = (x >> 12) << 6 | __builtin_ctzll(tmp);
      ans = ans << 6 | __builtin_ctzll(leaf[ans]);
      return ans;
    }
  } else {
    u32 ans = (x >> 6) << 6 | __builtin_ctzll(tmp);
    return ans;
  }
}

u32 pre(u32 x) {
  if (u64 tmp = leaf[x >> 6] & ~(-2ull << (x & 63)); !tmp) {
    if (u64 tmp = lv12[x >> 12] & ~(-1ull << ((x >> 6) & 63)); !tmp) {
      if (u64 tmp = lv18[x >> 18] & ~(-1ull << ((x >> 12) & 63)); !tmp) {
        if (u64 tmp = root & ~(-1ull << (x >> 18)); !tmp) {
          return -1;
        } else {
          u32 ans = 63 - __builtin_clzll(tmp);
          ans = ans << 6 | 63 - __builtin_clzll(lv18[ans]);
          ans = ans << 6 | 63 - __builtin_clzll(lv12[ans]);
          ans = ans << 6 | 63 - __builtin_clzll(leaf[ans]);
          return ans;
        }
      } else {
        u32 ans = (x >> 18) << 6 | 63 - __builtin_clzll(tmp);
        ans = ans << 6 | 63 - __builtin_clzll(lv12[ans]);
        ans = ans << 6 | 63 - __builtin_clzll(leaf[ans]);
        return ans;
      }
    } else {
      u32 ans = (x >> 12) << 6 | 63 - __builtin_clzll(tmp);
      ans = ans << 6 | 63 - __builtin_clzll(leaf[ans]);
      return ans;
    }
  } else {
    u32 ans = (x >> 6) << 6 | 63 - __builtin_clzll(tmp);
    return ans;
  }
}

} // namespace

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  int q = rd.uh();
  int i = 0;
  for (; i + 64 < n; i += 64) {
    u32 a = _mm256_movemask_epi8(_mm256_cmpeq_epi8(
        _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(rd.p + i)),
        _mm256_set1_epi8('1')));
    u32 b = _mm256_movemask_epi8(_mm256_cmpeq_epi8(
        _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(rd.p + i + 32)),
        _mm256_set1_epi8('1')));
    leaf[i >> 6] = a | u64(b) << 32;
  }
  for (; i < n; ++i) leaf[i >> 6] |= u64(rd.p[i] - '0') << (i & 63);
  for (int i = 0; i <= (n - 1) / 64; ++i)
    lv12[i >> 6] |= u64(!!leaf[i]) << (i & 63);
  for (int i = 0; i <= (n - 1) / 64 / 64; ++i)
    lv18[i >> 6] |= u64(!!lv12[i]) << (i & 63);
  for (int i = 0; i <= (n - 1) / 64 / 64 / 64; ++i)
    root |= u64(!!lv18[i]) << (i & 63);
  rd.p += n + 1;
  while (q--) {
    auto t = rd.u1();
    auto k = rd.uh();
    if (t == 0) {
      set(k);
    }
    if (t == 1) {
      unset(k);
    }
    if (t == 2) {
      auto x = get(k);
      wt.uw(x);
    }
    if (t == 3) {
      auto x = nxt(k);
      if (~x) {
        wt.uw(x);
      } else {
        wt.puts(" -1", 3);
      }
    }
    if (t == 4) {
      auto x = pre(k);
      if (~x) {
        wt.uw(x);
      } else {
        wt.puts(" -1", 3);
      }
    }
  }
  return 0;
}
