/*
 * This code is a heavily edited version of an example OpenCL program provided by Intel.
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */

#include <cassert>
#include "clSetup.h"
#include "utils.h"

#define CL_INTEL_SDK

//Global container for convenient ocl info
ocl_args_d_t *ocl;

ocl_args_d_t::ocl_args_d_t(int nQueues, int nKernels):
	context(NULL),
	device(NULL),
	program(NULL),
	platformVersion(OPENCL_VERSION_2_0),
	deviceVersion(OPENCL_VERSION_2_0),
	compilerVersion(OPENCL_VERSION_2_0),
	commandQueues(NULL),
	deviceQueue(NULL),
	kernels(NULL),
	nQueues(nQueues),
	nKernels(nKernels)
{
	//For now
	if(nQueues!=nKernels) {
		if(nQueues != 1)
		{
			assert(0);
		}
	}
	//assert(nQueues==nKernels);

	commandQueues = (cl_command_queue *)malloc(nQueues*sizeof(cl_command_queue));
	kernels = (cl_kernel *)malloc(nKernels*sizeof(cl_kernel));

}

ocl_args_d_t::~ocl_args_d_t()
{
		cl_int err = CL_SUCCESS;

		if (kernels)
		{
			for( int i = 0; i < nKernels; i++)
			{
				if(kernels[i])
				{
					CHECK_ERRORS(clReleaseKernel(kernels[i]));
				}
			}
		}

		if (program)
		{
			CHECK_ERRORS(clReleaseProgram(program));
		}
		if (commandQueues)
		{
			for( int i = 0; i < nQueues; i++)
			{
				if(commandQueues[i])
				{
					CHECK_ERRORS(clReleaseCommandQueue(commandQueues[i]));
				}
			}

		}
		if(deviceQueue)
		{
			CHECK_ERRORS(clReleaseCommandQueue(deviceQueue));
		}
		if (device)
		{
			CHECK_ERRORS(clReleaseDevice(device));
		}
		if (context)
		{
			CHECK_ERRORS(clReleaseContext(context));
		}
}


