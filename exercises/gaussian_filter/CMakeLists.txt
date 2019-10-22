
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

add_executable(gaussian_filter main.cpp)

target_link_libraries(gaussian_filter 
  ${OpenCL_Impl}::${OpenCL_Impl}
  ${FREEIMAGE_LIBRARIES} 
  fmt::fmt
  )

target_include_directories(gaussian_filter 
  PRIVATE
  ${FREEIMAGE_INCLUDE_DIRS})

set_target_properties(gaussian_filter PROPERTIES
              CXX_STANDARD 17)
FILE(COPY ./gausian_filter.cl DESTINATION "${CMAKE_BINARY_DIR}/bin")

