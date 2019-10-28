#include "clx.hpp"
#include <fmt/format.h>

auto show_device_info(cl_device_id const &id) {
  fmt::print("name:{}, vendor:{}\n", clx::get_device_info_name(id),
             clx::get_device_info_vendor(id));
}
auto show_platform_info(cl_platform_id const &id) {
  fmt::print("name:{}, vendor:{}\n", clx::get_platform_info_name(id),
             clx::get_platform_info_vendor(id));
}

int main() {
  auto ids = clx::get_platform_ids();
  for (auto &id : ids) {
    show_platform_info(id);
  }

  auto platform = ids[0];

  // devices.
  auto devices = clx::get_device_ids(platform, CL_DEVICE_TYPE_GPU);

  cl_context_properties properties[] = {CL_CONTEXT_PLATFORM,
                                        (cl_context_properties)platform, 0};

  auto context = clCreateContext(properties, devices.size(), devices.data(),
                                 nullptr, nullptr, nullptr);
  return 0;
}
