#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <CL/opencl.h>

 void print_device_info (cl_device_id dev_id)
{
  cl_ulong ul_prop;
  cl_uint ui_prop;
  size_t size_prop;
  char buffer[8*1024];

  clGetDeviceInfo (dev_id, CL_DEVICE_NAME, sizeof (buffer), buffer, NULL);
  printf ("    DEVICE: %s\n", buffer);
  clGetDeviceInfo (dev_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof (ui_prop), &ui_prop, NULL);
  printf ("    MAX COMPUTE UNITS: %u\n", ui_prop);
  clGetDeviceInfo (dev_id, CL_DEVICE_VERSION, sizeof (buffer), buffer, NULL);
  printf ("    DEVICE VERSION: %s\n", buffer);
  clGetDeviceInfo (dev_id, CL_DRIVER_VERSION, sizeof (buffer), buffer, NULL);
  printf ("    DRIVER VERSION: %s\n", buffer);

  clGetDeviceInfo (dev_id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof (ul_prop), &ul_prop, NULL);
  printf ("    GLOBAL MEM SIZE: %lu\n", ul_prop);
  clGetDeviceInfo (dev_id, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof (ul_prop), &ul_prop, NULL);
  printf ("    GLOBAL MEM CACHE SIZE: %lu\n", ul_prop);
  clGetDeviceInfo (dev_id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof (ul_prop), &ul_prop, NULL);
  printf ("    LOCAL MEM SIZE: %lu\n", ul_prop);
  clGetDeviceInfo (dev_id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof (ui_prop), &ui_prop, NULL);
  printf ("    MAX CLOCK FREQUENCY: %u\n", ui_prop);
  clGetDeviceInfo (dev_id, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof (size_prop), &size_prop, NULL);
  printf ("    TIMER RESOLUTION: %lu\n", size_prop);
}

 void print_platform_info (cl_platform_id platform)
{
  char buffer[8*1024];

  clGetPlatformInfo (platform, CL_PLATFORM_NAME, sizeof (buffer), buffer, NULL);
  printf ("PLATFORM_NAME: %s\n", buffer);
  clGetPlatformInfo (platform, CL_PLATFORM_VERSION, sizeof (buffer), buffer, NULL);
  printf ("  VERSION: %s\n", buffer);
  clGetPlatformInfo (platform, CL_PLATFORM_VENDOR, sizeof (buffer), buffer, NULL);
  printf ("  VENDOR: %s\n", buffer);
  clGetPlatformInfo (platform, CL_PLATFORM_PROFILE, sizeof (buffer), buffer, NULL);
  printf ("  PROFILE: %s\n", buffer);
}

 int connect_to_device(cl_device_type dev_type, cl_platform_id *platform, cl_device_id *device_id)
{
	cl_int err_code;
    cl_uint num_plat;
	cl_device_id *dev_ids;
    
	clGetPlatformIDs (0, NULL, &num_plat);
	
	if (num_plat == 1) printf("There is 1 platform available\n");
    else printf("There are %u platforms available\n", num_plat);
	
	if (num_plat <= 0) return 0;
	
	cl_platform_id *ids = malloc (sizeof (cl_platform_id) * num_plat);
    clGetPlatformIDs (num_plat, ids, NULL);

    for (unsigned int i = 0; i < num_plat; i++){
		print_platform_info (ids[i]);
		
		cl_uint num_devs;
		err_code = clGetDeviceIDs(ids[i], dev_type, 0, NULL, &num_devs);
		if (err_code != CL_SUCCESS || num_devs <= 0) continue;

		dev_ids = malloc (sizeof(cl_device_id) * num_devs);
		clGetDeviceIDs(ids[i], dev_type, num_devs, dev_ids, NULL);
		
		for (unsigned int j = 0; j < num_devs; j++){
			print_device_info(dev_ids[j]);
		}
	}
	
	*platform = ids[0];
	*device_id = dev_ids[0];
	
	return 1;
}

