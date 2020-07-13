#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>
#include <omp.h>
#include <string>

//#include "EventStats.h"
#include <CL/cl.h>
#include "utils.h"

#pragma OPENCL EXTENSION cl_khr_icd : enable

#define CHECK_ERRORS(ERR)                        			\
    if(ERR != CL_SUCCESS)                               		\
    {                                                   		\
	fprintf(stderr,"OpenCL error %s happened in file %s at line %d.\n", TranslateOpenCLError(ERR) , __FILE__, __LINE__); 		\
	fflush(stderr);\
	exit(1); \
    }

// Macros for OpenCL versions
#define OPENCL_VERSION_1_2  1.2f
#define OPENCL_VERSION_2_0  2.0f

#define CL_MAP_READ_WRITE (CL_MAP_READ | CL_MAP_WRITE)

//Map Macro
#define CL_MAP_SVM(QUEUE,TF,RW,PTR,SIZE)	\
	CHECK_ERRORS(				\
		clEnqueueSVMMap(		\
			QUEUE,			\
			TF,			\
			RW,			\
			PTR,			\
			SIZE,			\
			0,0,0)			\
	)

//Unmap Macro
#define CL_UNMAP_SVM(QUEUE,PTR)			\
	CHECK_ERRORS(				\
		clEnqueueSVMUnmap(		\
			QUEUE,			\
			PTR,			\
			0,0,0)			\
	)

struct ocl_args_d_t
{
  	ocl_args_d_t(int nQueues, int nKernels);
  	~ocl_args_d_t();

  	// Regular OpenCL objects: (These must always be in the structure
  	cl_context       context;           // hold the context handler
  	cl_device_id     device;            // hold the selected device handler
	cl_program       program;           // hold the program handler
	float            platformVersion;   // hold the OpenCL platform version (default 1.2)
  	float            deviceVersion;     // hold the OpenCL device version (default. 1.2)
  	float            compilerVersion;   // hold the device OpenCL C version (default. 1.2)

	//Changed from the original example to support multiple Queues, and therefore multiple kernels
  	cl_command_queue *commandQueues;      // hold the commands-queue handler(s)
  	cl_command_queue deviceQueue;
	cl_kernel        *kernels;            // hold the kernel handler(s)
	//eventStats 		 *kernelStats;		//holds the event statistics for each kernel

	//Should be the same for now
	int nQueues;
	int nKernels;



	//From here down need to be added in a separate structure for every benchmark (pretty much only if not using svmAlloc
	//cl_mem are only if you are not using SVM_Alloc on your buffers (Not used for our purposes)
    //cl_mem           srcA;              // hold first source buffer
    //cl_mem           srcB;              // hold second source buffer
	//cl_mem           dstMem;            // hold destination buffer
};



const char* TranslateOpenCLError(cl_int errorCode);

cl_int printDeviceInfo(cl_device_id device);

cl_int printKernelInfo(cl_kernel kernel);

cl_int printPlatformInfo(cl_platform_id platform);

bool CheckPreferredPlatformMatch(cl_platform_id platform, const char* preferredPlatform);

cl_platform_id FindOpenCLPlatform(const char* preferredPlatform, cl_device_type deviceType);

int GetPlatformAndDeviceVersion(cl_platform_id platformId);

int SetupOpenCL(cl_device_type deviceType);

int CreateAndBuildProgram(std::string kernel_file_path, std::string kernel_file_name, std::string includes = "");
