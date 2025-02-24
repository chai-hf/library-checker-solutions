#include "io.h"

namespace {

constexpr int N = 5e5;

int a[N];
int p[N];
int l[N];
u64 b[N + 1];

}  // namespace

int main() {
    rd rd;
    wt wt;
    int n = rd.uh();
    int q = rd.uh();
    for (int i = 0; i < n; ++i) a[i] = rd.uw();
    for (int i = 1; i < n; ++i) p[i] = rd.uh();
    for (int i = n - 1; i > 0; --i) l[p[i]] += (l[i] += 1);
    l[0] = p[0] = n;
    for (int i = 1; i < n; ++i) {
        int j = p[i];
        [[assume(i != j)]];
        int li = l[i];
        int lj = l[j];
        l[i] = p[i] = lj;
        l[j] -= li;
    }
    for (int i = 0; i < n; ++i) b[l[i]] = a[i];
    u64 sum = 0;
    for (int i = 1; i <= n; ++i) sum = b[i] += sum;
    for (int i = n; i >= 1; --i) b[i] -= b[i - (i & -i)];
    for (int i = 1; i <= q; ++i) {
        switch (rd.u1()) {
            case 0: {
                int k = l[rd.uh()];
                int x = rd.uw();
                for (; k <= n; k += k & -k) b[k] += x;
                break;
            }
            case 1: {
                int x = rd.uh();
                int L = l[x] - 1;
                int R = p[x];
                u64 sum = 0;
                for (; L > 0; L -= L & -L) sum -= b[L];
                for (; R > 0; R -= R & -R) sum += b[R];
                wt.ud(sum);
                break;
            }
            default:
                std::unreachable();
        }
    }
}
