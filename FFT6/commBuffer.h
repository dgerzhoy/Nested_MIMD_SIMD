/*
 *
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */

#include <cassert>
#include <functional>
#include <math.h>
#include <queue>
#include <string>
#include <vector>
#include <unistd.h>

#include "clBuffer.h"
#include "clSetup.h"
#include "fft6.h"
#include "utils.h"

#define NUM_GROUPS 24 //Number of Execution units on the GPU

enum kType
{
	CPU, 	//0 CPU (Default)
	GPU,	//1 GPU Launch Daemon
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
	//public:
	CL_Buffers_t::CL_Buffers_t(int N, int nThreads,int M);
	~CL_Buffers_t();

	int N;
	int nThreads;

	size_t WG_Size_Multiple; //CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE
	size_t gws;
	size_t lws;
	size_t hwBlocks;
	size_t blocks;
	size_t group_residency; //gRes

	int last_issued;

	pthread_mutex_t * launch_lock;

	//Control Buffers
	cl_SVM_Buffer<int> pFinish;
	cl_SVM_Buffer<int> pDone;
	cl_SVM_Buffer<int> pBlocks;

	//Sync Array
	cl_SVM_Buffer<int> pThreads;

	//Parameter Buffers
	cl_SVM_Buffer<int> pIjDiff;
	cl_SVM_Buffer<int> pPoW;
	cl_SVM_Buffer<int> pFirst;
	cl_SVM_Buffer<int> pStride;
	cl_SVM_Buffer<int> pOuter_i;
	cl_SVM_Buffer<int> pInner_n;
	cl_SVM_Buffer<complex> w;
	cl_SVM_Buffer<complex> a_flat;

	//Keep Track of What iteration we are on
	int iter;

	//Not cl_SVM_Buffer because not touched by GPU
	int nd;
	int iters;
	int Ndv2;
	int LOGN;
	complex * xin;
	complex * aux;
	int * brt;
	complex * v;
	int cutoff;

	//Binds Arguments to Kernel
	void GPU_Set_Kernel_Args();

	//Launches Daemon to GPU
	void GPU_Launch_Kernel();

	//Schedules a kernel instance to the daemon
	int GPU_Schedule(int tid, int kernel_nB, int kernel_id);
	
	//Blocking Wait for a particular kernel instance to finish
	void GPU_Sync(int tid, int iter, int wait_blocks, int kernel_id);

	//Sends kill command to daemon, and waits for it to finish
	void KillCommKernel();
	
	//Print the current scheduling table in the daemon
	void print_schedule();
	void print_schedule_H();
	
	//BenchMark Functions
	void CPU_Core(core_args_t * args);
	void GPU_Core(core_args_t * args);
	void CPU_Main(main_args_t * args);
	void GPU_Main(main_args_t * args);

	bool Check(char * filename);
};

