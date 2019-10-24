
find_package(FREEIMAGE REQUIRED)
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

