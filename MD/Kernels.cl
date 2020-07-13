/*
 *
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */
#include "MD.h"

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

void aIncrement(__global uint *pAtom, int index)
{
	__global atomic_int *aAtom = (__global volatile atomic_int *)pAtom;

	atomic_fetch_add_explicit(&aAtom[index], 1, memory_order_seq_cst, memory_scope_all_svm_devices );
}

void aStore(__global uint *pAtom, int index, uint val)
{
	__global atomic_int *aAtom = (__global volatile atomic_int *)pAtom;

	atomic_store_explicit(&aAtom[index], val, memory_order_seq_cst, memory_scope_all_svm_devices );
}

void afStore(__global float *pAtom, int index, float val)
{
	__global atomic_float *aAtom = (__global volatile atomic_float *)pAtom;

	atomic_store_explicit(&aAtom[index], val, memory_order_seq_cst, memory_scope_all_svm_devices );
}

uint aLoad(__global uint *pAtom, int index)
{
	__global atomic_int *aAtom = (__global volatile atomic_int *)pAtom;

	//uint ret;
	return atomic_load_explicit(&aAtom[index], memory_order_seq_cst, memory_scope_all_svm_devices );

}
uint aExchange(__global uint *pAtom, int index, uint val)
{
	__global atomic_int *aAtom = (__global volatile atomic_int *)pAtom;

	return atomic_exchange_explicit(&aAtom[index], val, memory_order_seq_cst, memory_scope_all_svm_devices );
}

float v(float x)
{
	if (x < M_PI_2)
	    return pown(sin(x), 2.0);
	  else
	    return 1.0;

}

float dv(float x) {
  if (x < M_PI_2)
{
	float sinval;
	float cosval;
	sinval = sincos(x,&cosval);
	return 2.0 * sinval * cosval;
    //return 2.0 * sin(x) * cos(x);
}
  else
    return 0.0;
}

float dist(float3 r1, float3 r2, float3 * pdr)
{
	float d;
	float3 dr;

	dr = r1 - r2;
	*pdr = dr;
	d = dr.x*dr.x + dr.y*dr.y + dr.z*dr.z;
	d = sqrt(d);

	return d;
}

//#define LAUNCH_DBG

__kernel void Launch_Daemon_MD(
		uint N,
		uint T,
		__global uint * pFinish,
		__global uint * pDone,
		__global uint * pBlocks,
		__global uint * pIters,
	//Benchmark Specific
		__global float3 * f,
		__global float3 * pos,
		__global float * pot
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

	//Constants
	const __private int nB = ceil((float)N/(float)lws);
	const __private int TnB = nB * T;
	const __private int gRes = ceil((float)TnB/(float)nG);


	//Local Variables used to control each block
	local int block_id; //Block that the group is currently on
	local int iter;			//Iteration that the group is currently on
	local int thread;
	//local int kern;
	local int g_iter;
	local int idx;
	local int go;
	local int done;
	local int lFinish;

	long int daemon_gid;
	
	int r; 		//Reduction Array Index
	float V; 	//Reduction variable
	local float lPot; //Reduction result per block

	float d = 0.0f;
	float3 rij;

	idx = lFinish = go = g_iter = 0;

	work_group_barrier( CLK_LOCAL_MEM_FENCE );

	while(1)
	{
		go = 0;
		work_group_barrier( CLK_LOCAL_MEM_FENCE );
		//Read Parameters for this iteration
		if(lid == 0)
		{
			lFinish = pFinish[0];

			idx = g_iter*nG+group;
			
			block_id = pBlocks[idx];
			if(block_id < nB) //Found Valid Block
			{
				iter = pIters[idx]; //Launching Iteration used to read in args and signal block is done.

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
			daemon_gid = lws*block_id + lid;

	/*
	*	Work Payload
	*		Can Be disabled by defining NULL_KERN
	*		Useful for measuring Launch Overhead
	*/
	#ifndef NULL_KERN

			if(daemon_gid < N && daemon_gid != iter)
			{
				d = dist(pos[iter],pos[daemon_gid],&rij);
				
				f[daemon_gid] -= rij*dv(d)/d;
				
				V = 0.5 * v(d);
			} else
			{
				V = 0;
			}

			lPot = work_group_reduce_add(V);

			if(lid == 0)
			{
				//calculate reduction array block id
				r = iter*nG + block_id;
				pot[r] = lPot;
			}

	#endif

			work_group_barrier( CLK_LOCAL_MEM_FENCE );
			if(lid == 0)
			{
				//printf("Executing block %d | iter %d\n",block_id,iter);
				pBlocks[idx]=nB; //Reset slot
				aIncrement(pDone,iter);
			}

		}//End Go Check

	} //End While

}
