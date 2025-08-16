#include <common.h>
prelude;

int main() {
  rd rd;
  wt wt;
  int n = rd.uh();
  while (n--) {
    let a = rd.ud();
    let b = rd.ud();
    let sum = a + b;
    wt.ud(sum);
  }
  return 0;
}
