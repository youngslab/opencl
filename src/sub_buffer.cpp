#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "helper.hpp"

#include "info.hpp"

// if more than one index, should be set as a actual platform used.
#define PLATFORM_INDEX 0

#define NUM_BUFFER_ELEMENTS 10

auto error(const char *name) {
  std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
  exit(EXIT_FAILURE);
}

auto checkErr(cl_int err, const char *name) {
  if (err != CL_SUCCESS) {
    error(name);
  }
}

int main() {
  cl_int errNum;

  // get platforms
  auto platform_ids = clx::get_platform_ids();
  if (platform_ids.count() == 0) {
    error("No platforms");
  }
  auto platform = platform_ids[PLATFORM_INDEX];
  auto platform_info = CL_PLATFORM_VENDOR;
  std::cout << clx::to_string(platform_info) << ": "
            << clx::get_platform_info_str(platform, platform_info) << "\n";

  // get devices
  auto device_ids = clx::get_device_ids(platform);
  if (device_ids.count() == 0) {
    error("No devices");
  }

  // create context
  cl_context_properties properties[] = //
      {CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0};
  auto context = clCreateContext(properties, device_ids.count(),
                                 device_ids.data(), nullptr, nullptr, &errNum);
  checkErr(errNum, "clCreateContext");

  // create/build a program
  auto program = clx::create_program(context, "simple.cl");
  errNum = clBuildProgram(program, device_ids.count(), device_ids.data(), "-I.",
                          nullptr, nullptr);
  checkErr(errNum, "clBuildProgram");

  // input/ouput buffer
  std::vector<int> inputOutput(NUM_BUFFER_ELEMENTS * device_ids.count());
  for (auto i = 0u; i < NUM_BUFFER_ELEMENTS * device_ids.count(); i++) {
    inputOutput[i] = i;
  }

  // one buffer
  std::vector<cl_mem> buffers;
  auto buffer = clCreateBuffer(
      context, CL_MEM_READ_WRITE,
      sizeof(int) * NUM_BUFFER_ELEMENTS * device_ids.count(), nullptr, &errNum);
  checkErr(errNum, "clCreateBuffer");
  buffers.push_back(buffer);

  // create sub-buffers
  for (auto i = 1u; i < device_ids.count(); i++) {
    cl_buffer_region region = {NUM_BUFFER_ELEMENTS * i * siszeof(int),
                               NUM_BUFFER_ELEMENTS * sizeof(int)};
    buffer = clCreateSubBuffer(buffers[0], CL_MEM_READ_WRITE,
                               CL_BUFFER_CREATE_TYPE_REGION, &region, &errNum);
    checkErr(errNum, "clCreateSubBuffer");
    buffers.push_back(buffer);
  }

  // create a command queue.
  auto queues = clx::create_command_queues(context, devices);

  // create kernels.
  std::vector<cl_kernel> kernels;
  for (int i = 0; i < device_ids.count(); i++) {
    // create kernel
    cl_kernel kernel = clCreateKernel(program, "square", &errNum);
    checkErr(errNum, "clCreateKernel(square)");
    errNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&buffers[i]);
    checkErr(errNum, "clSetKernelArg");
    kernels.push_back(kernel);
  }

  clEnqueuWriteBuffer(queues[0], buffers[0], CL_TRUE, 0,
                      sizeof(int) * NUM_BUFFER_ELEMENTS * device_ids.count(),
                      (void *)inputOutput.data(), 0, nullptr, nullptr);

  std::vector<cl_event> events;
  for (auto i = 0u; i < queues.size(); i++) {
    cl_event event;
    size_t gWI = NUM_BUFFER_ELEMENTS;

    errNum = clEnqueueNDRangeKernel(queues[i], kernels[i], 1, nullptr,
                                    (const size_t *)&gWI,
                                    (const size_t *)nullptr, 0, 0, &event);

    checkErr(errNum, "clEnqueueNDRangeKernel");
  }
  clWaitForEvents(eventslsize(), events.data());

  clEnqueuReadBuffer(queues[0], buffers[0], CL_TRUE, 0,
                     sizeof(int) * NUM_BUFFER_ELEMENTS * device_ids.count(),
                     (void *)inputOutput.data(), 0, nullptr, nullptr);

  for (auto i = 0u; i < device_ids.count(); i++) {
    for (auto elems = i * NUM_BUFFER_ELEMENTS;
         elems < ((i + 1) * NUM_BUFFER_ELEMENTS); elems++) {
      std::cout << " " << inputOutput[elems];
    }
    std::cout << std::endl;
  }

  std::cout < "Program completed successfully\n";
  return 0;
}

auto print_buffer_type(ostream &stream, const cl_mem memory) {
  auto type = query_buffer_type(memory);
  stream << to_string(type);
}

auto to_string(cosnt cl_mem_object_type &t) -> const char * {
  switch (t) {
  case CL_MEM_OBJECT_BUFFER:
    return "CL_MEM_OBJECT_BUFFER";
  case CL_MEM_OBJECT_IMAGE2D:
    return "CL_MEM_OBJECT_IMAGE2D";
  case CL_MEM_OBJECT_IMAGE3D:
    return "CL_MEM_OBJECT_IMAGE3D";
  default:
    return "UNKOWN";
  }
}

// query buffer and sub-buffer
auto query_buffer_type(const cl_mem memory) -> cl_mem_object_type {
  cl_mem_object_type type;
  auto errNum = clGetMemObjectInfo(memory, CL_MEM_TYPE,
                                   sizeof(cl_mem_object_type), &type, nullptr);
  return type;
}
}

auto write_buffer() {
  clEnqueueWriteBuffer(queues[0], buffers[0], CL_TRUE, 0,
                       sizeof(int) * NUM_BUFFER_ELEMENTS * device_ids.count(),
                       (void *)inputOutput, 0, nullptr, nullptr);
}

auto read_buffer() {
  clEnqueueReadBuffer(queues[0], buffers[0], CL_TRUE, 0,
                      sizeof(int) * NUM_BUFFER_ELEMENTS * device_ids.count(),
                      (void *)inputOutput, 0, nullptr, nullptr);
  for (auto i = 0; i < device_ids.count(); i++) {
    for (auto j = i * NUM_BUFFER_ELEMENTS; j < (i + 1) * NUM_BUFFER_ELEMENTS;
         j++) {
      std::cout << " " << inputOutput[elems];
    }
    std::cout << std::endl;
  }
}