const char* TranslateOpenCLError(cl_int errorCode)
{
		switch(errorCode)
		{
		case CL_SUCCESS:                            return "CL_SUCCESS";
		case CL_DEVICE_NOT_FOUND:                   return "CL_DEVICE_NOT_FOUND";
		case CL_DEVICE_NOT_AVAILABLE:               return "CL_DEVICE_NOT_AVAILABLE";
		case CL_COMPILER_NOT_AVAILABLE:             return "CL_COMPILER_NOT_AVAILABLE";
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
		case CL_OUT_OF_RESOURCES:                   return "CL_OUT_OF_RESOURCES";
		case CL_OUT_OF_HOST_MEMORY:                 return "CL_OUT_OF_HOST_MEMORY";
		case CL_PROFILING_INFO_NOT_AVAILABLE:       return "CL_PROFILING_INFO_NOT_AVAILABLE";
		case CL_MEM_COPY_OVERLAP:                   return "CL_MEM_COPY_OVERLAP";
		case CL_IMAGE_FORMAT_MISMATCH:              return "CL_IMAGE_FORMAT_MISMATCH";
		case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
		case CL_BUILD_PROGRAM_FAILURE:              return "CL_BUILD_PROGRAM_FAILURE";
		case CL_MAP_FAILURE:                        return "CL_MAP_FAILURE";
		case CL_MISALIGNED_SUB_BUFFER_OFFSET:       return "CL_MISALIGNED_SUB_BUFFER_OFFSET";                          //-13
		case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";   //-14
		case CL_COMPILE_PROGRAM_FAILURE:            return "CL_COMPILE_PROGRAM_FAILURE";                               //-15
		case CL_LINKER_NOT_AVAILABLE:               return "CL_LINKER_NOT_AVAILABLE";                                  //-16
		case CL_LINK_PROGRAM_FAILURE:               return "CL_LINK_PROGRAM_FAILURE";                                  //-17
		case CL_DEVICE_PARTITION_FAILED:            return "CL_DEVICE_PARTITION_FAILED";                               //-18
		case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";                         //-19
		case CL_INVALID_VALUE:                      return "CL_INVALID_VALUE";
		case CL_INVALID_DEVICE_TYPE:                return "CL_INVALID_DEVICE_TYPE";
		case CL_INVALID_PLATFORM:                   return "CL_INVALID_PLATFORM";
		case CL_INVALID_DEVICE:                     return "CL_INVALID_DEVICE";
		case CL_INVALID_CONTEXT:                    return "CL_INVALID_CONTEXT";
		case CL_INVALID_QUEUE_PROPERTIES:           return "CL_INVALID_QUEUE_PROPERTIES";
		case CL_INVALID_COMMAND_QUEUE:              return "CL_INVALID_COMMAND_QUEUE";
		case CL_INVALID_HOST_PTR:                   return "CL_INVALID_HOST_PTR";
		case CL_INVALID_MEM_OBJECT:                 return "CL_INVALID_MEM_OBJECT";
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
		case CL_INVALID_IMAGE_SIZE:                 return "CL_INVALID_IMAGE_SIZE";
		case CL_INVALID_SAMPLER:                    return "CL_INVALID_SAMPLER";
		case CL_INVALID_BINARY:                     return "CL_INVALID_BINARY";
		case CL_INVALID_BUILD_OPTIONS:              return "CL_INVALID_BUILD_OPTIONS";
		case CL_INVALID_PROGRAM:                    return "CL_INVALID_PROGRAM";
		case CL_INVALID_PROGRAM_EXECUTABLE:         return "CL_INVALID_PROGRAM_EXECUTABLE";
		case CL_INVALID_KERNEL_NAME:                return "CL_INVALID_KERNEL_NAME";
		case CL_INVALID_KERNEL_DEFINITION:          return "CL_INVALID_KERNEL_DEFINITION";
		case CL_INVALID_KERNEL:                     return "CL_INVALID_KERNEL";
		case CL_INVALID_ARG_INDEX:                  return "CL_INVALID_ARG_INDEX";
		case CL_INVALID_ARG_VALUE:                  return "CL_INVALID_ARG_VALUE";
		case CL_INVALID_ARG_SIZE:                   return "CL_INVALID_ARG_SIZE";
		case CL_INVALID_KERNEL_ARGS:                return "CL_INVALID_KERNEL_ARGS";
		case CL_INVALID_WORK_DIMENSION:             return "CL_INVALID_WORK_DIMENSION";
		case CL_INVALID_WORK_GROUP_SIZE:            return "CL_INVALID_WORK_GROUP_SIZE";
		case CL_INVALID_WORK_ITEM_SIZE:             return "CL_INVALID_WORK_ITEM_SIZE";
		case CL_INVALID_GLOBAL_OFFSET:              return "CL_INVALID_GLOBAL_OFFSET";
		case CL_INVALID_EVENT_WAIT_LIST:            return "CL_INVALID_EVENT_WAIT_LIST";
		case CL_INVALID_EVENT:                      return "CL_INVALID_EVENT";
		case CL_INVALID_OPERATION:                  return "CL_INVALID_OPERATION";
		case CL_INVALID_GL_OBJECT:                  return "CL_INVALID_GL_OBJECT";
		case CL_INVALID_BUFFER_SIZE:                return "CL_INVALID_BUFFER_SIZE";
		case CL_INVALID_MIP_LEVEL:                  return "CL_INVALID_MIP_LEVEL";
		case CL_INVALID_GLOBAL_WORK_SIZE:           return "CL_INVALID_GLOBAL_WORK_SIZE";                           //-63
		case CL_INVALID_PROPERTY:                   return "CL_INVALID_PROPERTY";                                   //-64
		case CL_INVALID_IMAGE_DESCRIPTOR:           return "CL_INVALID_IMAGE_DESCRIPTOR";                           //-65
		case CL_INVALID_COMPILER_OPTIONS:           return "CL_INVALID_COMPILER_OPTIONS";                           //-66
		case CL_INVALID_LINKER_OPTIONS:             return "CL_INVALID_LINKER_OPTIONS";                             //-67
		case CL_INVALID_DEVICE_PARTITION_COUNT:     return "CL_INVALID_DEVICE_PARTITION_COUNT";                     //-68
//    case CL_INVALID_PIPE_SIZE:                  return "CL_INVALID_PIPE_SIZE";                                  //-69
//    case CL_INVALID_DEVICE_QUEUE:               return "CL_INVALID_DEVICE_QUEUE";                               //-70

		default:
				return "UNKNOWN ERROR CODE";
		}
}

