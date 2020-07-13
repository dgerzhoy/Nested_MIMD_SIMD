/*
 * This program is a heavily edited version of c_MD from the ompscr, which adapts it to a heterogeneous multicore CPU-GPU System.
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */


#include "clSetup.h"

#include <cassert>
#include <string>
#include <atomic>
#include <math.h>
#include "clBuffer.h"
#include "utils.h"
#include <unistd.h>
//#include "kernel.h"
#include <queue>

#include "MD.h"
#include "commBuffer.h"

int NPARTS;       /* No. of particles */

/* -----------------------------------------------------------------------
                          PROTOTYPES
 * ----------------------------------------------------------------------- */

float v(float x);
float dv(float x);
//void initialize(int np, int nd, vnd_t box, vnd_t *pos, vnd_t *vel, vnd_t *acc);
void initialize(int np, int nd, vnd_t box);//, vnd_t *pos, vnd_t *vel, vnd_t *acc);
float dist(int nd, vnd_t r1, vnd_t r2, vnd_t dr);
float dot_prod(int n, cl_float3 x, cl_float3 y);
void compute(int np, int nd, vnd_t *pos, vnd_t *vel, float mass, vnd_t *f, float *pot_p, float *kin_p);
void update(int np, int nd, vnd_t *pos, vnd_t *vel, vnd_t *f, vnd_t *a, float mass, float dt);
int main (int argc, char **argv);

//OpenCL Context
extern ocl_args_d_t *ocl;


//Buffer/Benchmark info Container
CL_Buffers_t *pCommBuffer;


/* -----------------------------------------------------------------------
                          IMPLEMENTATION
 * ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
   statement function for the pair potential.
   This potential is a harmonic well which smoothly saturates to a
   maximum value at PI/2.
 * ----------------------------------------------------------------------- */
float v(float x) {
  if (x < M_PI_2)
    return pow(sin(x), 2.0f);
  else
    return 1.0f;
}
/* -----------------------------------------------------------------------
   statement function for the derivative of the pair potential
 * ----------------------------------------------------------------------- */
float dv(float x) {
  if (x < M_PI_2)
    return 2.0f * sin(x) * cos(x);
  else
    return 0.0f;
}
/* -----------------------------------------------------------------------
   Initialize the positions, velocities, and accelerations.
 * ----------------------------------------------------------------------- */
void initialize(vnd_t box){//int np, int nd, vnd_t box)
  int i, j;
  float x;

	int np = pCommBuffer->N;
	int nd = pCommBuffer->nd;

  srand(4711L);
  for (i = 0; i < np; i++) {
    for (j = 0; j < nd; j++) {
      x = rand() % 10000 / (float)10000.0;
      pCommBuffer->pos[i].s[j] = box[j] * x;
      pCommBuffer->vel[i].s[j] = 0.0;
      pCommBuffer->accel[i].s[j] = 0.0;
    }
  }
}
/* -----------------------------------------------------------------------
   Compute the displacement vector (and its norm) between two particles.
 * ----------------------------------------------------------------------- */
float dist(int nd, vnd_t r1, vnd_t r2, vnd_t dr) {
  int i;
  float d;

  d = 0.0;
  for (i = 0; i < nd; i++) {
    dr[i] = r1[i] - r2[i];
    d += dr[i] * dr[i];
  }
  return sqrt(d);
}
/* -----------------------------------------------------------------------
   Return the dot product between two vectors of type float and length n
 * ----------------------------------------------------------------------- */
float dot_prod(int n, cl_float3 x, cl_float3 y) {
  int i;
  float t = 0.0;

  for (i = 0; i < n; i++) {
    t += x.s[i] * y.s[i];
  }
  return t;
}

/* -----------------------------------------------------------------------
   Perform the time integration, using a velocity Verlet algorithm
 * ----------------------------------------------------------------------- */
void update(int np, int nd, vnd_t *pos, vnd_t *vel, vnd_t *f, vnd_t *a, float mass, float dt) {
  int i, j;
  float rmass;

  rmass = 1.0/mass;
  /* The time integration is fully parallel */
//#pragma omp parallel for default(shared) private(i, j) firstprivate(rmass, dt)
  for (i = 0; i < np; i++) {
    for (j = 0; j < nd; j++) {
      pos[i][j] = pos[i][j] + vel[i][j]*dt + 0.5*dt*dt*a[i][j];
      vel[i][j] = vel[i][j] + 0.5*dt*(f[i][j]*rmass + a[i][j]);
      a[i][j] = f[i][j]*rmass;
    }
  }
}

