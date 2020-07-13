/*
 * This program is a heavily edited version of c_fft6 from the omp source code repository, which adapts it to a heterogeneous multicore CPU-GPU System.
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */



#include "clSetup.h"

#include "config.h"
#include <cassert>
#include <string>
//#include <atomic>
#include <math.h>
#include "clBuffer.h"
#include "utils.h"
#include <unistd.h>
#include <queue>
#include <string>

#include "fft6.h"
#include "commBuffer.h"

/* Input values (CONSTANTS) */
unsigned NN;                 /* input vector is NN complex values */
unsigned LOGNN;              /* log base 2 of NN */
unsigned N;                  /* square root of NN */
unsigned LOGN;               /* log base 2 of N   */

/* -----------------------------------------------------------------------
                             FFT6 PROTOTYPES
 * ----------------------------------------------------------------------- */
int prperf(double tpose_time, double scale_time, double ffts_time, int nn, int lognn, int iters);
int gen_bit_reverse_table(int *brt, int n);
int gen_w_table(complex *w, int n, int logn, int ndv2);
int gen_v_table(complex *v, int n);
int dgen(complex *xin, int n);
int tpose(complex *a, complex *b, int n);
int tpose_seq(complex *a, complex *b, const int n);
int cffts(complex *a, int *brt, complex *w, int n, int logn, int ndv2);
int cffts_seq(complex *a, int *brt, complex *w, const int n, int logn, int ndv2);
int fft(complex *a, int *brt, complex *w, int n, int logn, int ndv2);
int scale(complex *a, complex *v, int n);
int scale_seq(complex *a, complex *v, const int n);
int chkmat(complex *a, int n);
complex complex_pow(complex z, int n);
int log2_int(int n);
int print_mat(complex *a, int n);
int print_vec(complex *a, int n);

/* -----------------------------------------------------------------------
                             END FFT6 PROTOTYPES
 * ----------------------------------------------------------------------- */

//OpenCL Context
extern ocl_args_d_t *ocl;

//Buffer/Benchmark info Container
CL_Buffers_t *pCommBuffer;

/* -----------------------------------------------------------------
                         FFT6 IMPLEMENTATION
 * ----------------------------------------------------------------- */
/* -----------------------------------------------------------------
       gen_bit_reverse_table - initialize bit reverse table
       Postcondition: br(i) = bit-reverse(i-1) + 1
   ----------------------------------------------------------------- */
int gen_bit_reverse_table(int *brt, int n) {
  register int i, j, k, nv2;

  nv2 = n / 2;
  j = 1;
  brt[0] = j;
  for (i = 1; i < n; ++i) {
	  k = nv2;
    while (k < j) {
			j -= k;
			k /= 2;
		}
	  j += k;
	  brt[i] = j;
  }
  return 0;
}
/* -----------------------------------------------------------------------
   gen_w_table: generate powers of w.
   postcondition: w(i) = w**(i-1)
 * ----------------------------------------------------------------------- */
int gen_w_table(complex *w, int n, int logn, int ndv2) {
    register int i;
		 complex ww, pt;

    ww.re = cos(PI / ndv2);
    ww.im = -sin(PI / ndv2);
		w[0].re = 1.f;
		w[0].im = 0.f;
    pt.re = 1.f;
    pt.im = 0.f;
    for (i = 1; i < ndv2; ++i) {
			w[i].re = pt.re * ww.re - pt.im * ww.im;
			w[i].im = pt.re * ww.im + pt.im * ww.re;
			pt = w[i];
    }
    return 0;
}
/* -----------------------------------------------------------------------
       gen_v_table - gen 2d twiddle factors
 * ----------------------------------------------------------------------- */
int gen_v_table(complex *v, int n) {
  register int j, k;
   complex wn;

  wn.re = cos(PI * 2.f / (n * n));
  wn.im = -sin(PI * 2.f / (n * n));
	for (j = 0; j < n; ++j) {
    for (k = 0; k < n; ++k) {
			v[j * n + k] = complex_pow(wn, j * k);
	  }
  }
  return 0;
}
/* -----------------------------------------------------------------------
           z^n = modulo^n (cos(n * alfa) + i sin(n * alfa)
 * ----------------------------------------------------------------------- */
