
cmake_minimum_required(VERSION 3.1)

project(hello_cl) 

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake") 
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan") 
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake") 
endif() 

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing)
  
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_BINARY_DIR})

find_package(khronos-opencl-icd-loader) 

add_executable(hello src/HelloWorld.cpp
  ./res/HelloWorld.cl)

set(RESOURCE_FILES ./res/HelloWorld.cl)

target_link_libraries(hello
  khronos-opencl-icd-loader::khronos-opencl-icd-loader)

install(TARGETS hello 
  RUNTIME DESTINATION bin
  RESOURCE DESTINATION bin/res 
)
