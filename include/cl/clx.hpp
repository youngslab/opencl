#include <vector>
#include <string>

#define __CL_ENABLE_EXCEPTIONS
#include "cl/cl.hpp"

namespace clx {

static cl_int g_errcode;
static char *g_errfunc;

auto set_error(cl_int errcode, const char *errfunc) {
  g_errcode = errcode;
  g_errfunc = const_cast<char *>(errfunc);
}

cl_int get_error_code() { return g_errcode; }

char *get_error_function() { return g_errfunc; }

std::vector<cl::Platform> get_platforms() {
  std::vector<cl::Platform> platforms;
  auto res = cl::Platform::get(&platforms);
  return platforms;
}

std::string get_program_build_logs(const cl::Program &p, const cl::Device &d) {
  return p.getBuildInfo<CL_PROGRAM_BUILD_LOG>(d);
}

auto create_context_properties(const cl_platform_id &id)
    -> std::vector<cl_context_properties> {
  return {CL_CONTEXT_PLATFORM, (cl_context_properties)id, 0};
}

cl_context create_context(cl_context_properties *ps, const cl_device_type &t) {
  return clCreateContextFromType(ps, t, nullptr, nullptr, &g_errcode);
}

cl_context create_context(cl_context_properties *ps,
                          const std::vector<cl_device_id> &ds) {
  return clCreateContext(ps, ds.size(), ds.data(), nullptr, nullptr,
                         &g_errcode);
}

auto get_devices(const cl_context &ctx) -> std::vector<cl_device_id> {
  size_t cnt = 0;
  std::vector<cl_device_id> ds;

  auto err = clGetContextInfo(ctx, CL_CONTEXT_DEVICES, 0, nullptr, &cnt);
  if (err != CL_SUCCESS) {
    set_error(err, "clGetContextInfo");
    return ds;
  }

  ds.reserve(cnt);
  err = clGetContextInfo(ctx, CL_CONTEXT_DEVICES, cnt, ds.data(), nullptr);
  if (err != CL_SUCCESS) {
    set_error(err, "clGetContextInfo");
    ds.clear();
    return ds;
  }

  return ds;
}

auto create_command_queue(const cl_context &ctx, const cl_device_id &d,
                          const cl_command_queue_properties &p)
    -> cl_command_queue {
  cl_int err = 0;
  auto q = clCreateCommandQueue(ctx, d, p, &err);
  if (err != CL_SUCCESS) {
    set_error(err, "clCreateCommandQueue");
    return nullptr;
  }
  return q;
}

auto create_command_queue(const cl_context &ctx, const cl_device_id &d)
    -> cl_command_queue {
  return create_command_queue(ctx, d, 0);
}

auto create_program(const cl_context &ctx, const cl_device_id &d,
                    const char *filepath) -> cl_program {
  auto err = 0;

  std::ifstream fs(filepath);
  std::string src((std::istreambuf_iterator<char>(fs)),
                  std::istreambuf_iterator<char>());
  auto program = clCreateProgramWithSource(ctx, 1, (const char **)&src.c_str(),
                                           nullptr, // null terminated
                                           &err);
  if (program == nullptr) {
    set_error(err, "clCreateProgramWithSource");
    return nullptr;
  }
}

} // namespace clx