complex complex_pow(complex z, int n) {
	 complex temp;
	 real_type pot, nangulo, modulo, angulo;

	modulo = sqrt(z.re * z.re + z.im * z.im);
	angulo = atan(z.im / z.re);
  pot = pow(modulo, n);
	nangulo = n * angulo;

	temp.re = pot * cos(nangulo);
	temp.im = pot * sin(nangulo);
  return(temp);
}
/* -----------------------------------------------------------------------
   A: Para una señal de entrada de tamaño N y de forma
                                  (1,0), (1,0), ..., (1,0)
   la salida debe ser de la forma
                                  (N,0), (0,0), ..., (0,0)

   B: Para una señal de entrada de tamaño N y de forma
                                  (0,0), (0,0), ..., (N*N, 0), ..., (0,0)
   la salida debe ser de la forma (N*N, 0), (0,0), ..., (0,0)

   Propiedad útil: C: FFT(s1 + s2) = FFT(s1) + FFT(s2)

 * ----------------------------------------------------------------------- */
int dgen(complex *xin, int n) {
  register int i;
	int nn = n * n;
	/* Señal de forma B */
  for (i = 0; i < nn; ++i) {
		xin[i].re = 0.f;
		xin[i].im = 0.f;
  }
  xin[nn / 2].re = (real_type)nn;

  /* Señal de forma A */
	/*
  for (i = 0; i < nn; ++i) {
		xin[i].re = 1.0;
		xin[i].im = 0.f;
  }
  */
  return 0;
}
/* ----------------------------------------------------------------- */
int tpose(complex *a, complex *b, const int n) {
  register int i, j;

//#pragma omp parallel for private(i, j)
  for (i = 0; i < n; ++i) {
    for (j = i; j < n; ++j) {
      b[i * n + j] = a[j * n + i];
      b[j * n + i] = a[i * n + j];
    }
  }
  return 0;
}

/* ----------------------------------------------------------------- */
int tpose_seq(complex *a, complex *b, const int n) {
  register int i, j;

  for (i = 0; i < n; ++i) {
    for (j = i; j < n; ++j) {
      b[i * n + j] = a[j * n + i];
      b[j * n + i] = a[i * n + j];
    }
  }
  return 0;
}
/* ----------------------------------------------------------------- */
int scale(complex *a, complex *v, const int n) {
  register int i, j, index;
	complex aa, vv;

//#pragma omp parallel for private(i, j, index, aa, vv)
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
			index = i * n + j;
      aa = a[index];
			vv = v[index];
			a[index].re = aa.re * vv.re - aa.im * vv.im;
			a[index].im = aa.re * vv.im + aa.im * vv.re;
    }
  }
  return 0;
}
/* ----------------------------------------------------------------- */
int scale_seq(complex *a, complex *v, const int n) {
  register int i, j, index;
	complex aa, vv;

  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
			index = i * n + j;
      aa = a[index];
			vv = v[index];
			a[index].re = aa.re * vv.re - aa.im * vv.im;
			a[index].im = aa.re * vv.im + aa.im * vv.re;
    }
  }
  return 0;
}
/* -----------------------------------------------------------------------
       chkmat - check the output matrix for correctness
 * ----------------------------------------------------------------------- */