cl_int printPlatformInfo(cl_platform_id platform) {

	cl_int err;

	char * retCharP;
	size_t retSizet;

	cl_platform_info pInfo[5] = {CL_PLATFORM_PROFILE,CL_PLATFORM_VERSION,CL_PLATFORM_NAME,CL_PLATFORM_VENDOR,CL_PLATFORM_EXTENSIONS};
	char ** InfoName = (char **)malloc(sizeof(char *) * 5);

	for(int i = 0; i < 5; i++) {
		InfoName[i] = (char *)malloc(sizeof(char)*32);
	}
	InfoName[0] = "CL_PLATFORM_PROFILE";
	InfoName[1] = "CL_PLATFORM_VERSION";
	InfoName[2] = "CL_PLATFORM_NAME";
	InfoName[3] = "CL_PLATFORM_VENDOR";
	InfoName[4] = "CL_PLATFORM_EXTENSIONS";

	printf("Printing Platform Info:\n");
	for(int i = 0; i < 5; i++) {

		CHECK_ERRORS(clGetPlatformInfo(platform, pInfo[i], 0, NULL, &retSizet));

		retCharP = (char *)malloc(sizeof(char)*retSizet);

		CHECK_ERRORS(clGetPlatformInfo(platform, pInfo[i], retSizet, retCharP, NULL));

		printf("\t%s returns: ",InfoName[i]);
		printf("\n\t\t%s\n",retCharP);

	}

	return 0;

}

