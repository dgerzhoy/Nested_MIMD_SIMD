/*****************************************************************************
 * Copyright (c) 2013-2016 Intel Corporation
 * All rights reserved.
 *
 * WARRANTY DISCLAIMER
 *
 * THESE MATERIALS ARE PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR ITS
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THESE
 * MATERIALS, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Intel Corporation is the author of the Materials, and requests that all
 * problem reports or change requests be submitted to it directly
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
//#include <tchar.h>
#include <memory.h>
//#include <windows.h>
#include "CL/cl.h"
#include "CL/cl_ext.h"
#include "utils.h"
#include <assert.h>


//we want to use POSIX functions
// #pragma warning( push )
// #pragma warning( disable : 4996 )


void LogInfo(const char* str, ...)
{
    if (str)
    {
        va_list args;
        va_start(args, str);

        vfprintf(stdout, str, args);

        va_end(args);
    }
    fflush(stdout);
}

void LogError(const char* str, ...)
{
    if (str)
    {
        va_list args;
        va_start(args, str);

        vfprintf(stderr, str, args);

        va_end(args);
    }
    fflush(stderr);
}

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
        LogError("Error: Couldn't find program source file '%s'.\n", fileName);
        errorCode = CL_INVALID_VALUE;
    }
    else {
        fseek(fp, 0, SEEK_END);
        *sourceSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        *source = new char[*sourceSize];
        if (*source == NULL)
        {
            LogError("Error: Couldn't allocate %d bytes for program source from file '%s'.\n", *sourceSize, fileName);
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

	LogInfo("Time Total: %f s\n",roi_time);
}

void print_roi_time()
{
	LogInfo("Time Total: %f s\n",roi_time);
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

	//LogInfo("Timer %d: %f\n",i,dTimer[i]);
	LogInfo("Timer %d: %f ( %d iters)\n",i,dTimer[i],count[i]);

}

void print_timers(int nTimers)
{
	printf("\n");
	for(int i = 0; i < nTimers; i++)
	{
		print_timer(i);
	}

}

#pragma warning( pop )
