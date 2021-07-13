/*
 * This code is a heavily edited version of an example OpenCL program provided by Intel.
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */

#include "utils.h"

#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "CL/cl.h"
#include "CL/cl_ext.h"


// Upload the OpenCL C source code to output argument source
// The memory resource is implicitly allocated in the function
// and should be deallocated by the caller
int ReadSourceFromFile(const char* fileName, char** source, size_t* sourceSize)
{
    int errorCode = CL_SUCCESS;

    FILE* fp = NULL;
    fp = fopen(fileName, "rb");
    if (fp == NULL)
    {
        fprintf(stderr,"Error: Couldn't find program source file '%s'.\n", fileName);
        errorCode = CL_INVALID_VALUE;
    }
    else {
        fseek(fp, 0, SEEK_END);
        *sourceSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        *source = new char[*sourceSize];
        if (*source == NULL)
        {
            fprintf(stderr,"Error: Couldn't allocate %d bytes for program source from file '%s'.\n", *sourceSize, fileName);
            errorCode = CL_OUT_OF_HOST_MEMORY;
        }
        else {
            fread(*source, 1, *sourceSize, fp);//
		}
    }
    return errorCode;
}

//Timing Utils
double roi_time;
double dTimer[N_TIMERS] = {};
int count[N_TIMERS] =  {0};

struct timespec begin;
struct timespec end;
struct timespec pBegin[N_TIMERS];
struct timespec pEnd[N_TIMERS];

//#define CLOCK CLOCK_PROCESS_CPUTIME_ID
//#define CLOCK CLOCK_THREAD_CPUTIME_ID
//#define CLOCK CLOCK_REALTIME
#define CLOCK CLOCK_MONOTONIC 

void roi_enter()
{
	clock_gettime(CLOCK,&begin);
}

void roi_exit()
{
	clock_gettime(CLOCK,&end);
	roi_time = (double)(end.tv_sec - begin.tv_sec) + ((double)(end.tv_nsec - begin.tv_nsec)) / 1000000000.0;

	printf("Time Total: %f s\n",roi_time);
}

void print_roi_time()
{
	printf("Time Total: %f s\n",roi_time);
}

void start_timer(int i)
{
	clock_gettime(CLOCK,&pBegin[i]);
}

void start_timer(int tid, int T, int i)
{
	start_timer(tid*T+i);
}

void accumulate_timer(int i)
{
	clock_gettime(CLOCK,&pEnd[i]);

	double val = (double)(pEnd[i].tv_sec - pBegin[i].tv_sec) + ((double)(pEnd[i].tv_nsec - pBegin[i].tv_nsec)) / 1000000000.0;

	dTimer[i]+= val;
	
	//printf("Timer [%d][%d] val %f | total %f\n",i,count[i],val,dTimer[i]);
	count[i]++;
}

void accumulate_timer(int tid, int T, int i)
{
	accumulate_timer(tid*T+i);
}


void print_timer(int i)
{

	//printf("Timer %d: %f\n",i,dTimer[i]);
    if (dTimer[i] > 0.0)
    {
	    printf("Timer %d: %f ( %d iters)\n",i,dTimer[i],count[i]);
    }
}

void print_timers(int nTimers)
{
	printf("\n");
	for(int i = 0; i < nTimers; i++)
	{
		print_timer(i);
	}

}