//
// Book:      OpenCL(R) Programming Guide
// Authors:   Aaftab Munshi, Benedict Gaster, Timothy Mattson, James Fung, Dan
// Ginsburg ISBN-10:   0-321-74964-2 ISBN-13:   978-0-321-74964-2 Publisher:
// Addison-Wesley Professional URLs: http://safari.informit.com/9780132488006/
//            http://www.openclprogrammingguide.com
//

// Convolution.cpp
//
//    This is a simple example that demonstrates OpenCL platform, device, and
//    context use.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#if !defined(CL_CALLBACK)
#define CL_CALLBACK
#endif

#include "clx.hpp"
#include <fmt/format.h>

// Constants
const unsigned int inputSignalWidth = 8;
const unsigned int inputSignalHeight = 8;

cl_uint inputSignal[inputSignalWidth][inputSignalHeight] = {
    {3, 1, 1, 4, 8, 2, 1, 3}, {4, 2, 1, 1, 2, 1, 2, 3},
    {4, 4, 4, 4, 3, 2, 2, 2}, {9, 8, 3, 8, 9, 0, 0, 0},
    {9, 3, 3, 9, 0, 0, 0, 0}, {0, 9, 0, 8, 0, 0, 0, 0},
    {3, 0, 8, 8, 9, 4, 4, 4}, {5, 9, 8, 1, 8, 1, 1, 1}};

const unsigned int outputSignalWidth = 6;
const unsigned int outputSignalHeight = 6;

cl_uint outputSignal[outputSignalWidth][outputSignalHeight];

const unsigned int maskWidth = 3;
const unsigned int maskHeight = 3;

cl_uint mask[maskWidth][maskHeight] = {
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
};

///
// Function to check and handle OpenCL errors
inline void checkErr(cl_int err, const char *name) {
  if (err != CL_SUCCESS) {
    std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
    exit(EXIT_FAILURE);
  }
}

///
//	main() for Convoloution example
//
int main(int argc, char **argv) {

  // First, select an OpenCL platform to run on.
  auto platforms = clx::get_platform_ids();
  auto platform_id = cl_platform_id{};
  auto cpu_devices = std::vector<cl_device_id>{};

  // selecte devices
  for (auto const &platform : platforms) {
    platform_id = platform;
    cpu_devices = clx::get_device_ids(platform_id, CL_DEVICE_TYPE_CPU);
    if (cpu_devices.size() != 0) {
      break;
    }
  }
  if (cpu_devices.size() == 0) {
    std::cout << "No CPU device found" << std::endl;
    exit(-1);
  }

  auto context = clx::create_context(
      platform_id, cpu_devices,
      [](const char *, const void *, size_t, void *) { exit(1); }, nullptr);

  std::ifstream srcFile("Convolution.cl");
  checkErr(srcFile.is_open() ? CL_SUCCESS : -1, "reading Convolution.cl");

  std::string srcProg(std::istreambuf_iterator<char>(srcFile),
                      (std::istreambuf_iterator<char>()));

  const char *src = srcProg.c_str();
  size_t length = srcProg.length();

  // Create program from source
  auto program = clx::create_program(context, "");
  auto success = clx::build_program(program, cpu_devices);
  if (!success) {
    fmt::print("{}\n",
               clx::get_program_build_info_log(program, cpu_devices[0]));
  }

  auto kernel = clx::create_kernel(program, "");

  auto input_signal_buffer =
      clx::create_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                         sizeof(cl_uint) * inputSignalHeight * inputSignalWidth,
                         static_cast<void *>(inputSignal));

  auto mask_buffer = clx::create_buffer(
      context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      sizeof(cl_uint) * maskHeight * maskWidth, static_cast<void *>(mask));

  auto output_signal_buffer = clx::create_buffer(
      context, CL_MEM_WRITE_ONLY,
      sizeof(cl_uint) * outputSignalHeight * outputSignalWidth, nullptr);

  auto queue = clx::create_command_queue(context, cpu_devices[0], 0);

  clx::set_arguments(kernel, input_signal_buffer, mask_buffer,
                     output_signal_buffer, outputSignalWidth, maskWidth);

  /*
  const size_t globalWorkSize[1] = {outputSignalWidth * outputSignalHeight};
  const size_t localWorkSize[1] = {1};

  // Queue the kernel up for execution across the array
  errNum = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalWorkSize,
                                  localWorkSize, 0, NULL, NULL);
  checkErr(errNum, "clEnqueueNDRangeKernel");

  errNum = clEnqueueReadBuffer(queue, outputSignalBuffer, CL_TRUE, 0,
                               sizeof(cl_uint) * outputSignalHeight *
                                   outputSignalHeight,
                               outputSignal, 0, NULL, NULL);
  checkErr(errNum, "clEnqueueReadBuffer");

  // Output the result buffer
  for (int y = 0; y < outputSignalHeight; y++) {
    for (int x = 0; x < outputSignalWidth; x++) {
      std::cout << outputSignal[x][y] << " ";
    }
    std::cout << std::endl;
  }

  std::cout << std::endl << "Executed program succesfully." << std::endl;

  return 0;
  */
}