cl_int printDeviceInfo(cl_device_id device) {

	cl_device_info info;
	cl_int err;
	cl_int ret_int;
	size_t ret_sizet;
	cl_device_fp_config ret_fp_config;
	char * ret_CharP;
	cl_device_svm_capabilities ret_svm_cap;
		cl_ulong ret_ulong;
	cl_command_queue_properties ret_cmd_props;


	printf("\nPrinting Device info-------------------\n");

	info = CL_DEVICE_MAX_CLOCK_FREQUENCY;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_int), &ret_int, NULL));

	printf("\t CL_DEVICE_MAX_CLOCK_FREQUENCY returns: %d\n", ret_int);

	info = CL_DEVICE_ADDRESS_BITS;

	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_int), &ret_int, NULL));

	printf("\t CL_DEVICE_ADDRESS_BITS returns: %d\n", ret_int);

	info = CL_DEVICE_MAX_COMPUTE_UNITS;

	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_int), &ret_int, NULL));

	printf("\t CL_DEVICE_MAX_COMPUTE_UNITS returns: %d\n", ret_int);

	info = CL_DEVICE_MAX_WORK_GROUP_SIZE;

	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(size_t), &ret_sizet, NULL));

	printf("\t CL_DEVICE_MAX_WORK_GROUP_SIZE returns: %d\n", ret_sizet);

	info = CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS;

	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_int), &ret_int, NULL));

	printf("\t CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS returns: %d\n", ret_int);

	size_t *retVect_sizet = (size_t *)malloc(sizeof(size_t)*ret_int);

	info = CL_DEVICE_MAX_WORK_ITEM_SIZES;

	CHECK_ERRORS(clGetDeviceInfo(device, info, ret_int*sizeof(size_t), retVect_sizet, NULL));

		for (int i = 0; i < ret_int; i++) {
			printf("\t CL_DEVICE_MAX_WORK_ITEM_SIZES %d returns: %d\n", i, retVect_sizet[i]);
		}

	info = CL_DEVICE_DOUBLE_FP_CONFIG;

	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_device_fp_config), &ret_fp_config, NULL));

		printf("\t CL_DEVICE_DOUBLE_FP_CONFIG returns: %ul\n", ret_fp_config);

	info = CL_DEVICE_EXTENSIONS;
	CHECK_ERRORS(clGetDeviceInfo(device, info, 0, NULL, &ret_sizet));
	ret_CharP = (char *)malloc(sizeof(char) * ret_sizet);
	CHECK_ERRORS(clGetDeviceInfo(device, info, ret_sizet, ret_CharP, NULL));

		for (int i = 0; i < (int)strlen(ret_CharP); i++)
		{
			if (ret_CharP[i] == ' ')
				ret_CharP[i] = '\n';
		}
		printf("\t CL_DEVICE_EXTENSIONS returns: %s\n", ret_CharP);

	info = CL_DEVICE_SVM_CAPABILITIES;

	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(ret_svm_cap), &ret_svm_cap, NULL));

		printf("map :%X\n",ret_svm_cap);
		printf("Course :%X\n",CL_DEVICE_SVM_COARSE_GRAIN_BUFFER);
		printf("FINE :%X\n",CL_DEVICE_SVM_FINE_GRAIN_BUFFER);
		printf("SYS :%X\n",CL_DEVICE_SVM_FINE_GRAIN_SYSTEM);
		printf("ATOM :%X\n",CL_DEVICE_SVM_ATOMICS);
		if(ret_svm_cap & CL_DEVICE_SVM_COARSE_GRAIN_BUFFER){
			ret_CharP = "CL_DEVICE_SVM_COARSE_GRAIN_BUFFER";
			printf("\t CL_DEVICE_SVM_CAPABILITIES returns: %s\n", ret_CharP);
		}
		if(ret_svm_cap & CL_DEVICE_SVM_FINE_GRAIN_BUFFER){
			ret_CharP = "CL_DEVICE_SVM_FINE_GRAIN_BUFFER";
			printf("\t CL_DEVICE_SVM_CAPABILITIES returns: %s\n", ret_CharP);
		}
		if(ret_svm_cap & CL_DEVICE_SVM_FINE_GRAIN_SYSTEM){
			ret_CharP = "CL_DEVICE_SVM_FINE_GRAIN_SYSTEM";
			printf("\t CL_DEVICE_SVM_CAPABILITIES returns: %s\n", ret_CharP);
		}
		if(ret_svm_cap & CL_DEVICE_SVM_ATOMICS){
			ret_CharP = "CL_DEVICE_SVM_ATOMICS";
			printf("\t CL_DEVICE_SVM_CAPABILITIES returns: %s\n", ret_CharP);
		}

	info = CL_DEVICE_MAX_CONSTANT_ARGS;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_int), &ret_int, NULL));
	printf("\t CL_DEVICE_MAX_CONSTANT_ARGS returns: %d\n", ret_int);

	info = CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_ulong), &ret_ulong, NULL));
	printf("\t CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE returns: %lu\n", ret_ulong);

	info = CL_DEVICE_GLOBAL_MEM_CACHE_SIZE;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_ulong), &ret_ulong, NULL));
	printf("\t CL_DEVICE_GLOBAL_MEM_CACHE_SIZE returns: %lu\n", ret_ulong);

		info = CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_int), &ret_int, NULL));
	printf("\t CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE returns: %d\n", ret_int);

	info = CL_DEVICE_GLOBAL_MEM_SIZE;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_ulong), &ret_ulong, NULL));
	printf("\t CL_DEVICE_GLOBAL_MEM_SIZE returns: %lu\n", ret_ulong);

	info = CL_DEVICE_LOCAL_MEM_SIZE;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_ulong), &ret_ulong, NULL));
	printf("\t CL_DEVICE_LOCAL_MEM_SIZE returns: %lu\n", ret_ulong);

	info = CL_DEVICE_MAX_ON_DEVICE_QUEUES;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_int), &ret_int, NULL));
	printf("\t CL_DEVICE_MAX_ON_DEVICE_QUEUES returns: %d\n", ret_int);

	info = CL_DEVICE_MAX_ON_DEVICE_EVENTS;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_int), &ret_int, NULL));
	printf("\t CL_DEVICE_MAX_ON_DEVICE_EVENTS returns: %d\n", ret_int);

	info = CL_DEVICE_QUEUE_ON_HOST_PROPERTIES;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_command_queue_properties), &ret_cmd_props, NULL));

	printf("\t CL_DEVICE_QUEUE_ON_HOST_PROPERTIES  returns: \n");
	if(ret_cmd_props & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE) {
		printf("\t\tCL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE\n");
	}
	if(ret_cmd_props & CL_QUEUE_PROFILING_ENABLE) {
		printf("\t\tCL_QUEUE_PROFILING_ENABLE\n");
	}

	info = CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_command_queue_properties), &ret_cmd_props, NULL));

	printf("\t CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES returns: \n");
	if(ret_cmd_props & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE) {
		printf("\t\t CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE \n");
	}
	if(ret_cmd_props & CL_QUEUE_PROFILING_ENABLE) {
		printf("\t\t CL_QUEUE_PROFILING_ENABLE \n");
	}

	info = CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_int), &ret_int, NULL));
	printf("\t CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE returns: %d\n", ret_int);

	info = CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE;
	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(cl_int), &ret_int, NULL));
	printf("\t CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE returns: %d\n", ret_int);

				info = CL_DEVICE_VERSION;
	CHECK_ERRORS(clGetDeviceInfo(device, info, 0, NULL, &ret_sizet));
	ret_CharP = (char *)malloc(sizeof(char) * ret_sizet);
	CHECK_ERRORS(clGetDeviceInfo(device, info, ret_sizet, ret_CharP, NULL));

	printf("\t CL_DEVICE_VERSION returns: %s\n", ret_CharP);

				info = CL_DRIVER_VERSION;
	CHECK_ERRORS(clGetDeviceInfo(device, info, 0, NULL, &ret_sizet));
	ret_CharP = (char *)malloc(sizeof(char) * ret_sizet);
	CHECK_ERRORS(clGetDeviceInfo(device, info, ret_sizet, ret_CharP, NULL));

	printf("\t CL_DRIVER_VERSION returns: %s\n", ret_CharP);

				info = CL_DEVICE_OPENCL_C_VERSION;
	CHECK_ERRORS(clGetDeviceInfo(device, info, 0, NULL, &ret_sizet));
	ret_CharP = (char *)malloc(sizeof(char) * ret_sizet);
	CHECK_ERRORS(clGetDeviceInfo(device, info, ret_sizet, ret_CharP, NULL));

	printf("\t CL_DEVICE_OPENCL_C_VERSION returns: %s\n", ret_CharP);

	info = CL_DEVICE_PROFILING_TIMER_RESOLUTION;

	CHECK_ERRORS(clGetDeviceInfo(device, info, sizeof(size_t), &ret_sizet, NULL));

	printf("\t CL_DEVICE_PROFILING_TIMER_RESOLUTION returns: %d\n", ret_sizet);

	return err;
}

