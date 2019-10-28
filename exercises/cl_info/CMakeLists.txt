
find_package(OpenCL) 
if(OPENCL_FOUND) 
message(STATUS "opencl found")
set(OpenCL_Impl OpenCL)
else()
message(STATUS "opencl not found")
find_package(khronos-opencl-icd-loader) 
set(OpenCL_Impl khronos-opencl-icd-loader) 
endif(OPENCL_FOUND)

find_package(fmt REQUIRED)

add_executable(cl_info cl_info.cpp)

target_link_libraries(cl_info 
  ${OpenCL_Impl}::${OpenCL_Impl}
  fmt::fmt
  pthread
  dl
  )

target_include_directories(cl_info 
  PRIVATE
  ${CMAKE_SOURCE_DIR}/include)

set_target_properties(cl_info PROPERTIES
              CXX_STANDARD 17)

