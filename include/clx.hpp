

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <vector>
#include <string>
#include <algorithm>

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

template <typename T, typename R> struct _get_info {
  auto operator()(T const &t, cl_uint const &info) -> R {
    R r;
    get_info(t, info, sizeof(R), &r, nullptr);
    return r;
  }
};

template <typename T> struct _get_info<T, std::string> {
  auto operator()(T const &t, cl_uint const &info) -> std::string {
    auto size = get_info_size(t, info);
    auto cs = std::vector<char>(size);
    get_info(t, info, size, cs.data(), nullptr);

    // create output string
    auto out = std::string{};
    std::transform(std::begin(cs), std::end(cs), std::back_inserter(out),
                   [](auto c) { return c; });
    return out;
  }
};

template <cl_uint Info, typename T>
auto get_info(T const &t) -> return_type_t<Info> {
  return _get_info<std::decay_t<T>, return_type_t<Info>>{}(t, Info);
}

} // namespace detail

auto get_platform_info_name(cl_platform_id const &id) -> std::string {
  return detail::get_info<CL_PLATFORM_NAME>(id);
}

auto get_platform_info_vendor(cl_platform_id const &id) -> std::string {
  return detail::get_info<CL_PLATFORM_VENDOR>(id);
}

auto get_platform_info_extension(cl_platform_id const &id) -> std::string {
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

auto create_context(cl_platform_id const &platform,
                    std::vector<cl_device_id> &devices) -> cl_context {
  cl_context_properties properties[] = {CL_CONTEXT_PLATFORM,
                                        (cl_context_properties)platform, 0};
  return clCreateContext(properties, devices.size(), devices.data(), nullptr,
                         nullptr, nullptr);
}

} // namespace clx