cl_int printKernelInfo(cl_kernel kernel)
{

		//printf("Printing Kernel Info\n");

		cl_kernel_work_group_info info;

		size_t ret3[3];
		size_t ret;
		cl_ulong ret_long;

		void * p;


		info = CL_KERNEL_GLOBAL_WORK_SIZE; //size_t[3]
		/*p = (void *)ret3;
		CHECK_ERRORS(clGetKernelWorkGroupInfo(kernel,ocl->device,info,3*sizeof(size_t),p,NULL));

		printf("CL_KERNEL_GLOBAL_WORK_SIZE: \n");
		for(int i = 0; i < 3; i++)
		{
			printf("\t%d",ret3[i]);

		}*/


		info = CL_KERNEL_WORK_GROUP_SIZE; // size_t

		info = CL_KERNEL_COMPILE_WORK_GROUP_SIZE; //size_t[3]

		info = CL_KERNEL_LOCAL_MEM_SIZE; //cl_ulong
		p = (void *)&ret_long;
		CHECK_ERRORS(clGetKernelWorkGroupInfo(kernel,ocl->device,info,sizeof(ret_long),p,NULL));
		printf("CL_KERNEL_LOCAL_MEM_SIZE: %lu\n",ret_long);

		info = CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE; //size_t
		p = (void *)&ret;
		CHECK_ERRORS(clGetKernelWorkGroupInfo(kernel,ocl->device,info,sizeof(ret),p,NULL));
		printf("CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE: %d\n",ret);

		info = CL_KERNEL_PRIVATE_MEM_SIZE; //cl_ulong
		p = (void *)&ret_long;
		CHECK_ERRORS(clGetKernelWorkGroupInfo(kernel,ocl->device,info,sizeof(ret_long),p,NULL));
		printf("CL_KERNEL_PRIVATE_MEM_SIZE: %lu\n",ret_long);
}


/*
 * Check whether an OpenCL platform is the required platform
 * (based on the platform's name)
 */
bool CheckPreferredPlatformMatch(cl_platform_id platform, const char* preferredPlatform)
{
		size_t stringLength = 0;
		cl_int err = CL_SUCCESS;
		bool match = false;

		// In order to read the platform's name, we first read the platform's name string length (param_value is NULL).
		// The value returned in stringLength
		err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, NULL, &stringLength);
		CHECK_ERRORS(err);

		// Now, that we know the platform's name string length, we can allocate enough space before read it
		//std::vector<char> platformName(stringLength);
		char * platformName = (char *)malloc(stringLength*sizeof(char));

		// Read the platform's name string
		// The read value returned in platformName
		err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, stringLength, &platformName[0], NULL);
		CHECK_ERRORS(err);

