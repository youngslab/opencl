#include "cl/clx.hpp"
#include <fmt/format.h>

int main() {
  auto platforms = clx::get_platform_ids();
  for (auto &platform : platforms) {
    fmt::print("[INFO] platform: {}\n", clx::to_string(platform));
    auto devices = clx::get_device_ids(platform, CL_DEVICE_TYPE_GPU);
    for (auto &device : devices) {
      fmt::print("[INFO] device: {}\n", clx::to_string(device));
    }
  }

  return 0;
}
