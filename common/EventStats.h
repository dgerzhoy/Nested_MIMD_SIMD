#pragma once

#include <stdio.h>
#include <stdlib.h>
//#include <tchar.h>
#include <memory.h>
#include <vector>

#include <CL/cl.h>
#include "utils.h"

//for perf. counters
//#include <Windows.h>

class eventStats
{
public:
	eventStats();
	~eventStats();

	void populate();
	void insertValues(cl_ulong queued_N, cl_ulong submit_N, cl_ulong start_N, cl_ulong end_N);
	void recalcDiffs();
	void printStats(char *label = "");
	void printAvgs();
	void printAvgsCSV(FILE * out);


	cl_event event;

	cl_ulong queued;
	cl_ulong submit;
	cl_ulong start;
	cl_ulong end;
	cl_ulong complete;

	long diffQueued;
	long diffSubmit;
	long diffStart;
	long diffComplete;

	long diffTotal;

	long long queueSum;
	long long submitSum;
	long long startSum;
	long long totalSum;
	long long completeSum;

	double queueAvg;
	double submitAvg;
	double startAvg;
	double totalAvg;
	double completeAvg;

	cl_int iterations;

	double queuePercentage;
	double submitPercentage;
	double workPercentage;

};
