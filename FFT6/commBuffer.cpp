/*
 *
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */
#include "commBuffer.h"

//#include <atomic>

#define MIN(a,b) ((a)<(b)?(a):(b))

//OpenCL Context
extern ocl_args_d_t *ocl;

static int log2_int(int n) {
		register int i, aux;

		aux = 1;
		for (i = 0; i <= 128; i++) {
				if (aux > n)
					 return (i - 1);
				aux <<= 1;
		}
  return -1;
}

CL_Buffers_t::CL_Buffers_t(int N, int nThreads,int M) :
N(N),
nThreads(nThreads),
gws(M * NUM_GROUPS * 7),
lws(gws/NUM_GROUPS),
hwBlocks(NUM_GROUPS/nThreads),
blocks((int)ceil((float)N/(float)lws)),
group_residency(ceil((float)blocks*nThreads/(float)NUM_GROUPS)),
last_issued(group_residency*NUM_GROUPS),
iter(0),
pFinish(1),
pDone(nThreads),
pBlocks(group_residency*NUM_GROUPS),
pThreads(group_residency*NUM_GROUPS),
//Benchmark Specific
Ndv2(N/2),
LOGN(log2_int(N)),
pIjDiff(nThreads),
pPoW(nThreads),
pFirst(nThreads),
pStride(nThreads),
pOuter_i(nThreads),
pInner_n(nThreads),
w(N/2),
a_flat(N*N)
{
	LogInfo("N = %d | nG = %d | gws = %d | lws = %d | hwBlocks = %d | blocks = %d | nthreads = %d | gRes %d\n", N,NUM_GROUPS, gws, lws, hwBlocks, blocks, nThreads, group_residency);

	xin = (complex *)calloc(N*N, sizeof(complex));
	if(xin  == NULL )
	{
		printf("ERROR: xin was NULL\n");
		exit(1);
	}
	aux = (complex *)calloc(N*N, sizeof(complex));
	if(aux  == NULL )
	{
		printf("ERROR: aux was NULL\n");
		exit(1);
	}
	brt = (int *)calloc(N, sizeof(int));
	if( brt == NULL )
	{
		printf("ERROR: brt was NULL\n");
		exit(1);
	}
	v = (complex *)calloc(N * N, sizeof(complex)); /* twiddles for 2d fft */
	if( v == NULL )
	{
		printf("ERROR: v was NULL\n");
		exit(1);
	}
	//w = (complex *)calloc((N / 2), sizeof(complex)); /* twiddles for 1d fft's */

}

CL_Buffers_t::~CL_Buffers_t()
{
}

void CL_Buffers_t::GPU_Set_Kernel_Args()
{
	int kernel_id = 0;
	cl_kernel kernel = ocl->kernels[kernel_id];
	int arg = 0;

	//Make sure in same order as kernel in .cl fie
	CHECK_ERRORS(clSetKernelArg(kernel, arg++, sizeof(int), &N));	//Its a constant
	CHECK_ERRORS(clSetKernelArg(kernel, arg++, sizeof(int), &nThreads));	//Its a constant
	pFinish.bind(kernel,arg++);
	pDone.bind(kernel,arg++);
	pBlocks.bind(kernel,arg++);
	pThreads.bind(kernel,arg++);
	//Benchmark Specific
	pIjDiff.bind(kernel,arg++);
	pPoW.bind(kernel,arg++);
	pFirst.bind(kernel,arg++);
	pStride.bind(kernel,arg++);
	pOuter_i.bind(kernel,arg++);
	pInner_n.bind(kernel,arg++);
	w.bind(kernel,arg++);
	a_flat.bind(kernel,arg++);

	
}

void CL_Buffers_t::GPU_Launch_Kernel()
{

	size_t globalWorkSize[1] = {gws};
	size_t localWorkGroups[1] = {lws};
	size_t *globalWorkOffset = NULL;

	pFinish[0] = 0;

	for(int i = 0; i < nThreads; i++)
	{
		pDone[i] = 0;
	}
	for(int i = 0; i < group_residency*NUM_GROUPS; i++)
	{
		pBlocks[i] = blocks;
	}


	CHECK_ERRORS(clEnqueueNDRangeKernel(ocl->commandQueues[0], ocl->kernels[0], 1, globalWorkOffset, globalWorkSize, localWorkGroups, 0, NULL,NULL));

	CHECK_ERRORS(clFlush(ocl->commandQueues[0]));

}

