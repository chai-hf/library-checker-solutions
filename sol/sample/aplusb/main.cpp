#include <common.h>
prelude;

int main() {
  std::vector<int> vec(2);
  for (def &ele : vec) std::cin >> ele;
  let sum = std::accumulate(vec.begin(), vec.end(), 0);
  std::println("{}", sum);
  return 0;
}
