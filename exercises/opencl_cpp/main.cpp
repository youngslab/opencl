#include <iostream>
#include <sstream>
#include <vector>
#include <fmt/format.h>

#include "cl/clx.hpp"
#include "cl/kernels.hpp"

#define BUFFER_SIZE 20
int A[BUFFER_SIZE];
int B[BUFFER_SIZE];
int C[BUFFER_SIZE];

int _main() {

  // init A, B, C
  for (int i = 0; i < BUFFER_SIZE; i++) {
    A[i] = i;
    B[i] = i * 2;
    C[i] = 0;
  }

  // 1. select platforms.
  std::vector<cl::Platform> platforms = clx::get_platforms();
  if (platforms.size() == 0) {
    std::cout << "no platforms found.\n";
  }

  cl_context_properties properties[] = {
      CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0]()), 0};

  // 2. create a context.
  cl::Context context(CL_DEVICE_TYPE_GPU, properties);

  // 3. query devices.
  std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
  for (auto &dev : devices) {
  }

  // 4. create command queue
  cl::CommandQueue queue(context, devices[0], 0);

  // 5. program object creation and build
  cl::Program::Sources sources(1, std::make_pair(clx::kernel::adder2, 0));

  cl::Program program(context, sources);

  try {
    program.build(devices);
  } catch (cl::Error &e) {
    for (auto &dev : devices) {
      // Check the build status
      cl_build_status status =
          program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(dev);
      if (status != CL_BUILD_ERROR)
        continue;

      // Get the build log
      std::string name = dev.getInfo<CL_DEVICE_NAME>();
      std::string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev);
      std::cerr << "Build log for " << name << ":" << std::endl
                << buildlog << std::endl;
    }
  }

  // 6. kernel & memory creation
  cl::Buffer aBuffer =
      cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                 BUFFER_SIZE * sizeof(int), (void *)&A[0]);
  cl::Buffer bBuffer =
      cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                 BUFFER_SIZE * sizeof(int), (void *)&B[0]);

  cl::Buffer cBuffer =
      cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                 BUFFER_SIZE * sizeof(int), (void *)&C[0]);

  cl::Kernel kernel(program, "vadd");
  kernel.setArg(0, aBuffer);
  kernel.setArg(1, bBuffer);
  kernel.setArg(2, cBuffer);

  // operations..
  queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(BUFFER_SIZE),
                             cl::NullRange);

  // kenerl won't be  not executed right now, so to block operations, just map
  // the the output to the host memory.
  int *output = (int *)queue.enqueueMapBuffer(
      cBuffer, CL_TRUE /* block */, CL_MAP_READ, 0, BUFFER_SIZE * sizeof(int));

  for (int i = 0; i < BUFFER_SIZE; i++) {
    std::cout << C[i] << " ";
  }
  std::cout << "\n";

  auto res = queue.enqueueUnmapMemObject(cBuffer, (void *)output);

  return 0;
}

int main() {
  try {
    return _main();
  } catch (cl::Error &err) {
    std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")"
              << std::endl;
  }
  return -1;
}