char* read_file(const char* filename)
{
	FILE* f = NULL;
	size_t file_size;
	
	f = fopen (filename, "r");
	if (f == NULL){
		return NULL;
	}

	// get the length
	fseek (f, 0, SEEK_END);
	file_size = ftell (f);
	fseek (f, 0, SEEK_SET);
	
	// allocate a buffer and fill it
	char* contents = (char *)malloc (file_size + 1);
	if (fread(contents, file_size, sizeof (char), f) != 1){
		fclose (f);
		free (contents);
		return NULL;
	}

	fclose (f);
	contents[file_size] = '\0';

	return contents;
}
 
 const char* get_ocl_error(cl_int error)
{
    switch(error) {
		case(CL_SUCCESS):                           return "Success";
		case(CL_DEVICE_NOT_FOUND):                  return "Device not found!";
		case(CL_DEVICE_NOT_AVAILABLE):              return "Device not available!";
		case(CL_MEM_OBJECT_ALLOCATION_FAILURE):     return "Memory object allocation failure!";
		case(CL_OUT_OF_RESOURCES):                  return "Out of resources!";
		case(CL_OUT_OF_HOST_MEMORY):                return "Out of host memory!";
		case(CL_PROFILING_INFO_NOT_AVAILABLE):      return "Profiling information not available!";
		case(CL_MEM_COPY_OVERLAP):                  return "Overlap detected in memory copy operation!";
		case(CL_IMAGE_FORMAT_MISMATCH):             return "Image format mismatch detected!";
		case(CL_IMAGE_FORMAT_NOT_SUPPORTED):        return "Image format not supported!";
		case(CL_INVALID_VALUE):                     return "Invalid value!";
		case(CL_INVALID_DEVICE_TYPE):               return "Invalid device type!";
		case(CL_INVALID_DEVICE):                    return "Invalid device!";
		case(CL_INVALID_CONTEXT):                   return "Invalid context!";
		case(CL_INVALID_QUEUE_PROPERTIES):          return "Invalid queue properties!";
		case(CL_INVALID_COMMAND_QUEUE):             return "Invalid command queue!";
		case(CL_INVALID_HOST_PTR):                  return "Invalid host pointer address!";
		case(CL_INVALID_MEM_OBJECT):                return "Invalid memory object!";
		case(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR):   return "Invalid image format descriptor!";
		case(CL_INVALID_IMAGE_SIZE):                return "Invalid image size!";
		case(CL_INVALID_SAMPLER):                   return "Invalid sampler!";
		case(CL_INVALID_BINARY):                    return "Invalid binary!";
		case(CL_INVALID_BUILD_OPTIONS):             return "Invalid build options!";
		case(CL_INVALID_PROGRAM):                   return "Invalid program object!";
		case(CL_INVALID_PROGRAM_EXECUTABLE):        return "Invalid program executable!";
		case(CL_INVALID_KERNEL_NAME):               return "Invalid kernel name!";
		case(CL_INVALID_KERNEL):                    return "Invalid kernel object!";
		case(CL_INVALID_ARG_INDEX):                 return "Invalid index for kernel argument!";
		case(CL_INVALID_ARG_VALUE):                 return "Invalid value for kernel argument!";
		case(CL_INVALID_ARG_SIZE):                  return "Invalid size for kernel argument!";
		case(CL_INVALID_KERNEL_ARGS):               return "Invalid kernel arguments!";
		case(CL_INVALID_WORK_DIMENSION):            return "Invalid work dimension!";
		case(CL_INVALID_WORK_GROUP_SIZE):           return "Invalid work group size!";
		case(CL_INVALID_GLOBAL_OFFSET):             return "Invalid global offset!";
		case(CL_INVALID_EVENT_WAIT_LIST):           return "Invalid event wait list!";
		case(CL_INVALID_EVENT):                     return "Invalid event!";
		case(CL_INVALID_OPERATION):                 return "Invalid operation!";
		case(CL_INVALID_GL_OBJECT):                 return "Invalid OpenGL object!";
		case(CL_INVALID_BUFFER_SIZE):               return "Invalid buffer size!";
		default:                                    return "Unknown error!";
    };
    
    return "Unknown Error";
}