#if 0
		for (std::vector<char>::const_iterator i = platformName.begin(); i != platformName.end(); ++i)
		printf("%c", *i);

		printf("\n");
#else
		//printf("%s\n",platformName);
#endif

		//printf("Comparing actual: '%s' to preferred: '%s'\n", platformName, preferredPlatform);

		// Now check if the platform's name is the required one
		if (strstr(&platformName[0], preferredPlatform) != 0)
		{
				// The checked platform is the one we're looking for
			//printf("MATCH actual: '%s' to preferred: '%s'\n", platformName, preferredPlatform);
				match = true;
		} else{
			//printf("NO MATCH actual: '%s' to preferred: '%s'\n", platformName, preferredPlatform);
		}

		return match;
}

/*
 * Find and return the preferred OpenCL platform
 * In case that preferredPlatform is NULL, the ID of the first discovered platform will be returned
 */
cl_platform_id FindOpenCLPlatform(const char* preferredPlatform, cl_device_type deviceType)
{
		cl_uint numPlatforms = 0;
		cl_int err = CL_SUCCESS;

		// Get (in numPlatforms) the number of OpenCL platforms available
		// No platform ID will be return, since platforms is NULL
		err = clGetPlatformIDs(0, NULL, &numPlatforms);
		CHECK_ERRORS(err);
		// printf("Number of available platforms: %u\n", numPlatforms);

		if (0 == numPlatforms)
		{
				fprintf(stderr,"Error: No platforms found!\n");
				return NULL;
		}

		std::vector<cl_platform_id> platforms(numPlatforms);

		// Now, obtains a list of numPlatforms OpenCL platforms available
		// The list of platforms available will be returned in platforms
		err = clGetPlatformIDs(numPlatforms, &platforms[0], NULL);
		CHECK_ERRORS(err);

		size_t stringLength = 0;
		//Print out platform names
		for (cl_uint i = 0; i < numPlatforms; i++)
		{
			// printf("Platform [%d]: ",i);
			CHECK_ERRORS(clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 0, NULL, &stringLength));
			std::vector<char> platformName(stringLength);

			CHECK_ERRORS(clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, stringLength, &platformName[0], NULL));
			printf("Platform %d: ", i);
			for (std::vector<char>::const_iterator j = platformName.begin(); j != platformName.end(); ++j)
			{
					printf("%c", *j);
				}
			printf("\n");
		}

		// Check if one of the available platform matches the preferred requirements
		for (cl_uint i = 0; i < numPlatforms; i++)
		{
				bool match = true;
				cl_uint numDevices = 0;

				// If the preferredPlatform is not NULL then check if platforms[i] is the required one
				// Otherwise, continue the check with platforms[i]
				if ((NULL != preferredPlatform) && (strlen(preferredPlatform) > 0))
				{
						// In case we're looking for a specific platform
						match = CheckPreferredPlatformMatch(platforms[i], preferredPlatform);
				}

				// match is true if the platform's name is the required one or don't care (NULL)
				if (match)
				{
						printf("Match Found %d\n",i);
						printPlatformInfo(platforms[i]);
						
						// Obtains the number of deviceType devices available on platform
						// When the function failed we expect numDevices to be zero.
						// We ignore the function return value since a non-zero error code
						// could happen if this platform doesn't support the specified device type.
						err = clGetDeviceIDs(platforms[i], deviceType, 0, NULL, &numDevices);
						CHECK_ERRORS(err);

						//printf("numDevices: %d\n",numDevices);
						if (0 != numDevices)
						{
								// There is at least one device that answer the requirements
								return platforms[i];
						}
				}
		}

		return NULL;
}

/*
 * This function read the OpenCL platdorm and device0G versions
 * (using clGetxxxInfo API) and stores it in the ocl structure.
 */
