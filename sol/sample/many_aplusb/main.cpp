#include <common.h>

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  while (n--) {
    auto a = rd.ud();
    auto b = rd.ud();
    auto sum = a + b;
    wt.ud(sum);
  }
  return 0;
}
