#include <common.h>

int main() {
  std::vector<int> vec;
  std::ranges::copy(std::views::istream<int>(std::cin) | std::views::take(2),
                    std::back_inserter(vec));
  auto sum = std::ranges::fold_left(vec, 0, std::plus());
  std::println("{}", sum);
  return 0;
}
