/*
 * This code is a heavily edited version of an example OpenCL program provided by Intel.
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */
#pragma once

#include <ctime>
#include <stdarg.h>
#include <sys/time.h>

#include "CL/cl.h"

#define N_TIMERS 28 

// Read OpenCL source code from fileName and store it in source. The number of read bytes returns in sourceSize
int ReadSourceFromFile(const char* fileName, char** source, size_t* sourceSize);

//Timing Utils
void roi_enter();
void roi_exit();
void print_roi_time();

void start_timer(int i);
void accumulate_timer(int i);
void start_timer(int tid, int T, int i);
void accumulate_timer(int tid, int T, int i);
void print_timer(int i);
void print_timers(int nTimers = N_TIMERS);
