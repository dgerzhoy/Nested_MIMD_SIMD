/*
 *
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */


#pragma once

#include <cassert>
#include <string>
#include <math.h>
#include "clBuffer.h"
#include <unistd.h>
#include <queue>
#include "clSetup.h"
#include "utils.h"
#include <functional>
#include <vector>
#include "MD.h"

#define NUM_GROUPS 24

enum kType
{
	CPU, 	//0 CPU (Default)
	GPU		//1 GPU Launch Daemon
};

//Defined in Benchmark
typedef struct main_args_t;
typedef struct core_args_t;
typedef struct kernel_core_args_t;
void *CPU_Slave(void * arguments);
void *GPU_Slave(void * arguments);

//Structure of atomic variable pointers to share data between the GPU and CPU
//Main reason is to provide synchronization between them
//Comm Buffer will be application specific, though most applications will have the core buffers.
struct CL_Buffers_t
{
	CL_Buffers_t(int N, int nThreads,int M);
	~CL_Buffers_t(){};

	int N; //np
	int nThreads;
	int nd;

	size_t WG_Size_Multiple; //CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE
	size_t gws;
	size_t lws;
	size_t hwBlocks;
	size_t blocks;
	size_t group_residency; //gRes

	int last_issued;

	pthread_mutex_t * launch_lock;

	cl_SVM_Buffer<unsigned int> pFinish;
	cl_SVM_Buffer<unsigned int> pDone;
	cl_SVM_Buffer<unsigned int> pBlocks;

	//Sync Array
	cl_SVM_Buffer<unsigned int> pIters;

	//Parameter Buffers
	cl_SVM_Buffer<cl_float3> pos;
	cl_SVM_Buffer<cl_float3> vel;
	cl_SVM_Buffer<cl_float3> force;
	cl_SVM_Buffer<cl_float3> accel;
	cl_SVM_Buffer<float> pPot;

	//Not cl_SVM_Buffer because not touched by GPU
	float pot;
	float kin;
	float mass;

	//Binds Arguments to Kernel
	void GPU_Set_Kernel_Args();

	//Launches Daemon to GPU
	void GPU_Launch_Kernel();

	//Schedules a kernel instance to the daemon
	void GPU_Schedule(int tid, int iter);

	//Blocking Wait for a particular kernel instance to finish
	void GPU_Sync(int tid, int iter);

	//Sends kill command to daemon, and waits for it to finish
	cl_uint KillCommKernel();

	//Print the current scheduling table in the daemon
	void print_schedule();
	void print_schedule_H();
};

