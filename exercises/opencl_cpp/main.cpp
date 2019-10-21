

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

int main() {

  // init A, B, C
  for (int i = 0; i < BUFFER_SIZE; i++) {
    A[i] = i;
    B[i] = i * 2;
    C[i] = 0;
  }

  std::vector<cl::Platform> platforms;
  auto res = cl::Platform::get(&platforms);
  if (res != CL_SUCCESS) {
    std::cout << "failed to get platforms\n";
  }
  cl_context_properties cprops[] = {CL_CONTEXT_PLATFORM,
                                    (cl_context_properties)(platforms[0]()), 0};

  // create a context.
  cl::Context context(CL_DEVICE_TYPE_GPU, cprops);

  // query devices.
  std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

  // create command queue
  cl::CommandQueue queue(context, devices[0], 0);

  // program object creation and build
  cl::Program::Sources sources(1, std::make_pair(kernelSourceCode, 0));
  cl::Program program(context, sources);
  program.build(devices);

  // kernel & memory creation
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

  queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(BUFFER_SIZE),
                             cl::NullRange);

  // kenerl won't be  not executed right now,
  int *output = (int *)queue.enqueueMapBuffer(
      cBuffer, CL_TRUE /* block */, CL_MAP_READ, 0, BUFFER_SIZE * sizeof(int));

  for (int i = 0; i < BUFFER_SIZE; i++) {
    std::cout << C[i] << " ";
  }
  std::cout << "\n";

  res = queue.enqueueUnmapMemObject(cBuffer, (void *)output);
}

