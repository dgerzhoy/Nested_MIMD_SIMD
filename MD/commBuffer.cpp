/*
 *
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */


#include "commBuffer.h"

CL_Buffers_t::CL_Buffers_t(int N, int nThreads,int M) :
N(N),
nThreads(nThreads),
nd(NDIM),
WG_Size_Multiple(M),
gws(M * NUM_GROUPS * 7),
lws(gws/NUM_GROUPS),
hwBlocks(NUM_GROUPS/nThreads),
blocks((int)ceil((float)N/(float)lws)),
group_residency(ceil((float)blocks*nThreads/(float)NUM_GROUPS)),
last_issued(group_residency*NUM_GROUPS),
pFinish(N),
pDone(N),
pBlocks(group_residency*NUM_GROUPS),
pIters(group_residency*NUM_GROUPS),
//Benchmark Specific
pos(N),
vel(N),
force(N),
accel(N),
pPot(NUM_GROUPS*N)
{
	LogInfo("nG = %d | gws = %d | lws = %d | hwBlocks = %d | blocks = %d | nthreads = %d | gRes %d\n", NUM_GROUPS, gws, lws, hwBlocks, blocks, nThreads, group_residency);
}

void CL_Buffers_t::GPU_Set_Kernel_Args()
{
	int kernel_id = 0;
	cl_kernel kernel = ocl->kernels[kernel_id];
	int arg = 0;

	//Make sure in same order as kernel in .cl fie
	CHECK_ERRORS(clSetKernelArg(kernel, arg++, sizeof(cl_uint), &N));	//Its a constant
	CHECK_ERRORS(clSetKernelArg(kernel, arg++, sizeof(cl_uint), &nThreads));	//Its a constant
	pFinish.bind(kernel,arg++);
	pDone.bind(kernel,arg++);
	pBlocks.bind(kernel,arg++);
	pIters.bind(kernel,arg++);
	force.bind(kernel, arg++);
	pos.bind(kernel, arg++);
	pPot.bind(kernel, arg++);

}

void CL_Buffers_t::GPU_Launch_Kernel()
{

	size_t globalWorkSize[1] = {gws};
	size_t localWorkGroups[1] = {lws};
	size_t *globalWorkOffset = NULL;

	for(int i = 0; i < N; i++)
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

void CL_Buffers_t::GPU_Schedule(int tid, int iter)
{
	int gR = group_residency;
	int nB = blocks;
	const int nG = NUM_GROUPS;

	int j;
	int val;
	int idx;

	for(int i = 0; i < nB; i++)
	{
		//For every block in kernel push onto block array

		//for(int j = 0; j < nG*gR; j++)
		pthread_mutex_lock(launch_lock);
		j = last_issued%(gR*nG);
		while(j != last_issued-1)
		{
				//printf("%d = Group %d | Slot %d\n",j, j%nG,j/nG);
				val = pBlocks[j];
				if(val == nB  || val == -1*nB)
				{
					//printf("tid %d | Scheduled iter %d | Block %d | Group %d | Slot %d\n",tid,iter,i,j%nG,j/nG);
					//pBlocks is the trigger, so all parameters need to be set before it 
					pIters[j] = iter;
					pBlocks[j] = i;
					last_issued = j+1;
					break;
				}
				j = (j+1)%(gR*nG);
		}
		pthread_mutex_unlock(launch_lock);

	}

}

void CL_Buffers_t::GPU_Sync(int tid, int iter)
{
	int count = 0;
	int done = pDone[iter];

	//printf("tid = %d | iter = %d | blocks = %d\n",tid,iter, blocks);

	while(done != blocks)
	{
		done = pDone[iter];
	#if 1
		count++;
		if(count%100000000==0)
		{
			printf("Tid = %d | Count = %d | Done = %d\n",tid,count, done);

		}
		if(count >= N*100000)
		{
			LogError("\nTid = %d | ============================================Stuck in Infinite loop, done = %d / %d | iter = %d\n",tid,done,blocks,iter);
			KillCommKernel();
			done = pDone[iter];
			printf("Tid = %d | Done = %d\n",tid, done);
			if(tid == 0)
			{
				//print_schedule_H();
				exit(0);
			} else {
				pthread_exit(0);
			}
		}
		#endif
	}
	pDone[iter] = 0;
	//printf("Tid %d | iter %d | done waiting for done  = %d\n",tid,iter,done);

}

cl_uint CL_Buffers_t::KillCommKernel()
{
	printf("Killing Comm Kernel\n");

	pFinish[0] = 1;
	CHECK_ERRORS(clFinish(ocl->commandQueues[0]));

	printf("Done Killing Comm Kernels\n");

	return CL_SUCCESS;

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
		printf("%d [ ",i);
		for(int j = 0; j < gR; j++)
		{
			idx = i*gR+j;
			block = pBlocks[idx];
			iter  = pIters[idx];
			printf("(%4d,%2d) ",iter,block);
		}
		printf("]\n");
	}
}

void CL_Buffers_t::print_schedule_H()
{
	int gR = group_residency;
	int nB = blocks;
	const int nG = NUM_GROUPS;

	int block;
	int iter;
	int idx;

	printf("Iters\n");
	for(int i = 0; i < gR; i++)
	{
		printf("%d [ ",i);
		for(int j = 0; j < nG; j++)
		{
			idx = i*nG+j;
			iter  = pIters[idx];
			printf("%4d ",iter);
		}
		printf("]\n");
	}
	printf("Blocks\n");
	for(int i = 0; i < gR; i++)
	{
		printf("%d [ ",i);
		for(int j = 0; j < nG; j++)
		{
			idx = i*nG+j;
			block = pBlocks[idx];
			printf("%2d ",block);
		}
		printf("]\n");
	}
}