int GetPlatformAndDeviceVersion (cl_platform_id platformId)
{
		cl_int err = CL_SUCCESS;

		// Read the platform's version string length (param_value is NULL).
		// The value returned in stringLength
		size_t stringLength = 0;
		err = clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, 0, NULL, &stringLength);
		CHECK_ERRORS(err);

		// Now, that we know the platform's version string length, we can allocate enough space before read it
		std::vector<char> platformVersion(stringLength);

		// Read the platform's version string
		// The read value returned in platformVersion
		err = clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, stringLength, &platformVersion[0], NULL);
		CHECK_ERRORS(err);

		if (strstr(&platformVersion[0], "OpenCL 2.0") != NULL)
		{
				ocl->platformVersion = OPENCL_VERSION_2_0;
				printf("Platform Version is 2.0!!!!\n");
		} else {
				printf("Platform Version is Not 2.0!!!!\n");
		}

		// Read the device's version string length (param_value is NULL).
		err = clGetDeviceInfo(ocl->device, CL_DEVICE_VERSION, 0, NULL, &stringLength);
		CHECK_ERRORS(err);

		// Now, that we know the device's version string length, we can allocate enough space before read it
		std::vector<char> deviceVersion(stringLength);

		// Read the device's version string
		// The read value returned in deviceVersion
		err = clGetDeviceInfo(ocl->device, CL_DEVICE_VERSION, stringLength, &deviceVersion[0], NULL);
		CHECK_ERRORS(err);

		if (strstr(&deviceVersion[0], "OpenCL 2.0") != NULL)
		{
				ocl->deviceVersion = OPENCL_VERSION_2_0;
		}

		// Read the device's OpenCL C version string length (param_value is NULL).
		err = clGetDeviceInfo(ocl->device, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &stringLength);
		CHECK_ERRORS(err);

		// Now, that we know the device's OpenCL C version string length, we can allocate enough space before read it
		std::vector<char> compilerVersion(stringLength);

		// Read the device's OpenCL C version string
		// The read value returned in compilerVersion
		err = clGetDeviceInfo(ocl->device, CL_DEVICE_OPENCL_C_VERSION, stringLength, &compilerVersion[0], NULL);
		CHECK_ERRORS(err);

		if (strstr(&compilerVersion[0], "OpenCL C 2.0") != NULL)
		{
				ocl->compilerVersion = OPENCL_VERSION_2_0;
		}

		return err;
}

/*
 * This function picks/creates necessary OpenCL objects which are needed.
 * The objects are:
 * OpenCL platform, device, context, and command queue.
 */
int SetupOpenCL(cl_device_type deviceType)
{
		// The following variable stores return codes for all OpenCL calls.
		cl_int err = CL_SUCCESS;
		// Query for all available OpenCL platforms on the system
		// Here you enumerate all platforms and pick one which name has preferredPlatform as a sub-string
		//cl_platform_id platformId = FindOpenCLPlatform("Intel", deviceType);
		#ifdef CL_INTEL_SDK
			cl_platform_id platformId = FindOpenCLPlatform("Intel(R) OpenCL", deviceType);
		#else
			cl_platform_id platformId = FindOpenCLPlatform("Intel Gen OCL Driver", deviceType);
		#endif
		if (NULL == platformId)
		{
				fprintf(stderr,"Error: Failed to find OpenCL platform.\n");
				return CL_INVALID_VALUE;
		}

		// Create context with device of specified type.
		// Required device type is passed as function argument deviceType.
		cl_context_properties contextProperties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platformId, 0};
		ocl->context = clCreateContextFromType(contextProperties, deviceType, NULL, NULL, &err);
		if ((CL_SUCCESS != err) || (NULL == ocl->context))
		{
				fprintf(stderr,"Couldn't create a context, clCreateContextFromType() returned '%s'.\n", TranslateOpenCLError(err));
				return err;
		}

		// Query for OpenCL device which was used for context creation
		err = clGetContextInfo(ocl->context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &ocl->device, NULL);
		CHECK_ERRORS(err);

		// Read the OpenCL platform's version and the device OpenCL and OpenCL C versions
		GetPlatformAndDeviceVersion(platformId);

		// Create command queue.
		// OpenCL kernels are enqueued for execution to a particular device through special objects called command queues.
		// Command queue guarantees some ordering between calls and other OpenCL commands.
		for(int i = 0; i < ocl->nQueues; i++)
		{
			if (OPENCL_VERSION_2_0 == ocl->deviceVersion)
			{
				//printf("USING OPENCL 2.0\n");
				//const cl_command_queue_properties properties[] = { CL_QUEUE_SIZE, CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE ,CL_QUEUE_PROPERTIES, (cl_command_queue_properties)(CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_ON_DEVICE | CL_QUEUE_ON_DEVICE_DEFAULT | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE), 0 };

				//const cl_command_queue_properties properties[] = {CL_QUEUE_PROPERTIES, (cl_command_queue_properties)(CL_QUEUE_PROFILING_ENABLE), 0 };
				const cl_command_queue_properties properties[] = { 0 };

				ocl->commandQueues[i] = clCreateCommandQueueWithProperties(ocl->context, ocl->device, properties, &err);
			}
			else {
				// default behavior: OpenCL 1.2
				printf("ERROR: LaunchDaemon System does not work with OpenCL 1.2\n");
				exit(1);
			}

			CHECK_ERRORS(err);
		}


		return CL_SUCCESS;
}

