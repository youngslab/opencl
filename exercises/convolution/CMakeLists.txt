

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

add_executable(convolution Convolution.cpp)

target_link_libraries(convolution 
  ${OpenCL_Impl}::${OpenCL_Impl}
  fmt::fmt
  pthread
  dl
  )

target_include_directories(convolution 
  PRIVATE
  ${CMAKE_SOURCE_DIR}/include)

set_target_properties(convolution PROPERTIES
              CXX_STANDARD 17)

 FILE(COPY ./Convolution.cl DESTINATION "${CMAKE_BINARY_DIR}/bin")

