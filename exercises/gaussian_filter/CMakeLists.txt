
find_package(FREEIMAGE REQUIRED)

find_package(khronos-opencl-icd-loader REQUIRED)

add_executable(gaussian_filter main.cpp)

target_link_libraries(gaussian_filter 
  khronos-opencl-icd-loader::khronos-opencl-icd-loader 
  ${FREEIMAGE_LIBRARIES} 
  )

target_include_directories(gaussian_filter 
  PRIVATE
  ${FREEIMAGE_INCLUDE_DIRS})

