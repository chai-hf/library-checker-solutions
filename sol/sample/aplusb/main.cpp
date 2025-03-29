#include "atcoder/all"
#include "common.h"
prelude;

int main() {
  std::vector<int> vec(2);
  for (auto &ele : vec) std::cin >> ele;
  auto sum = std::accumulate(vec.begin(), vec.end(), 0);
  std::println("{}", sum);
  return 0;
}
