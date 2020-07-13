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
#include "timers.h"


//Timing Utils
double roi_time;
double dTimer[10] = {};

struct timespec begin;
struct timespec end;
struct timespec pBegin[10];
struct timespec pEnd[10];

//#define CLOCK CLOCK_PROCESS_CPUTIME_ID
//#define CLOCK CLOCK_REALTIME
//#define CLOCK CLOCK_THREAD_CPUTIME_ID
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

void accumulate_timer(int i)
{
	clock_gettime(CLOCK,&pEnd[i]);

	dTimer[i]+= (double)(pEnd[i].tv_sec - pBegin[i].tv_sec) + ((double)(pEnd[i].tv_nsec - pBegin[i].tv_nsec)) / 1000000000.0;
}

void print_timer(int i)
{

	printf("Timer %d: %f\n",i,dTimer[i]);

}

void print_timers()
{
	printf("\n");
	for(int i = 0; i < 10; i++)
	{
		print_timer(i);
	}

}
