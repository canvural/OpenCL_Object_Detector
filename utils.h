#ifndef __UTILS__H__
#define __UTILS__H__

#include <CL/opencl.h>

int connect_to_device(cl_device_type dev_type, cl_platform_id *platform, cl_device_id *device_id);
char* read_file(const char* filename);
void print_device_info (cl_device_id dev_id);
void print_platform_info (cl_platform_id platform);
const char* get_ocl_error(cl_int error);

#endif