//End Functionally Unchanged MD functions

//Pthreads Structures and functions

/* -----
Argument structure for the slave function
-----*/
struct args_slave_t {
	int index;
};

/* -----
return structure for the slave function
-----*/
typedef struct ret_slave_t {

	float pot;
	float kin;
};

void CPU_Core(int index, float *pot_p, float *kin_p)
{
	//printf("Calling CPU Core!\n");

	int np = pCommBuffer->N;
	int nd = pCommBuffer->nd;
	int num_threads = pCommBuffer->nThreads;

	int i, j, k, g;
	cl_float3 rij;
	float  d;
	float pot, kin;
	int l;

	
	pot = 0.0;
	kin = 0.0;

	/* The computation of forces and energies is fully parallel. */
	for (i = index; i < np; i = i + num_threads) {
		/* compute potential energy and forces */
		for (j = 0; j < nd; j++)
			pCommBuffer->force[i].s[j] = 0.0f;

			//Inner loop
		start_timer(0);
		for (j = 0; j < np; j++) {
			if (i != j) {

				//d = dist(nd, pos[i], pos[j], rij);
				d = 0.0;
				for (l = 0; l < nd; l++) {
					rij.s[l] = pCommBuffer->pos[i].s[l] - pCommBuffer->pos[j].s[l];
					d += rij.s[l] * rij.s[l];
				}
				d = sqrt(d);

				/* attribute half of the potential energy to particle 'j' */
				pot = pot + 0.5 * v(d);

				for (k = 0; k < nd; k++) {
					pCommBuffer->force[i].s[k] = pCommBuffer->force[i].s[k] - rij.s[k]* ((d < M_PI_2) ? (2.0f * sin(d) * cos(d)) : 0.0f)/d;
				}
			}
		}
		accumulate_timer(0);

		/* compute kinetic energy */
		//Changing j to 2nd i, looks like mistake...doesn't affect performance though.
		kin = kin + dot_prod(nd, pCommBuffer->vel[i], pCommBuffer->vel[i]);
	}

	*pot_p = pot;
	*kin_p = kin;

}

void *CPU_Slave(void * arguments)
{

	args_slave_t args = *( (args_slave_t *)arguments);

	int index = args.index;

	printf("STATUS::Entering CPU_Slave with index = %d\n",index);fflush(stdout);

	ret_slave_t *ret_vals = (ret_slave_t *)malloc(sizeof(ret_slave_t));

	float pot = 0;
	float kin = 0;

	//Call Core
	CPU_Core(index,&pot,&kin);

	ret_vals->pot = pot;
	ret_vals->kin = kin;

	return (void *)ret_vals;

}

