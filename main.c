/**
 *
 *
 */

#define KERNEL_FILE "kernel.cl"
#define KERNEL_FUNC "box_filter"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

#include <CL/cl.h>

#include "utils.h"

int main(int argc, char *argv[])
{
	int err;
	
	cl_platform_id platform;
    cl_device_id device;
	cl_command_queue command_queue;
	cl_program program;
	cl_kernel kernel;
	cl_context context;
	
	size_t global_size[2] = {0};
	size_t local_size[2] = {20, 20};
	
	// find platform and device to use
    if (!connect_to_device(CL_DEVICE_TYPE_GPU, &platform, &device)){
		printf ("Unable to find suitable OpenCL device!\n");
        return 1;
    }
	
	char buffer[512];
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof (buffer), buffer, NULL);
    printf ("Using device: %s\n", buffer);
	
	context = clCreateContext(0, 1, &device, NULL, NULL, &err);
    if (!context || err != CL_SUCCESS){
        printf("ERROR: Failed to create a compute context! %s\n", get_ocl_error(err));
        return EXIT_FAILURE;
    }
	
	cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
    command_queue = clCreateCommandQueue (context, device, properties, &err);
    if (!command_queue || err != CL_SUCCESS){
        printf("ERROR: Failed to create a command commands! %s\n", get_ocl_error(err));
        return EXIT_FAILURE;
    }
	
	const char *strings[] = {NULL, NULL};
	strings[0] = read_file(KERNEL_FILE);

	if (strings[0] == NULL){
		fprintf(stderr, "Unable to find \"%s\"", KERNEL_FILE);
		return 1;
	}
	
	program = clCreateProgramWithSource(context, 1, (const char **) & strings[0], NULL, &err);
    if (!program || err != CL_SUCCESS)
    {
        printf("ERROR: Failed to create compute program! %s\n", get_ocl_error(err));
        return EXIT_FAILURE;
    }
	
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS){
		size_t len;
        char buffer[2048];

        printf("ERROR: Failed to build program executable! %s\n", get_ocl_error(err));
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return EXIT_FAILURE;
    }
	
	kernel = clCreateKernel(program, KERNEL_FUNC, &err);
    if (!kernel || err != CL_SUCCESS){
        printf("ERROR: Failed to create compute kernel! %s\n", get_ocl_error(err));
        exit(1);
    }
	
	
	
	cvReleaseImage(&input_image);
	cvReleaseImage(&im322);
	cvReleaseImage(&output_image);
	cvReleaseImage(&output_image2);
	
	clReleaseMemObject(input_image_cl);
	clReleaseMemObject(output_image_cl);
	clReleaseCommandQueue(command_queue);
	clReleaseKernel(kernel);
	clReleaseContext(context);
	clReleaseProgram(program);
	
	return 0;
}