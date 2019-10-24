
cmake_minimum_required(VERSION 3.1)

project(hello_cl) 

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake") 
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan") 
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake") 
endif() 

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(CONANFILE conanfile.txt
    PROFILE ${CMAKE_CONNAN_PROFILE}
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing)
 
set(CMAKE_SYSTEM_PREFIX_PATH ${CMAKE_SYSTEM_PREFIX_PATH}
    $ENV{HOMEBREW_PREFIX})
 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(OpenCL) 
if(OPENCL_FOUND) 
message(STATUS "opencl found")
set(OpenCL_Impl OpenCL)
else()
message(STATUS "opencl not found")
find_package(khronos-opencl-icd-loader) 
set(OpenCL_Impl khronos-opencl-icd-loader) 
endif(OPENCL_FOUND)

add_subdirectory(third_party)
add_subdirectory(exercises)

find_package(khronos-opencl-icd-loader) 

add_executable(hello src/HelloWorld.cpp
  ./res/HelloWorld.cl)


set(RESOURCE_FILES ./res/HelloWorld.cl)

target_link_libraries(hello
  khronos-opencl-icd-loader::khronos-opencl-icd-loader)

set_target_properties(hello PROPERTIES
              CXX_STANDARD 17)

install(TARGETS hello 
  RUNTIME DESTINATION bin
  RESOURCE DESTINATION bin/res 
)

add_executable(helper_example src/main.cpp)

target_link_libraries(helper_example
  khronos-opencl-icd-loader::khronos-opencl-icd-loader)

set_target_properties(helper_example PROPERTIES
              CXX_STANDARD 17)




