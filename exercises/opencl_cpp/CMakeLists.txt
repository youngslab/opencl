
find_package(fmt REQUIRED)

add_executable(opencl_cpp main.cpp)

target_link_libraries(opencl_cpp 
  ${OpenCL_Impl}::${OpenCL_Impl}
  fmt::fmt
)

target_include_directories(opencl_cpp 
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(opencl_cpp 
  PROPERTIES
    CXX_STANDARD 17
)