int chkmat(complex *a, int n) {
  int sign, i, j, nn, errors;
  real_type EPSILON = 1e-4f;

  errors = 0;
	nn = n * n;
  for (i = 0; i < n; ++i) {
    sign = 1;
    for (j = 0; j < n; ++j) {
      if (a[i * n + j].re > nn * sign + EPSILON)
        ++errors;
      if (a[i * n + j].re < nn * sign - EPSILON)
        ++errors;
      sign = -sign;
    }
  }
  if (errors > 0) {
		printf("Errors = %d\n", errors);
		exit(-1);
  } else{
    printf("\ngrep: Passed chkmat()\n\n");fflush(stdout);
  }
  return 0;
}
/* ----------------------------------------------------------------- */
int prperf(double tpose_time, double scale_time, double ffts_time, int nn, int lognn, int iters) {
   double secs;
   double fpercent, spercent, tpercent;
   double flops, mflops;

  tpose_time /= iters;
  scale_time /= iters;
  ffts_time /= iters;
  secs = tpose_time + scale_time + ffts_time;
  tpercent = tpose_time / secs * 100;
  spercent = scale_time / secs * 100;
  fpercent = ffts_time / secs * 100;
  flops = (real_type) (nn * 5 * lognn);
  mflops = flops / 1e6f / secs;
  printf("***********************\n");
  printf("1D FFT %d points\n" ,nn);
  printf("***********************\n");
  printf("Time per input vector:\n");
  printf("tpose    : %lf %lf percent\n", tpose_time, tpercent);
  printf("scale    : %lf %lf percent\n", scale_time, spercent);
  printf("ffts     : %lf %lf percent\n", ffts_time, fpercent);
  printf("total(s) : %lf\n", secs);
  printf("mflop/s  : %lf\n", mflops);
  return 0;
} /* prperf */
/* -----------------------------------------------------------------------
    Base 2 logarithm
 * ----------------------------------------------------------------------- */
int log2_int(int n) {
		register int i, aux;

		aux = 1;
		for (i = 0; i <= 128; i++) {
				if (aux > n)
					 return (i - 1);
				aux <<= 1;
		}
  return -1;
}
/* ----------------------------------------------------------------- */
int print_mat(complex *a, int n) {
  register int i, j;

  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
			printf(" (%.0f, %.0f)", a[i * n + j].re, a[i * n + j].im);
    }
		printf("\n");
  }
	printf("\n");
  return 0;
}
/* ----------------------------------------------------------------- */
int print_vec(complex *a, int n) {
  register int i;

  for (i = 0; i < n*n; ++i)
		printf("  (%.0f, %.0f)", a[i].re, a[i].im);
	printf("\n\n");
  return 0;
}
/* ----------------------------------------------------------------- */

/* -----------------------------------------------------------------
                         END FFT6 IMPLEMENTATION
 * ----------------------------------------------------------------- */


//Pthreads Structures and functions

/* -----
Argument structure for the slave function
-----*/
struct args_slave_t {
	int index;
	int *brt;
	complex *w;
	int n;
	int logn;
	int ndv2;
};

/* -----
return structure for the slave function
-----*/
typedef struct ret_slave_t {


};

/* -----
Argument structure for the core function
-----*/

struct core_args_t {
	int index;
	int outer_i;
	//int nThreads;
};

void CPU_Core(core_args_t * args)
{
	int index = args->index;
	int outer_i = args->outer_i;

	int *brt = pCommBuffer->brt;
	int n = pCommBuffer->N;
	int logn = pCommBuffer->LOGN;
	int ndv2 = n/2;
	complex *a_ptr = pCommBuffer->a_flat.ptr();
	complex *a = &a_ptr[outer_i*n];
	complex *w = pCommBuffer->w.ptr();
	int iter = 0;
	int inner_n;

	register int i, j;
		int powerOfW, stage, first, spowerOfW;
		int ijDiff;
		int stride;
		complex ii, jj, temp, pw;

 /* bit reverse step */
 for (i = 0; i < n; ++i) {
	 j = brt[i];
	 if (i < (j-1)) {
		 temp = a[j - 1];
		 a[j - 1] = a[i];
		 a[i] = temp;
	 }
 }

 /* butterfly computations */
 ijDiff = 1;
 stride = 2;
 spowerOfW = ndv2;
 for (stage = 0; stage < logn; ++stage) {
	 /* Invariant: stride = 2 ** stage
			Invariant: ijDiff = 2 ** (stage - 1) */
	 first = 0;
	 for (powerOfW = 0; powerOfW < ndv2; powerOfW += spowerOfW) {
		 inner_n = (n-first+stride-1)/stride;
#ifdef PERLOOP
		 if (inner_n >= pCommBuffer->cutoff)
		 {
			 start_timer(0);
		 }
#endif
			 pw = w[powerOfW];
			 /* Invariant: pwr + sqrt(-1)*pwi = W**(powerOfW - 1) */
			 for (i = first; i < n; i += stride) {
				 j = i + ijDiff;
				 jj = a[j];
				 ii = a[i];
				 temp.re = jj.re * pw.re - jj.im * pw.im;
				 temp.im = jj.re * pw.im + jj.im * pw.re;
				 a[j].re = ii.re - temp.re;
				 a[j].im = ii.im - temp.im;
				 a[i].re = ii.re + temp.re;
				 a[i].im = ii.im + temp.im;
			 }
#ifdef PERLOOP
		if (inner_n >= pCommBuffer->cutoff)
		{
			accumulate_timer(0);
		}
#endif

		 iter++;
		 ++first;
	 }
	 ijDiff = stride;
	 stride <<= 1;
	 spowerOfW /= 2;
 }
}