void CPU_Main()
{

	printf("Entering CPU Compute (pthreads)\n");
	int np = pCommBuffer->N;
	int nd = pCommBuffer->nd;
	int num_threads = pCommBuffer->nThreads;
	float mass = pCommBuffer->mass;

	float pot;
	float kin;

	int index;
	int rc;
	pthread_t *threads;
	args_slave_t * thread_args;

	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	for (int j = 0; j < num_threads; j++)
	{
  	CPU_SET(j, &cpuset);
	}

	threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
	thread_args = (args_slave_t *)malloc(num_threads * sizeof(args_slave_t));

	roi_enter();

	threads[0] = pthread_self();
	rc = pthread_setaffinity_np(threads[0], sizeof(cpu_set_t), &cpuset);
	if(rc != 0)
	{
		LogError("ERROR: Failed to set thread affinity index = %d, rc = %d\n", index, rc);
   		exit(-1);
	}

	for(index = 1; index < num_threads; index++){

		// printf("LAUNCHING SLAVE %d|np: %d\n", index,np);
		thread_args[index].index = index;

			rc = pthread_create( &threads[index], NULL, CPU_Slave, &thread_args[index] );
			if (rc != 0){
      		LogError("ERROR: Failed to create thread index = %d, rc = %d\n", index, rc);
      		exit(-1);
    	}
			rc = pthread_setaffinity_np(threads[index], sizeof(cpu_set_t), &cpuset);
			if(rc != 0)
			{
				LogError("ERROR: Failed to set thread affinity index = %d, rc = %d\n", index, rc);
	      		exit(-1);
			}

		// printf("SLAVE %d LAUNCHED\n", index);

	}

	// printf("Main thread core LAUNCHED\n");
	CPU_Core(0,&pot,&kin);

	for(index  = 1; index < num_threads; index++) {
		void *ret_vals;
		rc = pthread_join(threads[index],&ret_vals);
		if (rc != 0){
      		printf("ERROR: Failed to join thread index = %d, rc = %d\n", index, rc);
      		exit(-1);
    	}

		pot = pot + ((ret_slave_t *)ret_vals)->pot;
		kin = kin + ((ret_slave_t *)ret_vals)->kin;

		free(ret_vals);

	}

	kin = kin * 0.5 * mass;
  	pCommBuffer->pot = pot;
  	pCommBuffer->kin = kin;
	printf("pot = %f | kin = %f\n",pCommBuffer->pot,pCommBuffer->kin);

	roi_exit();

}

void GPU_Core(int index, float *pot_p, float *kin_p)
{

	int np = pCommBuffer->N;
	int nd = pCommBuffer->nd;
	int num_threads = pCommBuffer->nThreads;
	const int nG = NUM_GROUPS;

	int i, j, k;
	cl_float3 rij;
	float  d;
	float pot, kin;
	int l;

	pot = 0.0;
	kin = 0.0;

	/* The computation of forces and energies is fully parallel. */
	for (i = index; i < np; i = i + num_threads) {

		/* compute potential energy and forces */
		//Inner loop
		pCommBuffer->GPU_Schedule(index,i);
		pCommBuffer->GPU_Sync(index, i);

		//Do final reduction across the blocks
		for(int r = 0; r < pCommBuffer->blocks; r++)
		{
			pot += pCommBuffer->pPot[i*nG+r];
		}

		/* compute kinetic energy */
		//Changing j to 2nd i, looks like mistake...doesn't affect performance though.
		kin = kin + dot_prod(nd, pCommBuffer->vel[i], pCommBuffer->vel[i]);
	}

	*pot_p = pot;
	*kin_p = kin;
}

void *GPU_Slave(void * arguments)
{

	args_slave_t args = *( (args_slave_t *)arguments);

	int index = args.index;

	//printf("STATUS::Entering GPU_Slave with index = %d\n",index);fflush(stdout);

	ret_slave_t *ret_vals = (ret_slave_t *)malloc(sizeof(ret_slave_t));

	float pot = 0;
	float kin = 0;

	//Call Core
	GPU_Core(index,&pot,&kin);

	ret_vals->pot = pot;
	ret_vals->kin = kin;

	return (void *)ret_vals;

}

void GPU_Main()
{
	int np = pCommBuffer->N;
	int nThreads = pCommBuffer->nThreads;
	int gR = pCommBuffer->group_residency;
	int nB = pCommBuffer->blocks;
	const int nG = NUM_GROUPS;

	float mass = pCommBuffer->mass;

	float pot;
	float kin;

	int index;
	int rc;
	pthread_t *threads;
	args_slave_t * thread_args;

	threads = (pthread_t *)malloc((nThreads) * sizeof(pthread_t));
	thread_args = (args_slave_t *)malloc(nThreads * sizeof(args_slave_t));

	pCommBuffer->launch_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pCommBuffer->launch_lock, NULL);

	roi_enter();
	pCommBuffer->GPU_Set_Kernel_Args();
	pCommBuffer->GPU_Launch_Kernel();

	for(index = 1; index < nThreads; index++){

		// printf("LAUNCHING SLAVE %d|np: %d\n", index,np);
		thread_args[index].index = index;

			rc = pthread_create( &threads[index], NULL, GPU_Slave, &thread_args[index] );
			if (rc != 0){
      		LogError("ERROR: Failed to create thread index = %d, rc = %d\n", index, rc);
      		exit(-1);
    	}

		// printf("SLAVE %d LAUNCHED\n", index);

	}

	printf("Main thread core LAUNCHED\n");
	GPU_Core(0,&pot,&kin);

	for(index  = 1; index < nThreads; index++) {
		void *ret_vals;
		rc = pthread_join(threads[index],&ret_vals);
		if (rc != 0){
			LogError("ERROR: Failed to join thread index = %d, rc = %d\n", index, rc);
			exit(-1);
		}

		pot = pot + ((ret_slave_t *)ret_vals)->pot;
		kin = kin + ((ret_slave_t *)ret_vals)->kin;

		free(ret_vals);

	}
	kin = kin * 0.5 * mass;
  	pCommBuffer->pot = pot;
  	pCommBuffer->kin = kin;
	roi_exit();
	//print_timers();

	printf("pot = %f | kin = %f\n",pCommBuffer->pot,pCommBuffer->kin);
	pCommBuffer->KillCommKernel();

}

