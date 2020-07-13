/*
 * Kernel and Launch Mechanism for FFT6
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */
#include "fft6.h"

void TranslateOpenCLError(int errorCode)
{
		switch(errorCode)
		{
		case CLK_SUCCESS:                         printf("CLK_SUCCESS\n"); break;
    case CLK_INVALID_QUEUE :                  printf("CLK_INVALID_QUEUE\n"); break;
    case CLK_INVALID_NDRANGE :                printf("CLK_INVALID_NDRANGE\n"); break;
    case CLK_INVALID_EVENT_WAIT_LIST :        printf("CLK_INVALID_EVENT_WAIT_LIST\n"); break;
    case CLK_DEVICE_QUEUE_FULL :              printf("CLK_DEVICE_QUEUE_FULL\n"); break;
    case CLK_INVALID_ARG_SIZE :               printf("CLK_INVALID_ARG_SIZE\n"); break;
    case CLK_EVENT_ALLOCATION_FAILURE :       printf("CLK_EVENT_ALLOCATION_FAILURE\n"); break;
		case CLK_OUT_OF_RESOURCES :               printf("CLK_OUT_OF_RESOURCES\n"); break;
    default :                                 printf("UNKNOWN\n"); break;
    }
}

void aIncrement(__global int *pAtom, int index)
{
	__global atomic_int *aAtom = (__global volatile atomic_int *)pAtom;

	atomic_fetch_add_explicit(&aAtom[index], 1, memory_order_seq_cst, memory_scope_all_svm_devices );
}

void aAdd(__global int *pAtom, int index, int val)
{
	__global atomic_int *aAtom = (__global volatile atomic_int *)pAtom;

	atomic_fetch_add_explicit(&aAtom[index], val, memory_order_seq_cst, memory_scope_all_svm_devices );
}

void aSubtract(__global int *pAtom, int index, int val)
{
	__global atomic_int *aAtom = (__global volatile atomic_int *)pAtom;

	atomic_fetch_sub_explicit(&aAtom[index], val, memory_order_seq_cst, memory_scope_all_svm_devices );
}

void aStore(__global int *pAtom, int index, int val)
{
	__global atomic_int *aAtom = (__global volatile atomic_int *)pAtom;

	atomic_store_explicit(&aAtom[index], val, memory_order_seq_cst, memory_scope_all_svm_devices );
}

void afStore(__global float *pAtom, int index, float val)
{
	__global atomic_float *aAtom = (__global volatile atomic_float *)pAtom;

	atomic_store_explicit(&aAtom[index], val, memory_order_seq_cst, memory_scope_all_svm_devices );
}

int aLoad(__global int *pAtom, int index)
{
	__global atomic_int *aAtom = (__global volatile atomic_int *)pAtom;

	//int ret;
	return atomic_load_explicit(&aAtom[index], memory_order_seq_cst, memory_scope_all_svm_devices );

}

void aMark(__global int *pAtom, int index, int lid, int val)
{
	if(lid!=0)
		return;

	#if 1

		pAtom[index] = val;

	#else
		__global atomic_int *aAtom = (__global volatile atomic_int *)pAtom;

		atomic_store_explicit(&aAtom[index], val, memory_order_seq_cst, memory_scope_all_svm_devices );
	#endif
}

#if 0
int aExchange(__global int *pAtom, int index, int val)
{
	__global atomic_int *aAtom = (__global volatile atomic_int *)pAtom;

	return atomic_exchange_explicit(&aAtom[index], val, memory_order_seq_cst, memory_scope_all_svm_devices );
}
#endif

//#define NULL_KERN
//#define LAUNCH_DBG

