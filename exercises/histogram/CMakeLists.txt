
find_package(FREEIMAGE REQUIRED)

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

add_executable(histogram histogram.cpp)

target_link_libraries(histogram 
  ${OpenCL_Impl}::${OpenCL_Impl}
  ${FREEIMAGE_LIBRARIES} 
  fmt::fmt
  pthread
  dl
  )

target_include_directories(histogram 
  PRIVATE
  ${FREEIMAGE_INCLUDE_DIRS})

set_target_properties(histogram PROPERTIES
              CXX_STANDARD 17)

FILE(COPY histogram_image.cl DESTINATION "${CMAKE_BINARY_DIR}/bin")

