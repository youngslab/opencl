#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <array>

#include <fmt/format.h>
#include "sx.hpp"

namespace clx {

cl_int g_err;
char const *g_func;

auto set_err_if_err(cl_int const &err, char const *func) -> void {
  if (err != CL_SUCCESS) {
    g_err = err;
    g_func = func;
  }
}

namespace detail {

auto get_info_size(cl_device_id const &id, cl_device_info const &info)
    -> std::size_t {
  auto size = std::size_t{};
  auto err = clGetDeviceInfo(id, info, 0, nullptr, &size);
  set_err_if_err(err, "clGetDeviceInfo");
  return size;
}

auto get_info_size(cl_platform_id const &id, cl_platform_info const &info)
    -> std::size_t {
  auto size = std::size_t{};
  auto err = clGetPlatformInfo(id, info, 0, nullptr, &size);
  set_err_if_err(err, "clGetPlatformInfo");
  return size;
}

auto get_info_size(cl_context const &ctx, cl_context_info const &info)
    -> std::size_t {
  auto size = std::size_t{};
  auto err = clGetContextInfo(ctx, info, 0, nullptr, &size);
  set_err_if_err(err, "clGetContextInfo");
  return size;
}

auto get_info_size(cl_program const &p, cl_device_id const &d,
                   cl_program_build_info const &info) -> std::size_t {
  auto size = std::size_t{};
  auto err = clGetProgramBuildInfo(p, d, info, 0, nullptr, &size);
  set_err_if_err(err, "clGetProgramBuildInfo");
  return size;
}

template <typename T, cl_uint Info>
auto get_info_size(T const &t) -> std::size_t {
  return get_info_size(t, Info);
}

// cl_uint -> cl_device_info
template <cl_uint Info> struct return_type { using type = cl_uint; };
template <> struct return_type<CL_PLATFORM_NAME> { using type = std::string; };
template <> struct return_type<CL_PLATFORM_PROFILE> {
  using type = std::string;
};
template <> struct return_type<CL_PLATFORM_VENDOR> {
  using type = std::string;
};
template <> struct return_type<CL_PLATFORM_VERSION> {
  using type = std::string;
};
template <> struct return_type<CL_PLATFORM_EXTENSIONS> {
  using type = std::string;
};
template <> struct return_type<CL_DEVICE_TYPE> { using type = cl_device_type; };
template <> struct return_type<CL_DEVICE_NAME> { using type = std::string; };
template <> struct return_type<CL_DEVICE_VENDOR> { using type = std::string; };
template <> struct return_type<CL_DEVICE_VERSION> { using type = std::string; };
template <> struct return_type<CL_DEVICE_PROFILE> { using type = std::string; };
template <> struct return_type<CL_DEVICE_EXTENSIONS> {
  using type = std::string;
};
template <> struct return_type<CL_CONTEXT_DEVICES> {
  using type = std::vector<cl_device_id>;
};

// PROGRAM
template <> struct return_type<CL_PROGRAM_BUILD_LOG> {
  using type = std::string;
};

template <cl_uint Info> using return_type_t = typename return_type<Info>::type;

auto get_info(cl_platform_id const &id, cl_uint const &info,
              size_t param_value_size, void *param_value,
              size_t *param_value_size_ret) -> cl_int {
  auto err = clGetPlatformInfo(id, info, param_value_size, param_value,
                               param_value_size_ret);
  set_err_if_err(err, "clGetPlatformInfo");
  return err;
}

auto get_info(cl_device_id const &id, cl_uint const &info,
              size_t param_value_size, void *param_value,
              size_t *param_value_size_ret) -> cl_int {
  auto err = clGetDeviceInfo(id, info, param_value_size, param_value,
                             param_value_size_ret);
  set_err_if_err(err, "clGetDeviceInfo");
  return err;
}

auto get_info(cl_context const &ctx, cl_uint const &info,
              size_t param_value_size, void *param_value,
              size_t *param_value_size_ret) -> cl_int {
  auto err = clGetContextInfo(ctx, info, param_value_size, param_value,
                              param_value_size_ret);
  set_err_if_err(err, "clGetContextInfo");
  return err;
}

auto get_info(cl_program const &p, cl_device_id const &d, cl_uint const &info,
              size_t param_value_size, void *param_value,
              size_t *param_value_size_ret) -> cl_int {
  auto err = clGetProgramBuildInfo(p, d, info, param_value_size, param_value,
                                   param_value_size_ret);
  set_err_if_err(err, "clGetProgramBuildInfo");
  return err;
}

template <typename R, typename... Ts> struct _get_info {
  auto operator()(Ts... ts, cl_uint const &info) -> R {
    R r;
    get_info(ts..., info, sizeof(R), &r, nullptr);
    return r;
  }
};

template <typename... Ts> struct _get_info<std::string, Ts...> {
  auto operator()(Ts... ts, cl_uint const &info) -> std::string {
    auto size = get_info_size(ts..., info);
    auto cs = std::vector<char>(size);
    get_info(ts..., info, size, cs.data(), nullptr);

    // create output string
    auto out = std::string{};
    std::transform(std::begin(cs), std::end(cs), std::back_inserter(out),
                   [](auto c) { return c; });
    return out;
  }
};

// template <cl_uint Info, typename T>
// auto get_info(T const &t) -> return_type_t<Info> {
// return _get_info<return_type_t<Info>, T>{}(t, Info);
//}

template <cl_uint Info, typename... Ts>
auto get_info(Ts... ts) -> return_type_t<Info> {
  return _get_info<return_type_t<Info>, Ts...>{}(ts..., Info);
}

auto get_property_name(cl_platform_id const &i) -> cl_context_properties {
  return CL_CONTEXT_PLATFORM;
}

auto create_context_properties_impl() -> cl_context_properties { return 0; }

template <typename T, typename... Ts>
auto create_context_properties_impl(T t, Ts... ts)
    -> std::array<cl_context_properties, sizeof...(Ts) * 2 + 3> {
  return sx::make_array(get_property_name(t), (cl_context_properties)t,
                        create_context_properties_impl(ts...));
}

} // namespace detail

auto get_platform_info_name(cl_platform_id const &id) -> std::string {
  return detail::get_info<CL_PLATFORM_NAME>(id);
}

auto get_platform_info_vendor(cl_platform_id const &id) -> std::string {
  return detail::get_info<CL_PLATFORM_VENDOR>(id);
}

auto get_platform_info_extensions(cl_platform_id const &id) -> std::string {
  return detail::get_info<CL_PLATFORM_EXTENSIONS>(id);
}

auto get_platform_info_profile(cl_platform_id const &id) -> std::string {
  return detail::get_info<CL_PLATFORM_PROFILE>(id);
}

auto get_platform_info_version(cl_platform_id const &id) -> std::string {
  return detail::get_info<CL_PLATFORM_VERSION>(id);
}

auto get_device_info_type(cl_device_id const &id) {
  return detail::get_info<CL_DEVICE_TYPE>(id);
}

auto get_device_info_profile(cl_device_id const &id) -> std::string {
  return detail::get_info<CL_DEVICE_PROFILE>(id);
}

auto get_device_info_name(cl_device_id const &id) -> std::string {
  return detail::get_info<CL_DEVICE_NAME>(id);
}

auto get_device_info_extensions(cl_device_id const &id) -> std::string {
  return detail::get_info<CL_DEVICE_EXTENSIONS>(id);
}

auto get_device_info_vendor(cl_device_id const &id) -> std::string {
  return detail::get_info<CL_DEVICE_VENDOR>(id);
}

auto get_program_build_info_log(cl_program const &p, cl_device_id const &id)
    -> std::string {
  return detail::get_info<CL_PROGRAM_BUILD_LOG>(p, id);
}

auto get_platform_id_count() -> uint32_t {
  auto cnt = 0u;
  auto err = clGetPlatformIDs(0, nullptr, &cnt);
  set_err_if_err(err, "clGetPlatformIDs");
  return cnt;
}

auto get_platform_ids() -> std::vector<cl_platform_id> {
  auto cnt = get_platform_id_count();
  std::vector<cl_platform_id> ids(cnt);
  auto err = clGetPlatformIDs(cnt, ids.data(), nullptr);
  set_err_if_err(err, "clGetPlatformIDs");
  return ids;
}

auto get_device_id_count(cl_platform_id const &id, cl_device_type const &type)
    -> std::size_t {
  auto cnt = cl_uint{0};
  auto err = clGetDeviceIDs(id, type, 0, nullptr, &cnt);
  set_err_if_err(err, "clGetDeviceIDs");
  return cnt;
}

auto get_device_ids(cl_platform_id const &id, cl_device_type const &type)
    -> std::vector<cl_device_id> {
  auto cnt = get_device_id_count(id, type);
  std::vector<cl_device_id> ids(cnt);
  auto err = clGetDeviceIDs(id, type, cnt, ids.data(), nullptr);
  set_err_if_err(err, "clGetDeviceIDs");
  return ids;
}

using context_callback = void(CL_CALLBACK *)(const char *errinfo,
                                             const void *private_info,
                                             size_t cb, void *user_data);

auto create_context(cl_platform_id const &platform,
                    std::vector<cl_device_id> const &devices,
                    context_callback cb, void *user_data) -> cl_context {
  cl_int err;
  cl_context_properties properties[] = {CL_CONTEXT_PLATFORM,
                                        (cl_context_properties)platform, 0};
  auto ctx = clCreateContext(properties, devices.size(), devices.data(), cb,
                             user_data, &err);
  set_err_if_err(err, "clCreateContext");
  return ctx;
}

auto create_context(cl_platform_id const &platform,
                    std::vector<cl_device_id> const &devices) -> cl_context {
  return create_context(platform, devices, nullptr, nullptr);
}

template <typename... Ts> auto create_context_properties(Ts... ts) {
  return detail::create_context_properties_impl(ts...);
}

auto create_context_from_type(std::vector<cl_context_properties> const &ps,
                              cl_device_type const &t, context_callback cb,
                              void *user_data) -> cl_context {
  cl_int err;
  auto ctx = clCreateContextFromType(ps.data(), t, cb, user_data, &err);
  set_err_if_err(err, "clCreateContextFromType");
  return ctx;
}
auto create_context_from_type(std::vector<cl_context_properties> const &ps,
                              cl_device_type const &t) {
  return create_context_from_type(ps, t, nullptr, nullptr);
}

template <std::size_t N>
auto create_context_from_type(std::array<cl_context_properties, N> const &ps,
                              cl_device_type const &t, context_callback cb,
                              void *user_data) -> cl_context {
  cl_int err;
  auto ctx = clCreateContextFromType(ps.data(), t, cb, user_data, &err);
  set_err_if_err(err, "clCreateContextFromType");
  return ctx;
}

template <std::size_t N>
auto create_context_from_type(std::array<cl_context_properties, N> const &ps,
                              cl_device_type const &t) {
  return create_context_from_type(ps, t, nullptr, nullptr);
}

auto create_program(cl_context ctx, char const *file) -> cl_program {
  std::ifstream srcFile(file);
  if (!srcFile.is_open()) {
    return nullptr;
  }
  std::string code(std::istreambuf_iterator<char>(srcFile),
                   (std::istreambuf_iterator<char>()));

  auto code_ptr = code.c_str();
  auto size_ptr = code.size();
  // Create program from source
  cl_int err;
  auto program = clCreateProgramWithSource(ctx, 1, &code_ptr, &size_ptr, &err);
  set_err_if_err(err, "clCreateProgramWithSource");
  return program;
}

auto build_program(cl_program const &p, std::vector<cl_device_id> const &ds)
    -> bool {
  // Build program
  auto err = clBuildProgram(p, ds.size(), ds.data(), nullptr, nullptr, nullptr);
  set_err_if_err(err, "clBuildProgram");
  return err == CL_SUCCESS;
}

auto create_kernel(cl_program const &p, char const *name) -> cl_kernel {
  cl_int err;
  auto kernel = clCreateKernel(p, name, &err);
  set_err_if_err(err, "clCreateKernel");
  return kernel;
}

auto create_buffer(cl_context const &ctx, cl_mem_flags const &flags,
                   size_t size, void *host_ptr) -> cl_mem {
  auto err = cl_int{};
  auto mem = clCreateBuffer(ctx, flags, size, host_ptr, &err);
  set_err_if_err(err, "clCreateBuffer");
  return mem;
}

auto create_command_queue(cl_context const &c, cl_device_id const &d,
                          cl_command_queue_properties const &ps)
    -> cl_command_queue {
  auto err = cl_int{};
  auto q = clCreateCommandQueue(c, d, ps, &err);
  set_err_if_err(err, "clCreateCommandQueue");
  return q;
}

auto set_arguments_impl(cl_kernel const &k, std::size_t i) {
  return CL_SUCCESS;
}

template <typename T, typename... Ts>
auto set_arguments_impl(cl_kernel const &k, std::size_t i, T const &t,
                        Ts... ts) {
  auto err = clSetKernelArg(k, i, sizeof(T), &t);
  set_err_if_err(err, "clSetKernelArg");
  return set_arguments_impl(k, i + 1, ts...);
}

template <typename... Ts> auto set_arguments(cl_kernel const &k, Ts... ts) {
  return set_arguments_impl(k, 0, ts...);
}

auto to_string(cl_device_id const &id) -> std::string {
  return fmt::format("[name:{}, vendor:{}, profile:{}, type:{}, extensions:{}]",
                     get_device_info_name(id), get_device_info_vendor(id),
                     get_device_info_profile(id), get_device_info_type(id),
                     get_device_info_extensions(id));
}

auto to_string(cl_platform_id const &id) -> std::string {
  return fmt::format("[name:{}, vendor:{}, profile:{},  extensions:{}]",
                     get_platform_info_name(id), get_platform_info_vendor(id),
                     get_platform_info_profile(id),
                     get_platform_info_extensions(id));
}

auto enqueue_nd_ranage_kernel(
    cl_command_queue command_queue, cl_kernel kernel, cl_uint work_dim,
    const size_t *global_work_offset, const size_t *global_work_size,
    const size_t *local_work_size, cl_uint num_events_in_wait_list,
    const cl_event *event_wait_list, cl_event *event) -> cl_int {
  return clEnqueueNDRangeKernel(
      command_queue, kernel, work_dim, global_work_offset, global_work_size,
      local_work_size, num_events_in_wait_list, event_wait_list, event);
}

auto enqueue_nd_ranage_kernel(cl_command_queue command_queue, cl_kernel kernel,
                              cl_uint work_dim,
                              const size_t *global_work_offset,
                              const size_t *global_work_size,
                              const size_t *local_work_size) -> cl_int {
  return enqueue_nd_ranage_kernel(command_queue, kernel, work_dim,
                                  global_work_offset, global_work_size,
                                  local_work_size, 0, nullptr, nullptr);
}

auto enqueue_read_buffer(cl_command_queue command_queue, cl_mem buffer,
                         cl_bool blocking_read, size_t offset, size_t cb,
                         void *ptr, cl_uint num_events_in_wait_list,
                         const cl_event *event_wait_list, cl_event *event)
    -> cl_int {
  return clEnqueueReadBuffer(command_queue, buffer, blocking_read, offset, cb,
                             ptr, num_events_in_wait_list, event_wait_list,
                             event);
}

auto enqueue_read_buffer(cl_command_queue command_queue, cl_mem buffer,
                         cl_bool blocking_read, size_t offset, size_t cb,
                         void *ptr) -> cl_int {
  return enqueue_read_buffer(command_queue, buffer, blocking_read, offset, cb,
                             ptr, 0, nullptr, nullptr);
}
} // namespace clx
