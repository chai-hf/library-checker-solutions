#include "common.h"
prelude;

int main() {
  rd rd;
  wt wt;
  int n = rd.uw();
  while (n--) wt.ud(rd.ud() + rd.ud());
  return 0;
}
