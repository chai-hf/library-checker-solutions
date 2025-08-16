#include <common.h>
#include <toy/bit.h>
prelude;

namespace {

template <typename K, typename V, u32 N> struct HashMap {
  static_assert(has_single_bit(N));
  K key[N];
  V val[N];
  std::bitset<N> use;
  static constexpr u32 shift = 64 - log(N);
  static constexpr u64 r = 11995408973635179863ULL;
  def set(K k, V v) -> void {
    u32 hash = k * r >> shift;
    for (;;) {
      if (use[hash] == 0) {
        key[hash] = k;
        use[hash] = 1;
        val[hash] = v;
        return;
      }
      if (key[hash] == k) {
        val[hash] = v;
        return;
      }
      (++hash) &= (N - 1);
    }
  }
  def get(K k) -> V {
    u32 hash = k * r >> shift;
    for (;;) {
      if (use[hash] == 0) {
        return 0;
      }
      if (key[hash] == k) {
        return val[hash];
      }
      (++hash) &= (N - 1);
    }
  }
};

HashMap<u64, u64, 1 << 20> hash_map;

} // namespace

int main() {
  rd rd;
  wt wt;
  int q = rd.uh();
#ifdef LOCAL
  hash_map.use.reset();
#endif
  while (q--) {
    let t = rd.u1();
    if (t == 0) {
      let k = rd.ud();
      let v = rd.ud();
      hash_map.set(k, v);
    }
    if (t == 1) {
      let k = rd.ud();
      let v = hash_map.get(k);
      wt.ud(v);
    }
  }
  return 0;
}