__kernel void Launch_Daemon_FFT6(
		int N,
		int T,
		__global int * pFinish,
		__global int * pDone,
		__global int * pBlocks,
		__global int * pThreads,
		//Benchmark Specific
		__global int * pIjDiff,
		__global int * pPoW,
		__global int * pFirst,
		__global int * pStride,
		__global int * pOuter_i,
		__global int * pInner_n,
		__global complex * w,
		__global complex * p_a
		)
{
	//Built in constants
	const __private int gws = get_global_size(0);
	const __private int lws = get_local_size(0);
	const __private int gid = get_global_id(0);
	const __private int lid = get_local_id(0);

	const __private int nG = get_num_groups(0);
	const __private int group = get_group_id(0);

	#ifdef LAUNCH_DBG
		if( lid == 0 )
			printf("Hello from group %d\n",group);
	#endif

	//Flex Constants
	const __private int nB = ceil((float)N/(float)lws);
	const __private int TnB = nB * T;
	const __private int gRes = ceil((float)TnB/(float)nG);

	local int block_id; //Block that the group is currently on
	local int iter;			//Iteration that the group is currently on
	local int thread;			//Thread that the group is currently on
  	local int kern;			//Thread that the group is currently on
	local int g_iter;
	local int idx;
	local int go;
	local int done;
	local int lFinish;

	long int flex_gid;

	int i,j,k;

	//Kernel Parameters
	local int ijDiff,PoW,first,stride,outer_i;
	local complex pw;
	local int a_idx;
	//Variables for the algorithm
	private complex ii,jj,temp;
	local int inner_n;
	
	complex * a;


	//Initialize
	idx = lFinish = go = g_iter = 0;
  	i= j = k = 0;

	work_group_barrier( CLK_LOCAL_MEM_FENCE );

	//printf("long %d | long long %d\n",sizeof(long),sizeof(long long));
#if 0 
	if(gid == 0)
	{
			printf("nB %d | TnB %d | gRes %d \n",\
			nB,
			TnB,
			gRes
			);

	}
	return;
#endif
	while(1)
	{
		go = 0;
		work_group_barrier( CLK_LOCAL_MEM_FENCE );
		//Read Variables for this iteration
		if(lid == 0)
		{
			lFinish = pFinish[0];

			idx = g_iter * nG + group;
			
			block_id = pBlocks[idx];
			if(block_id < nB && block_id >= 0) //Found Valid Block
			{
				thread = pThreads[idx]; //Launching Thread used to read in args and signal block is done.

        		//read in bench args /initialize stuff
				ijDiff = pIjDiff[thread];
				PoW = pPoW[thread];
				first = pFirst[thread];
				stride = pStride[thread];
				outer_i = pOuter_i[thread];
				inner_n = pInner_n[thread];
				pw = w[PoW];

        		go = 1;
			} 
        	//Increment and roll
        	g_iter = (g_iter+1)%gRes;
		}
		work_group_barrier( CLK_LOCAL_MEM_FENCE );

		if( lFinish != 0)
		{
		#ifdef LAUNCH_DBG
			if( lid == 0 )
			{
					printf("GPU Received Finish Command | lsize %d | gsize %d | nthreads %d\n", lws, gws,T);

			}
		#endif
			return;
		} //End Finish Check

		if(go)
		{
			//Index
			flex_gid = lws*block_id + lid;
			i = flex_gid*stride + first; 

			//Double pointer access
			a = &p_a[outer_i*N];

	/*
	*	Work Payload
	*		Can Be disabled by defining NULL_KERN
	*		Useful for measuring Launch Overhead
	*/
	#ifndef NULL_KERN
			if(i < inner_n)
			{
				j = i + ijDiff;
				jj = (complex)a[j];
 			 	ii = (complex)a[i];
				temp.re = (float)(jj.re * pw.re - jj.im * pw.im);
				temp.im = (float)(jj.re * pw.im + jj.im * pw.re);
				a[j].re = (float)(ii.re - temp.re);
				a[j].im = (float)(ii.im - temp.im);
				a[i].re = (float)(ii.re + temp.re);
				a[i].im = (float)(ii.im + temp.im);
			} //End flex_gid < N 
      		
	#endif		//Done Work Payload

			work_group_barrier( CLK_LOCAL_MEM_FENCE );
      		
			if(lid == 0)
			{
				//printf("Executing block %d | iter %d\n",block_id,iter);
				pBlocks[idx]=nB; //Reset Slot
				aIncrement(pDone,thread);
			}
		}//End Go Check
	} //End While

}

