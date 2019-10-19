
#include <iostream>
#include "helper.hpp"

int main() {

  auto ids = clx::get_platform_ids();
  std::cout << "We have ...\n";
  for (auto &id : ids) {
    std::cout << " - " << id;
  }

  std::cout << "hello world\n";
}