/*
 * Create and build OpenCL program from its source code
 */
int CreateAndBuildProgram(std::string kernel_file_path, std::string kernel_file_name, std::string includes)
{
		cl_int err = CL_SUCCESS;
		std::string build_options_str;
		if(includes.compare("") == 0)
		{
			build_options_str = "-cl-fast-relaxed-math -cl-mad-enable -cl-no-signed-zeros -cl-unsafe-math-optimizations -cl-finite-math-only -cl-std=CL2.0 -s " + kernel_file_path + kernel_file_name;
			//build_options_str = "-cl-uniform-work-group-size -cl-fast-relaxed-math -cl-mad-enable -cl-no-signed-zeros -cl-unsafe-math-optimizations -cl-finite-math-only -cl-std=CL2.0 -s " + kernel_file_path + kernel_file_name;
		} else {
			build_options_str = "-cl-fast-relaxed-math -cl-mad-enable -cl-no-signed-zeros -cl-unsafe-math-optimizations -cl-finite-math-only -cl-std=CL2.0 " + includes + " -s " + kernel_file_path + kernel_file_name;
			//build_options_str = "-cl-uniform-work-group-size -cl-fast-relaxed-math -cl-mad-enable -cl-no-signed-zeros -cl-unsafe-math-optimizations -cl-finite-math-only -cl-std=CL2.0 " + includes + " -s " + kernel_file_path + kernel_file_name;
		}
		const char *build_options = build_options_str.c_str();

		printf("Build Options: %s\n",build_options);

		// Upload the OpenCL C source code from the input file to source
		// The size of the C program is returned in sourceSize
		char* source = NULL;
		size_t src_size = 0;
		err = ReadSourceFromFile(kernel_file_name.c_str(), &source, &src_size);
		if (CL_SUCCESS != err)
		{
				fprintf(stderr,"Error: ReadSourceFromFile returned %s.\n", TranslateOpenCLError(err));
				if (source)
				{
						delete[] source;
						source = NULL;
				}
				return err;
		}

		// And now after you obtained a regular C string call clCreateProgramWithSource to create OpenCL program object.
		ocl->program = clCreateProgramWithSource(ocl->context, 1, (const char**)&source, &src_size, &err);
		if (CL_SUCCESS != err)
		{
				fprintf(stderr,"Error: clCreateProgramWithSource returned %s.\n", TranslateOpenCLError(err));
				if (source)
				{
						delete[] source;
						source = NULL;
				}
				return err;
		}

		// Build the program
		// During creation a program is not built. You need to explicitly call build function.
		// Here you just use create-build sequence,
		err = clBuildProgram(ocl->program, 1, &ocl->device, build_options, NULL, NULL);

		if (CL_SUCCESS != err)
		{
				fprintf(stderr,"Error: clBuildProgram() for source program returned %s.\n", TranslateOpenCLError(err));

				// In case of error print the build log to the standard output
				// First check the size of the log
				// Then allocate the memory and obtain the log from the program
				if (err == CL_BUILD_PROGRAM_FAILURE)
				{
						size_t log_size = 0;
						clGetProgramBuildInfo(ocl->program, ocl->device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

						std::vector<char> build_log(log_size);
						clGetProgramBuildInfo(ocl->program, ocl->device, CL_PROGRAM_BUILD_LOG, log_size, &build_log[0], NULL);

						fprintf(stderr,"Error happened during the build of OpenCL program.\nBuild log:%s", &build_log[0]);
				}
		}
		return err;
}
