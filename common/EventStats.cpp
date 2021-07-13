#include "EventStats.h"


eventStats::eventStats()/*:

	event(NULL),
	queued(NULL),
	submit(NULL),
	start(NULL),
	end(NULL),
	diffQueued(NULL),
	diffSubmit(NULL),
	diffStart(NULL),
	diffTotal(NULL)*/
	{
	iterations = 0;
	queueSum = 0;
	submitSum = 0;
	startSum = 0;
	totalSum = 0;
	completeSum = 0;
	}

eventStats::~eventStats() {


}

void eventStats::populate() {

	if (CL_SUCCESS != clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued, NULL))
	{
		printf("Profiling has failed at queue");
		exit(1);
	}
	if (CL_SUCCESS != clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit, NULL))
	{
		printf("Profiling has failed at submit");
		exit(1);
	}
	if (CL_SUCCESS != clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL))
	{
		printf("Profiling has failed at start");
		exit(1);
	}
	if (CL_SUCCESS != clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL))
	{
		printf("Profiling has failed at end");
		exit(1);
	}
	if (CL_SUCCESS != clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_COMPLETE, sizeof(cl_ulong), &complete, NULL))
	{
		printf("Profiling has failed at end");
		exit(1);
	}

	recalcDiffs();

}

void eventStats::insertValues(cl_ulong queued_N, cl_ulong submit_N, cl_ulong start_N, cl_ulong end_N)
{

	queued = queued_N;
	submit = submit_N;
	start = start_N;
	end = end_N;

}

void eventStats::recalcDiffs()
{

	diffQueued = submit - queued;
	diffSubmit = start - submit;
	diffStart = end - start;
	diffTotal = end - queued;
	diffComplete = complete - end;


	queueSum += diffQueued;
	submitSum += diffSubmit;
	startSum += diffStart;
	totalSum += diffTotal;
	completeSum += diffComplete;

	iterations++;

	//queueAvg = (double)((double)(queueSum) / iterations);
	//submitAvg = (double)((double)(submitSum) / iterations);
	//startAvg = (double)((double)(startSum) / iterations);
	//totalAvg = (double)((double)(totalSum) / iterations);

	//queuePercentage = 100 * queueAvg / totalAvg;
	//submitPercentage = 100 * submitAvg / totalAvg;
	//workPercentage = 100 * startAvg / totalAvg;

}

void eventStats::printStats(char *label)
{
	//return;
	printf(label);
	printf("Event time: \n\t\
			queued: %lu \n\t\
			submit: %lu \n\t\
			start: %lu \n\t\
			end: %lu \n\t\
			diff Queue-Submit: \t%f\n\t\
			diff Submit-Start: \t%f\n\t\
			diff Start-End: \t%f\n\t\
			diff Queue-End: \t%f\n\
			diff Total Submit  \t%f\n",
		(unsigned long)queued, \
		(unsigned long)submit, \
		(unsigned long)start, \
		(unsigned long)end, \
		(double)(diffQueued) / 1000000.0, \
		(double)(diffSubmit) / 1000000.0, \
		(double)(diffStart) / 1000000.0, \
		(double)(diffTotal) / 1000000.0, \
		(double)(diffQueued+diffSubmit) / 1000000.0 \
	);

	fflush(stdout);

}

void eventStats::printAvgs()
{

	queueAvg = (double)((double)(queueSum) / iterations);
	submitAvg = (double)((double)(submitSum) / iterations);
	startAvg = (double)((double)(startSum) / iterations);
	totalAvg = (double)((double)(totalSum) / iterations);
	completeAvg = (double)((double)(completeSum) / iterations);
	queuePercentage = 100 * queueAvg / totalAvg;
	submitPercentage = 100 * submitAvg / totalAvg;
	workPercentage = 100 * startAvg / totalAvg;

	printf("\n----FINAL AVERAGES----%d Iterations\nEvent time (ms): \n\t\
			Average time in Queue: \t%f\n\t\
			Average time in Submit: %f\n\t\
			Average time in Start: \t%f\n\t\
			Average total time: \t%f\n\t\
			Work Percentage: \t%02.3f\n\t\
			Submit Percentage: \t%02.3f\n\t\
			Average Queue+Submit: \t%f\n\t\
			Complete: \t%f\n\t\
			", \
		iterations, \
		(double)(queueAvg) / 1000000.0, \
		(double)(submitAvg) / 1000000.0, \
		(double)(startAvg) / 1000000.0, \
		(double)(totalAvg) / 1000000.0, \
		workPercentage, \
		submitPercentage, \
		(double)(queueAvg + submitAvg) / 1000000.0, \
		(double)(completeAvg) / 1000000.0 \
	);
}

void eventStats::printAvgsCSV(FILE * out){

	fprintf(out,"%f,%f,%f,%f\n",\
		(double)(queueAvg) / 1000000.0, \
		(double)(submitAvg) / 1000000.0, \
		(double)(startAvg) / 1000000.0, \
		(double)(totalAvg) / 1000000.0);



}