void *CPU_Slave(void * arguments)
{

	args_slave_t args = *( (args_slave_t *)arguments);

	int tid = args.index;
	int n = pCommBuffer->N;
	int nthreads = pCommBuffer->nThreads;

	//printf("STATUS::Entering CPU_Slave with index = %d\n",tid);fflush(stdout);

	core_args_t core_args;
	core_args.index = tid;

	//Call Core
	for( int i = tid; i < n; i+= nthreads)
	{
		core_args.outer_i = i;
		CPU_Core(&core_args);
	}
	return (void *)NULL;

}

//CFFTS
void CPU_Main()
{

	//LogInfo("Entering CPU Compute (pthreads)\n");
	int N = pCommBuffer->N;
	int num_threads = pCommBuffer->nThreads;

	int index;
	int rc;
	pthread_t *threads;
	args_slave_t * thread_args;

	threads = (pthread_t *)malloc((num_threads) * sizeof(pthread_t));
	thread_args = (args_slave_t *)malloc(num_threads * sizeof(args_slave_t));

	pCommBuffer->launch_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pCommBuffer->launch_lock, NULL);

	for(index = 1; index < num_threads; index++){

		//LogInfo("LAUNCHING SLAVE %d|N: %d\n", index,N);
		thread_args[index].index = index;

			rc = pthread_create( &threads[index], NULL, CPU_Slave, &thread_args[index] );
			if (rc != 0){
					LogError("ERROR: Failed to create thread index = %d, rc = %d\n", index, rc);
					exit(-1);
			}
		//LogInfo("SLAVE %d LAUNCHED\n", index);

	}

	core_args_t core_args;
	core_args.index = 0;

	roi_enter();

	//LogInfo("Main thread core LAUNCHED\n");
	for( int i = 0; i < N; i+= num_threads)
	{
		if(i%(N/8)==0)
		{
			LogInfo("Main outer_i = %d\n",i);
		}
		core_args.outer_i = i;
		CPU_Core(&core_args);
	}
	for(index  = 1; index < num_threads; index++) {
		rc = pthread_join(threads[index],NULL);
		if (rc != 0){
					printf("ERROR: Failed to join thread index = %d, rc = %d\n", index, rc);
					exit(-1);
			}
	}

	roi_exit();

}