int CL_Buffers_t::GPU_Schedule(int tid, int kernel_nB, int kernel_id)
{

	int gR = group_residency;
	int nB = blocks;
	const int nG = NUM_GROUPS;
	int ret_iter = -1;

	int j;
	int val;
	int idx;

	//printf("tid %d scheduling kernel %d with nB %d | blocks %d\n",tid,kernel_id,kernel_nB, blocks);
	pthread_mutex_lock(launch_lock);

	for(int i = 0; i < kernel_nB; i++)
	{
		//For every block in kernel push onto block array

		j = last_issued%(gR*nG);
		while(j != last_issued-1)
		{
			//LogInfo("%d = Group %d | Slot %d\n",j, j%nG,j/nG);
			val = pBlocks[j];
			if(val == nB  || val == -1*nB)
			{
				//LogInfo("\ttid %d | Scheduled iter %d | Block %d | Group %d | Slot %d\n",tid,iter,i,j%nG,j/nG);

				//pBlocks is the trigger, so all parameters need to be set before it 
				pThreads[j] = tid;
				pBlocks[j] = i;
				last_issued = j+1;

				break;
			}
			j = (j+1)%(gR*nG);
		}
	}
	ret_iter = iter;
	iter = iter+1;
	pthread_mutex_unlock(launch_lock);
	return ret_iter;
}

//FFT6 has non-uniform block size so we need to pass in the number of blocks to wait for
void CL_Buffers_t::GPU_Sync(int tid, int iter, int wait_blocks, int kernel_id)
{

	unsigned long int count = 0;
	int done = pDone[tid];

	//LogInfo("Waiting for | tid = %d | iter = %d | blocks = %d | kernel = %d\n",tid,iter, wait_blocks, kernel_id);

	while(done != wait_blocks)
	{
		done = pDone[tid];
	#if 1 
		count++;
		if(count%10000000000==0)
		{
			LogInfo("Still waiting for | Tid = %d | iter = %d | Count = %lu | Done = %d\n",tid,iter,count, done);
			if(tid == 0)
			{
				print_schedule_H();
			}

		}
		if(count >= 3000000000)
		{
			LogError("\nTid = %d | ============================================Stuck in Infinite loop, done = %d / %d | iter = %d\n",tid,done,wait_blocks,iter);
			if(tid == 0)
			{
				print_schedule_H();
			}
			KillCommKernel();
			//done = pDone[iter];
			done = pDone[tid];
			LogInfo("Tid = %d | Done = %d\n",tid, done);
			if(tid == 0)
			{
				print_schedule_H();
				exit(0);
			} else {
				pthread_exit(0);
			}
		}
		#endif
	}
	pDone[tid] = 0;
	//LogInfo("Tid %d | iter %d | done waiting for done  = %d | count %d\n",tid,iter,done,count);
}

void CL_Buffers_t::KillCommKernel()
{
	LogInfo("Killing Comm Kernels\n");
	pFinish[0] = 1;
	
	CHECK_ERRORS(clFinish(ocl->commandQueues[0]));

	LogInfo("Done Killing Comm Kernels\n");
}


void CL_Buffers_t::print_schedule()
{
	int gR = group_residency;
	int nB = blocks;
	const int nG = NUM_GROUPS;

	int block;
	int iter;
	int idx;

	for(int i = 0; i < nG; i++)
	{
		LogInfo("%d [ ",i);
		for(int j = 0; j < gR; j++)
		{
			idx = i*gR+j;
			block = pBlocks[idx];
			LogInfo("(%4d,%2d) ",iter,block);
		}
		LogInfo("]\n");
	}
}


void CL_Buffers_t::print_schedule_H()
{
	int gR = group_residency;
	const int nG = NUM_GROUPS;

	int block;
	int tid;
	int kern;
	int idx;

	LogInfo("Iters\n");
	for(int i = 0; i < gR; i++)
	{
		LogInfo("%d [ ",i);
		for(int j = 0; j < nG; j++)
		{
			idx = i*nG+j;
			tid  = pThreads[idx];
			LogInfo("%6d ",iter);
		}
		LogInfo("]\n");
	}
	LogInfo("Blocks\n");
	for(int i = 0; i < gR; i++)
	{
		LogInfo("%d [ ",i);
		for(int j = 0; j < nG; j++)
		{
			idx = i*nG+j;
			block = pBlocks[idx];
			LogInfo("%6d ",block);
		}
		LogInfo("]\n");
	}

}

bool CL_Buffers_t::Check(char * filename)
{

	float re;
	float im;

	FILE * fp;

	fp = fopen(filename,"w");
	if(fp == NULL)
	{
		printf("Couldnt open file %s\n",filename);
		exit(-1);
	}

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			re = a_flat[i*N+j].re;
			im = a_flat[i*N+j].im;

			//if(re != 0.0 || im != 0.0)
			fprintf(fp,"[%d][%d] ( %.10f , %.10f )\n",i,j,re,im);
		}	
	}

	fclose(fp);
}
