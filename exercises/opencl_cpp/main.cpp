

#include <iostream>
#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#include "cl.hpp"

#define BUFFER_SIZE 20
int A[BUFFER_SIZE];
int B[BUFFER_SIZE];
int C[BUFFER_SIZE];

static char kernelSourceCode[] =
    "__kernel void \n"
    "vadd(__global int *a, __global int*b, __golbal int *c) \n"
    "{                                                      \n"
    "   size_t i = get_global_id(0);                        \n"
    "   c[i] = a[i] + b[i];                                 \n"
    "}                                                      \n";

std::vector<cl::Platform> get_platforms() {
  std::vector<cl::Platform> platforms;
  auto res = cl::Platform::get(&platforms);
  return platforms;
}

int _main() {

  // init A, B, C
  for (int i = 0; i < BUFFER_SIZE; i++) {
    A[i] = i;
    B[i] = i * 2;
    C[i] = 0;
  }

  // 1. select platforms.
  std::vector<cl::Platform> platforms = get_platforms();
  if (platforms.size() == 0) {
    std::cout << "no platforms found.\n";
  }
  cl_context_properties properties[] = {
      CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0]()), 0};

  // 2. create a context.
  cl::Context context(CL_DEVICE_TYPE_GPU, properties);

  // 3. query devices.
  std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

  // 4. create command queue
  cl::CommandQueue queue(context, devices[0], 0);

  // 5. program object creation and build
  cl::Program::Sources sources(1, std::make_pair(kernelSourceCode, 0));
  cl::Program program(context, sources);
  program.build(devices);

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
}

int main() {
  try {
    _main();
  } catch (cl::Error &err) {
    std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")"
              << std::endl;
  }
}