void CL_Buffers_t::GPU_Core(core_args_t * args)
{
	int index = args->index;
	int outer_i = args->outer_i;

	int n = N;
	int logn = LOGN;
	int ndv2 = n/2;
	complex *a_ptr = a_flat.ptr();
	complex *a = &a_ptr[outer_i*n];
	//complex *w = w.ptr();
	int iter = 0;
	int thread_iter;
	int inner_n;
	int inner_blocks;

	register int i, j;
		int powerOfW, stage, first, spowerOfW;
		int ijDiff;
		int stride;
		complex ii, jj, temp, pw;

		/* bit reverse step */
		for (i = 0; i < n; ++i) {
			j = brt[i];
			if (i < (j-1)) {
				temp = a[j - 1];
				a[j - 1] = a[i];
				a[i] = temp;
			}
		}

		/* butterfly computations */
		ijDiff = 1;
		stride = 2;
		spowerOfW = ndv2;
		for (stage = 0; stage < logn; ++stage) {
			/* Invariant: stride = 2 ** stage
				 Invariant: ijDiff = 2 ** (stage - 1) */
			first = 0;
			for (powerOfW = 0; powerOfW < ndv2; powerOfW += spowerOfW) {
				//pw = w[powerOfW];
				/* Invariant: pwr + sqrt(-1)*pwi = W**(powerOfW - 1) */
				inner_n = (n-first+stride-1)/stride;


if (inner_n >= cutoff)
{
#ifdef PERLOOP
	start_timer(0);
#endif
				//printf("inner\n");
				thread_iter = iters*index + iter;
				inner_blocks = ceil((float)inner_n/(float)pCommBuffer->lws);

				//"Kernel Arguments that change every iteration"
				pIjDiff[index] = ijDiff;
				pPoW[index] = powerOfW;
				pFirst[index] = first;
				pStride[index] = stride;
				pOuter_i[index] = outer_i;
				pInner_n[index] = inner_n;

				int l_iter = pCommBuffer->GPU_Schedule(index,inner_blocks,inner_n);
				GPU_Sync(index, l_iter, inner_blocks,inner_n);
#ifdef PERLOOP
	accumulate_timer(0);
#endif
} else {
	pw = w[powerOfW];
	for (i = first; i < n; i += stride) {
		//printf("outer_i %d | first %d | i %d\n",outer_i,first,i);
		j = i + ijDiff;
		//printf("stage %d | PoW %d | i %d | ijDiff %d | j %d | first %d | stride %d | sPow %d\n",stage,powerOfW,i,ijDiff,j,first,stride,spowerOfW);
		jj = a[j];
		ii = a[i];
		temp.re = jj.re * pw.re - jj.im * pw.im;
		temp.im = jj.re * pw.im + jj.im * pw.re;
		a[j].re = ii.re - temp.re;
		a[j].im = ii.im - temp.im;
		a[i].re = ii.re + temp.re;
		a[i].im = ii.im + temp.im;
	}

}
				iter++;
				++first;
			}
			ijDiff = stride;
			stride <<= 1;
			spowerOfW /= 2;
		}

		//printf("GPU Did %d iterations for size %d\n",iter,n);

}

void *GPU_Slave(void * arguments)
{

	args_slave_t args = *( (args_slave_t *)arguments);

	int tid = args.index;
	int n = pCommBuffer->N;
	int nthreads = pCommBuffer->nThreads;

	//printf("STATUS::Entering GPU_Slave with index = %d\n",tid);fflush(stdout);

	core_args_t core_args;
	core_args.index = tid;

	//Call Core
	for( int i = tid; i < n; i+= nthreads)
	{
		core_args.outer_i = i;
		pCommBuffer->GPU_Core(&core_args);
	}
	return (void *)NULL;


}

struct main_args_t {
};

