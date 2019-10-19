

#include <CL/opencl.h>
#include <optional>
#include <vector>
#include <fstream>

// device, platform and context

namespace clx {

auto get_platform_ids() -> std::vector<cl_platform_id> {

  cl_int errNum;
  cl_uint numPlatforms;

  errNum = clGetPlatformIDs(0, nullptr, &numPlatforms);
  if (errNum != CL_SUCCESS || numPlatforms <= 0) {
    return {};
  }

  std::vector<cl_platform_id> ids(numPlatforms);
  errNum = clGetPlatformIDs(numPlatforms, ids.data(), nullptr);
  if (errNum != CL_SUCCESS) {
    ids.clear();
  }

  return ids;
}

auto get_device_ids(const cl_platform_id &platform)
    -> std::vector<cl_device_id> {
  std::vector<cl_device_id> device_ids;
  cl_uint num_devices;
  auto errNum =
      clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &num_devices);
  if (errNum != CL_SUCCESS || num_devices <= 0) {
    return device_ids;
  }

  device_ids.reserve(num_devices);
  errNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_devices,
                          device_ids.data(), nullptr);
  if (errNum != CL_SUCCESS || num_devices <= 0) {
    device_ids.clear();
  }

  return device_ids;
}

auto release(const std::vector<cl_command_queue> &qs) {
  for (auto &q : qs) {
    // TODO: release resourcs.
  }
}

auto release(const std::vector<cl_mem> &ms) {
  for (auto &m : ms) {
    // TODO: release resourcs.
  }
}
template <typename T>
auto create_buffers(cl_context ctx, int num_elements, int num_devices)
    -> std::vector<cl_mem> {

  cl_uint err_num;
  std::vector<cl_mem> bs;
  auto b =
      clCreateBuffer(ctx, CL_MEM_READ_WRITE,
                     sizeof(T) * num_elements * num_devices, nullptr, &err_num);
  if (err_num != CL_SUCCESS) {
    return bs;
  }
  bs.push_back(b);

  // create sub-buffers
  for (auto i = 1u; i < num_devices; i++) {
    cl_buffer_region region = {num_elements * i * sizeof(int),
                               num_elements * sizeof(int)};
    auto sub_buffer =
        clCreateSubBuffer(bs[0], CL_MEM_READ_WRITE,
                          CL_BUFFER_CREATE_TYPE_REGION, &region, &err_num);
    if (err_num != CL_SUCCESS) {
      release(bs);
      bs.clear();
      break;
    }
    bs.push_back(sub_buffer);
  }

  return bs;
}

auto create_command_queue(cl_context ctx, const std::vector<cl_device_id> ds)
    -> std::vector<cl_command_queue> {
  cl_int err_num;
  std::vector<cl_command_queue> qs;
  for (int i = 0; i < ds.size(); i++) {
    auto q = clCreateCommandQueue(ctx, ds[i], 0, &err_num);
    if (err_num != CL_SUCCESS) {
      release(qs);
      qs.clear();
      break;
    }
    qs.push_back(q);
  }
  return qs;
}

auto create_program(cl_context context, const char *filepath) -> cl_program {

  std::ifstream srcFile(filepath);
  if (!srcFile.is_open()) {
    return nullptr;
  }

  std::string srcProg(std::istreambuf_iterator<char>(srcFile),
                      (std::istreambuf_iterator<char>()));

  const char *src = srcProg.c_str();
  size_t length = srcProg.length();

  cl_int err_num;
  auto program = clCreateProgramWithSource(context, 1, &src, &length, &err_num);
  if (err_num != CL_SUCCESS) {
    return nullptr;
  }
  return program;
}

auto to_string(cl_platform_info name) {
  std::string result;
  switch (name) {
  case CL_PLATFORM_PROFILE:
    result = "CL_PLATFORM_PROFILE";
    break;
  case CL_PLATFORM_VERSION:
    result = "CL_PLATFORM_VERSION";
    break;
  case CL_PLATFORM_NAME:
    result = "CL_PLATFORM_NAME";
    break;
  case CL_PLATFORM_VENDOR:
    result = "CL_PLATFORM_VENDOR";
    break;
  case CL_PLATFORM_EXTENSIONS:
    result = "CL_PLATFORM_EXTENSIONS";
    break;
  defulat:
    result = "unkown";
    break;
  }
  return result;
}

auto get_platform_info_str(cl_platform_id id, cl_platform_info name)
    -> std::optional<std::string> {

  std::string info;

  cl_int errNum;
  std::size_t param_size;
  errNum = clGetPlatformInfo(id, name, 0, nullptr, &param_size);
  if (errNum != CL_SUCCESS) {
    return std::nullopt;
  }

  info.reserve(param_size);
  errNum = clGetPlatformInfo(id, name, param_size, info.data(), nullptr);
  if (errNum != CL_SUCCESS) {
    return std::nullopt;
  }

  return info;
}
} // namespace clx

