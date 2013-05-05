/**
 * 22.04.2013
 * Can VURAL && Engin CULHACI
 *
 * Waldboost training using LBP as weak classifiers.
 */

#define KERNEL_FILE "kernel.cl"
#define KERNEL_FUNC "train"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <OpenCV/cv.h>
#include <OpenCV/highgui.h>
#include <sys/types.h>
#include <dirent.h>

#include <CL/cl.h>

#include "utils.h"
#include "main.h"
#include "weakclassifier.h"

#define IMAGE_WIDTH 320

int current_size = 0;

int main(int argc, char *argv[])
{
	int err;
	cl_platform_id platform;
	cl_device_id device;
	cl_command_queue command_queue;
	cl_program program;
	cl_kernel kernel;
	cl_context context;
	
	

    //##############################//
    //########OpenCL Stuff##########//
    //##############################//

    // find platform and device to use
    if (!connect_to_device(CL_DEVICE_TYPE_GPU, &platform, &device)) {
        printf ("Unable to find suitable OpenCL device!\n");
        return 1;
    }

    char buffer[512];
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof (buffer), buffer, NULL);
    printf ("Using device: %s\n", buffer);

    context = clCreateContext(0, 1, &device, NULL, NULL, &err);

    if (!context || err != CL_SUCCESS) {
        printf("ERROR: Failed to create a compute context! %s\n", get_ocl_error(err));
        return EXIT_FAILURE;
    }

    cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
    command_queue = clCreateCommandQueue (context, device, properties, &err);

    if (!command_queue || err != CL_SUCCESS) {
        printf("ERROR: Failed to create a command commands! %s\n", get_ocl_error(err));
        return EXIT_FAILURE;
    }

    const char *strings[] = {NULL, NULL};
    strings[0] = read_file(KERNEL_FILE);

    if (strings[0] == NULL) {
        fprintf(stderr, "Unable to find \"%s\"", KERNEL_FILE);
        return 1;
    }

    program = clCreateProgramWithSource(context, 1, (const char **) & strings[0], NULL, &err);

    if (!program || err != CL_SUCCESS) {
        printf("ERROR: Failed to create compute program! %s\n", get_ocl_error(err));
        return EXIT_FAILURE;
    }

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    if (err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];

        printf("ERROR: Failed to build program executable! %s\n", get_ocl_error(err));
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return EXIT_FAILURE;
    }

    kernel = clCreateKernel(program, KERNEL_FUNC, &err);

    if (!kernel || err != CL_SUCCESS) {
        printf("ERROR: Failed to create compute kernel! %s\n", get_ocl_error(err));
        exit(1);
    }
	
	edizinami **all_histograms = malloc(0 * sizeof(edizinami*));
	if(all_histograms == NULL) {
		printf("Cannot allocate memory!");
		return -1;
	}
	
	// Get all weak classifiers and dataset we'll use
	read_all_histograms(&all_histograms);
	
	for(int i = 0; i < current_size; i++) {
		int total = 0;
		//printf("Hallelujah! %s\n", all_histograms[i]->filename);
		
		// Initial weights
		all_histograms[i]->weight = 1 / current_size;
	}
	
	for(int i = 0; i < current_size; i++) {
		free(all_histograms[i]->filename);
		free(all_histograms[i]->data);
		free(all_histograms[i]);
	}
	free(all_histograms);

	clReleaseCommandQueue(command_queue);
    clReleaseKernel(kernel);
    clReleaseContext(context);
    clReleaseProgram(program);
    return 0;
}

void *read_all_histograms(edizinami ***all_histograms)
{
	DIR *dp;
    struct dirent *ep;
    dp = opendir ("./faces/");
    char *d;
	
	if (dp != NULL) {
        while (ep = readdir (dp)) {
            d = strstr(ep->d_name, ".txt");
			
			if(d == NULL) {
				continue;
            }
			//printf("\tFile found! %s\n", ep->d_name);
			
			edizinami **tmp = realloc(*all_histograms, ++current_size * sizeof(char*));
			if(NULL == tmp) {
				printf("Cannot alllocate memory");
				exit(-1);
			}
			
			*all_histograms = tmp;
			edizinami *histogram = malloc(1 * sizeof(edizinami));
			histogram->filename = malloc((strlen(ep->d_name) + 9) * sizeof(char));
			strcpy(histogram->filename, "./faces/");
			strcat(histogram->filename, ep->d_name);
			histogram->data = read_histogram_file(histogram->filename);
			if(NULL == histogram->data) {
				printf("Fatal Error!!!!!");
				exit(-1);
			}
			
			histogram->fob = 1;
			(*all_histograms)[current_size-1] = histogram;
		}
	}
	else {
        printf("Couldn't open the directory");
		return NULL;
    }
	closedir(dp);
	
	//Non-face histograms
	dp = opendir ("./nonfaces/");
	
	if (dp != NULL) {
		while (ep = readdir (dp)) {
            d = strstr(ep->d_name, ".txt");
			
			if(d == NULL) {
				continue;
            }
			//printf("\tFile found! %s\n", ep->d_name);
			
			edizinami **tmp = realloc(*all_histograms, ++current_size * sizeof(char*));
			if(NULL == tmp) {
				printf("Cannot alllocate memory");
				exit(-1);
			}
			
			*all_histograms = tmp;
			edizinami *histogram = malloc(1 * sizeof(edizinami));
			histogram->filename = malloc((strlen(ep->d_name) + 12) * sizeof(char));
			strcpy(histogram->filename, "./nonfaces/");
			strcat(histogram->filename, ep->d_name);
			histogram->data = read_histogram_file(histogram->filename);
			if(NULL == histogram->data) {
				printf("Fatal Error!!!!!");
				exit(-1);
			}
			
			histogram->fob = 0;
			(*all_histograms)[current_size-1] = histogram;
		}
	}
	else {
        printf("Couldn't open the directory");
		return NULL;
    }
	
	closedir(dp);
	return NULL;
}