void CL_Buffers_t::GPU_Main(main_args_t * args)
{
	int num_threads = nThreads;

	int index;
	int rc;
	pthread_t *threads;
	args_slave_t * thread_args;

	threads = (pthread_t *)malloc((num_threads) * sizeof(pthread_t));
	thread_args = (args_slave_t *)malloc(num_threads * sizeof(args_slave_t));

	pCommBuffer->launch_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pCommBuffer->launch_lock, NULL);

	for(index = 1; index < num_threads; index++){

		//LogInfo("LAUNCHING SLAVE %d|N: %d\n", index,N);
		thread_args[index].index = index;

			rc = pthread_create( &threads[index], NULL, GPU_Slave, &thread_args[index] );
			if (rc != 0){
					LogError("ERROR: Failed to create thread index = %d, rc = %d\n", index, rc);
					exit(-1);
			}
		//LogInfo("SLAVE %d LAUNCHED\n", index);

	}
	core_args_t core_args;
	core_args.index = 0;

	roi_enter();
	GPU_Set_Kernel_Args();
	GPU_Launch_Kernel();
	
	//LogInfo("Main thread core LAUNCHED\n");
	for( int i = 0; i < N; i+= num_threads)
	{
		if(i%(N/8)==0)
		{
			LogInfo("Main outer_i = %d\n",i);
		}
		core_args.outer_i = i;
		GPU_Core(&core_args);
	}
	for(index  = 1; index < num_threads; index++) {
		rc = pthread_join(threads[index],NULL);
		if (rc != 0){
					printf("ERROR: Failed to join thread index = %d, rc = %d\n", index, rc);
					exit(-1);
			}
	}

	roi_exit();
	KillCommKernel();

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
	int cutoff;

	//Parse your inputs

	//Array Size
	if(argc>=2) {
		N = atoi(argv[1]);
	} else {
		N = 8192;
	}

	//nThreads
	if (argc>=3){
		num_threads = atoi(argv[2]);
	}

	//Kernel Type
	if(argc>=4)
	{
		int k_type = atoi(argv[3]);
		if(!(k_type == 0 || k_type == 1))
		{
				//if(argv)
				printf("ERROR FOR NOW argv[4] must be 0 or 1");
				exit(1);
		}
		Kernel_Type = (kType)k_type;
	}

	if(argc>=5)
	{

		cutoff = atoi(argv[4]);
	} else {
		cutoff = 64;
	}

	if(cutoff<64 && Kernel_Type == GPU){
		LogError("A cutoff of %d will not work for GPU\n",cutoff);
		exit(1);

	}

	LogInfo("Width: %d\n", N);
	LogInfo("nThreads: %d\n", num_threads);
	LogInfo("cutoff: %d\n", cutoff);

	//Intialize container 1 queue and 1 kernel
	*ocl = ocl_args_d_t(1,1);

	printf("\n---Setting up OpenCL\n");
	//initialize Open CL objects (context, queue, etc.)
	CHECK_ERRORS(SetupOpenCL(deviceType));
	//printf("\n---Finished Setting up OpenCL\n");

	//printDeviceInfo(ocl->device);

	LogInfo("\n---Generated input, Calling CreateandBuildProgram\n");

	std::string kernel_file_path = "/home/dgerzhoy/Workspace/LaunchDaemon_clean/FFT6/";
	std::string kernel_file_name = "Kernels.cl";
	// Create and build the OpenCL program
	CHECK_ERRORS(CreateAndBuildProgram(kernel_file_path,kernel_file_name));


	int WG_Size_Multiple = 32;

	//Compile Kernels
	switch (Kernel_Type) {
		case CPU:
			break;
		case GPU:
			*(ocl->kernels) = clCreateKernel(ocl->program, "Launch_Daemon_FFT6", &err);
			break;
		default:
			printf("Default Case! This shouldnt happen\n!"); exit(1);
	}
	if(Kernel_Type != CPU)
	{
		CHECK_ERRORS(err);
		CHECK_ERRORS(clGetKernelWorkGroupInfo(ocl->kernels[0], ocl->device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t), &WG_Size_Multiple, NULL));
		//LogInfo("Kernel returned %d\n",WG_Size_Multiple);
	}

	//Initialize the Communication Buffer
	pCommBuffer = new CL_Buffers_t(N, num_threads,WG_Size_Multiple);
	pCommBuffer->cutoff = cutoff;

	/* precompute the input array and fft constants */
	gen_bit_reverse_table(pCommBuffer->brt, pCommBuffer->N);
	gen_w_table(pCommBuffer->w.ptr(), pCommBuffer->N, pCommBuffer->LOGN, pCommBuffer->N / 2);
	gen_v_table(pCommBuffer->v, pCommBuffer->N);

	dgen(pCommBuffer->xin,N);
	tpose(pCommBuffer->xin,pCommBuffer->a_flat.ptr(),N);
	//exit(1);
	//main_args_t args;
	std::string filename;
	switch (Kernel_Type) {
		case CPU:
			filename = "Check_CPU";
			CPU_Main();//NPARTS, NDIM, mass);//position, velocity, mass, force, &potential, &kinetic);
			break;
		case GPU:
			filename = "Check_GPU";
			pCommBuffer->GPU_Main(NULL);
			break;
		default:
			printf("Default Case! This shouldnt happen\n!"); exit(1);
	}
	
	filename = filename + argv[2] + "_" + argv[1];
	//pCommBuffer->Check(filename.c_str());	

	//print_timers();

	return 0;
}
