#pragma once

#include "CL/cl.h"
#include <stdarg.h>
//#include <d3d9.h>
//#include "EventStats.h"
#include <ctime>
#include <sys/time.h>

#define N_TIMERS 28 


// Print useful information to the default output. Same usage as with printf
void LogInfo(const char* str, ...);

// Print error notification to the default output. Same usage as with printf
void LogError(const char* str, ...);

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
