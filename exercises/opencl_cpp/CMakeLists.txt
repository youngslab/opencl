

find_package(khronos-opencl-icd-loader REQUIRED)

add_executable(opencl_cpp main.cpp)

target_link_libraries(opencl_cpp 
  khronos-opencl-icd-loader::khronos-opencl-icd-loader 
  )

message(STATUS "CMAKE_SORUCE_DIR: ${CMAKE_SOURCE_DIR}")

target_include_directories(opencl_cpp 
  PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  )

