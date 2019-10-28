

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

add_executable(clx clx.cpp)

target_link_libraries(clx 
  ${OpenCL_Impl}::${OpenCL_Impl}
  fmt::fmt
  pthread
  dl
  )

target_include_directories(clx 
  PRIVATE
  ${CMAKE_SOURCE_DIR}/include)

set_target_properties(clx PROPERTIES
              CXX_STANDARD 17)
# FILE(COPY ./gausian_filter.cl DESTINATION "${CMAKE_BINARY_DIR}/bin")