int main(int argc, char* argv[])
{
	//Generic Variables (Always here)
	cl_int err;
	cl_device_type deviceType = CL_DEVICE_TYPE_GPU;
	ocl = (ocl_args_d_t *)malloc(sizeof(ocl_args_d_t));
	int num_threads = 1;
	kType Kernel_Type = CPU;


	//Benchmark Specific Variabls
	float mass = 1.0;
  	float dt = 1.0e-4;
  	vnd_t box;
  	float potential, kinetic, E0;

	//Parse your inputs

	//Array Size
	if(argc>=2) {
		NPARTS  = atoi(argv[1]);
	} else {
		NPARTS  = 4096;
	}

	//nThreads
	if (argc>=3){
		num_threads = atoi(argv[2]);
	}

	if(argc>=4)
	{
		int k_type = atoi(argv[3]);
		if(!(k_type == 0 || k_type == 1))
		{
				printf("ERROR FOR NOW argv[4] must be 0 1");
				exit(1);
		}
		Kernel_Type = (kType)k_type;
	}

	printf("Width: %d\n", NPARTS);
	printf("nThreads: %d\n", num_threads);

	//Intialize container 1 queue and 1 kernel
	*ocl = ocl_args_d_t(1,1);

	// printf("\n---Setting up OpenCL\n");
	//initialize Open CL objects (context, queue, etc.)
	CHECK_ERRORS(SetupOpenCL(deviceType));
	//printf("\n---Finished Setting up OpenCL\n");

	//printDeviceInfo(ocl->device);

	// printf("\n---Generated input, Calling CreateandBuildProgram\n");

	std::string kernel_file_path = "/home/dgerzhoy/Workspace/LaunchDaemon_clean/MD/";
	std::string kernel_file_name = "Kernels.cl";
	// Create and build the OpenCL program
	CHECK_ERRORS(CreateAndBuildProgram(kernel_file_path,kernel_file_name));


	int WG_Size_Multiple = 32;

	//Compile Kernels
	switch (Kernel_Type) {
		case CPU: break;
		case GPU:
			*(ocl->kernels) = clCreateKernel(ocl->program, "Launch_Daemon_MD", &err);
			break;
		default:
			printf("Default Case! This shouldnt happen\n!"); exit(1);
	}
	if(Kernel_Type != CPU)
	{
		CHECK_ERRORS(err);
		CHECK_ERRORS(clGetKernelWorkGroupInfo(ocl->kernels[0], ocl->device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t), &WG_Size_Multiple, NULL));
		//printf("Kernel returned %d\n",WG_Size_Multiple);
	}
	
	//Initialize the Communication Buffer
	pCommBuffer = new CL_Buffers_t(NPARTS,num_threads,WG_Size_Multiple);
	pCommBuffer->mass = mass;


  	/* set initial positions, velocities, and accelerations */
  	for (int i = 0; i < NDIM; i++)
    	box[i] = 10.0;
 	initialize(box);


	switch (Kernel_Type) {
		case CPU:
			CPU_Main();//NPARTS, NDIM, mass);//position, velocity, mass, force, &potential, &kinetic);
			break;
		case GPU:
			GPU_Main();
			break;
		default:
			printf("Default Case! This shouldnt happen\n!"); exit(1);
	}

	return 0;
}
