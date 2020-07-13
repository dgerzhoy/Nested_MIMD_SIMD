#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include "utils.h"
#include "clSetup.h"

extern ocl_args_d_t *ocl;

template <typename T>
class cl_SVM_Buffer
{
public:
	cl_SVM_Buffer(cl_uint size);
	cl_SVM_Buffer(cl_uint size,const char * label);
	cl_SVM_Buffer(cl_uint size,const char * label,cl_svm_mem_flags Mem_Flags);
	cl_SVM_Buffer(cl_uint size,cl_uint reset1);
	~cl_SVM_Buffer();

	//class T;
	cl_uint size;

	//Reset value to 0
	void reset();

	//Set Kernel Args
	void bind(cl_kernel kernel, int arg);

	//Generate Random Data
	void generateRandomData();

	T operator[](int i)
	const { return buffer[i];}

	T& operator[](int i)
	{ return buffer[i];}

	T *ptr()
	{
			return buffer;
	}

private:
	//ocl_args_d_t *ocl;
	cl_uint buffer_size;
	//cl_svm_mem_flags mem_flags;
	T *buffer;
};



template <typename T>
cl_SVM_Buffer<T>::cl_SVM_Buffer(cl_uint size):
	//ocl(ocl),
	//mem_flags(mem_flags)
	size(size)
{
	if(!ocl)
		throw "You need a global ocl_args_d_t pointer!";

	//LogInfo("Creating a 1D array of size = %d\n",size);

	cl_svm_mem_flags mem_flags = CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_SVM_ATOMICS;;
	// the buffer should be aligned with 4K page and size should fit 64-byte cached line
	buffer_size = (((sizeof(T) * size) - 1) / 64 + 1) * 64;

	buffer = (T *)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);
	//buffer = (T *)malloc(buffer_size);

	if (NULL == buffer)
	{
		LogError("Error: clSVMAlloc (1D) failed to allocate buffers. \n");
		throw "Error: clSVMAlloc (1D) failed to allocate buffers. \n";
	}
	//LogInfo("\tStart Addr = %p\n",buffer);
	reset();

}

template <typename T>
cl_SVM_Buffer<T>::cl_SVM_Buffer(cl_uint size,const char * label):
	//ocl(ocl),
	//mem_flags(mem_flags)
	size(size)
{
	if(!ocl)
		throw "You need a global ocl_args_d_t pointer!";


	cl_svm_mem_flags mem_flags = CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_SVM_ATOMICS;;
	// the buffer should be aligned with 4K page and size should fit 64-byte cached line
	buffer_size = (((sizeof(T) * size) - 1) / 64 + 1) * 64;
	
	//LogInfo("Creating a 1D array of size = %d | (bsize %d) | (%s)\n",size,buffer_size,label);

	buffer = (T *)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);
	//buffer = (T *)malloc(buffer_size);

	if (NULL == buffer)
	{
		LogError("Error: clSVMAlloc (1D) failed to allocate buffers. \n");
		throw "Error: clSVMAlloc (1D) failed to allocate buffers. \n";
	}
	//LogInfo("\tStart Addr = %p\n",buffer);
	reset();

}

template <typename T>
cl_SVM_Buffer<T>::cl_SVM_Buffer(cl_uint size,const char * label, cl_svm_mem_flags Mem_Flags):
	//ocl(ocl),
	//mem_flags(mem_flags)
	size(size)
{
	if(!ocl)
		throw "You need a global ocl_args_d_t pointer!";

	//LogInfo("Creating a 1D array of size = %d | (%s)\n",size,label);

	cl_svm_mem_flags mem_flags = Mem_Flags | CL_MEM_SVM_FINE_GRAIN_BUFFER;
	// the buffer should be aligned with 4K page and size should fit 64-byte cached line
	buffer_size = (((sizeof(T) * size) - 1) / 64 + 1) * 64;

	buffer = (T *)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);
	//buffer = (T *)malloc(buffer_size);

	if (NULL == buffer)
	{
		LogError("Error: clSVMAlloc (1D) failed to allocate buffers. \n");
		throw "Error: clSVMAlloc (1D) failed to allocate buffers. \n";
	}
	//LogInfo("\tStart Addr = %p\n",buffer);
	reset();

}



template <typename T>
cl_SVM_Buffer<T>::cl_SVM_Buffer(cl_uint size, cl_uint reset1):
	//ocl(ocl),
	//mem_flags(mem_flags)
	size(size)
{
	if(!ocl)
		throw "You need a global ocl_args_d_t pointer!";

	cl_svm_mem_flags mem_flags = CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_SVM_ATOMICS;
	// the buffer should be aligned with 4K page and size should fit 64-byte cached line
	buffer_size = (((sizeof(T) * size) - 1) / 64 + 1) * 64;

	buffer = (T *)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);
	//buffer = (T *)malloc(buffer_size);

	if (NULL == buffer)
	{
		LogError("Error: clSVMAlloc (1D) failed to allocate buffers. \n");
		throw "Error: clSVMAlloc (1D) failed to allocate buffers. \n";
	}
	//LogInfo("\tStart Addr = %p\n",buffer);
	if(reset1)
		reset();

}

template <typename T>
cl_SVM_Buffer<T>::~cl_SVM_Buffer()
{
	if(buffer)
		clSVMFree(ocl->context,(void *)buffer);

}
template <typename T>
void cl_SVM_Buffer<T>::reset()
{
	if(buffer)
		memset((void *)buffer,0,buffer_size);
}

template <typename T>
void cl_SVM_Buffer<T>::bind(cl_kernel kernel, int arg)
{
	//printf("Binding arg %d\n",arg);
	CHECK_ERRORS(clSetKernelArgSVMPointer(kernel, arg, (void *)buffer));
}

template <typename T>
void cl_SVM_Buffer<T>::generateRandomData()
{
	srand(12345);
	T temp;

	for (int i = 0; i < size; ++i)
	{
		temp = (T)(rand()%128);
		buffer[i] = temp;
	}

}

template <typename T>
class cl_SVM_Buffer_2D_Flat
{
public:
	cl_SVM_Buffer_2D_Flat(uint size0, uint size1);
	cl_SVM_Buffer_2D_Flat(uint size0, uint size1,const char * label);
	cl_SVM_Buffer_2D_Flat(uint size0, uint size1,const char * label, cl_svm_mem_flags Mem_Flags);
	//cl_SVM_Buffer_2D_Flat(uint size0);
	~cl_SVM_Buffer_2D_Flat();

	//class T;
	cl_uint size0;
	cl_uint size1;
	const char * label;

	//void alloc(int i, int size1);

	//Reset value to 0
	void reset();

	//Set Kernel Args
	void bind(cl_kernel kernel, int arg);
	void bind(cl_kernel kernel, int arg, int i);

	//Generate Random Data
	void generateRandomData();

#if 1
	T *operator[](int i)
	{ return &buffer[i*size1];}

	//T operator()(int i, int j)
	//const { return buffer[i][j]; }

	//T& *operator[](int i)
	//{ return buffer[i];}

	T *ptr()
	{
		return buffer;
	}

#endif

private:
	//ocl_args_d_t *ocl;
	uint buffer_size;
	//cl_svm_mem_flags mem_flags;
	T *buffer;
};

template <typename T>
cl_SVM_Buffer_2D_Flat<T>::cl_SVM_Buffer_2D_Flat(uint size0, uint size1):
	//ocl(ocl),
	//mem_flags(mem_flags)
	size0(size0),
	size1(size1),
	label("NO_LABEL")
{
	if(!ocl)
		throw "You need a global ocl_args_d_t pointer!";

	//LogInfo("Creating a 2D array of size0 = %d | size1 = %d\n",size0,size1);

	// the buffer should be aligned with 4K page and size should fit 64-byte cached line
	#if 1
		cl_svm_mem_flags mem_flags = CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_SVM_ATOMICS;;
		buffer_size = (((sizeof(T) * size0*size1) - 1) / 64 + 1) * 64;
		buffer = (T *)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);
	#else
		buffer_size = (sizeof(T) * size0*size1);
		buffer = (T *)malloc(buffer_size);
	#endif

	if(NULL == buffer)
	{
		LogError("Error: malloc failed to allocate buffers. \n");
		throw "Error: malloc failed to allocate buffers. \n";
	}
	//LogInfo("\tStart Addr = %p\n",buffer);

	reset();

}

template <typename T>
cl_SVM_Buffer_2D_Flat<T>::cl_SVM_Buffer_2D_Flat(uint size0, uint size1, const char * label):
	//ocl(ocl),
	//mem_flags(mem_flags)
	size0(size0),
	size1(size1),
	label(label)
{
	if(!ocl)
		throw "You need a global ocl_args_d_t pointer!";

	//LogInfo("Creating a 2D array of size0 = %d | size1 = %d (%s)\n",size0,size1,label);

		// the buffer should be aligned with 4K page and size should fit 64-byte cached line
	#if 1
		cl_svm_mem_flags mem_flags = CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_SVM_ATOMICS;;
		buffer_size = (((sizeof(T) * size0*size1) - 1) / 64 + 1) * 64;
		buffer = (T *)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);
	#else
		buffer_size = (sizeof(T) * size0*size1);
		buffer = (T *)malloc(buffer_size);
	#endif

	if(NULL == buffer)
	{
		LogError("Error: malloc failed to allocate buffers. \n");
		throw "Error: malloc failed to allocate buffers. \n";
	}
	//LogInfo("\tStart Addr = %p\n",buffer);

	reset();

}

template <typename T>
cl_SVM_Buffer_2D_Flat<T>::cl_SVM_Buffer_2D_Flat(uint size0, uint size1, const char * label, cl_svm_mem_flags Mem_Flags):
	//ocl(ocl),
	//mem_flags(mem_flags)
	size0(size0),
	size1(size1),
	label(label)
{
	if(!ocl)
		throw "You need a global ocl_args_d_t pointer!";

	//LogInfo("Creating a 2D array of size0 = %d | size1 = %d (%s)\n",size0,size1,label);

		// the buffer should be aligned with 4K page and size should fit 64-byte cached line
	#if 1
		cl_svm_mem_flags mem_flags = Mem_Flags | CL_MEM_SVM_FINE_GRAIN_BUFFER;
		buffer_size = (((sizeof(T) * size0*size1) - 1) / 64 + 1) * 64;
		buffer = (T *)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);
	#else
		buffer_size = (sizeof(T) * size0*size1);
		buffer = (T *)malloc(buffer_size);
	#endif

	if(NULL == buffer)
	{
		LogError("Error: malloc failed to allocate buffers. \n");
		throw "Error: malloc failed to allocate buffers. \n";
	}
	//LogInfo("\tStart Addr = %p\n",buffer);

	reset();

}



#if 0
template <typename T>
cl_SVM_Buffer_2D_Flat<T>::cl_SVM_Buffer_2D_Flat(uint size0):
	//ocl(ocl),
	//mem_flags(mem_flags)
	size0(size0)
{
	if(!ocl)
		throw "You need a global ocl_args_d_t pointer!";

	//LogInfo("Creating a 2D array of size0 = %d\n",size0);

		cl_svm_mem_flags mem_flags = CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_SVM_ATOMICS;
		// the buffer should be aligned with 4K page and size should fit 64-byte cached line
		buffer_size = (((sizeof(T *) * size0) - 1) / 64 + 1) * 64;

		buffer = (T **)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);
	//buffer = (T **)malloc(sizeof(T *)*size0);

	if(NULL == buffer)
	{
		LogError("Error: malloc failed to allocate buffers. \n");
		throw "Error: malloc failed to allocate buffers. \n";
	}

	for(int i = 0; i < size0; i++)
	{
		buffer[i] = NULL;
	}

}
#endif

#if 1
template <typename T>
cl_SVM_Buffer_2D_Flat<T>::~cl_SVM_Buffer_2D_Flat()
{
	clSVMFree(ocl->context,(void *)buffer);

}
#endif
#if 0
template <typename T>
void cl_SVM_Buffer_2D_Flat<T>::alloc(int i, int size1)
{
	cl_svm_mem_flags mem_flags = CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_SVM_ATOMICS;;
	buffer_size = (((sizeof(T) * size1) - 1) / 64 + 1) * 64;

	buffer[i] = (T *)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);

	if (NULL == buffer[i])
	{
		LogError("Error: clSVMAlloc (2D) failed to allocate buffers. \n");
		throw "Error: clSVMAlloc (2D) failed to allocate buffers. \n";
	}

}
#endif
template <typename T>
void cl_SVM_Buffer_2D_Flat<T>::reset()
{
		if(buffer)
			memset((void *)buffer,0,buffer_size);
}

template <typename T>
void cl_SVM_Buffer_2D_Flat<T>::bind(cl_kernel kernel, int arg)
{
	//printf("Binding %s\n",label);
	CHECK_ERRORS(clSetKernelArgSVMPointer(kernel, arg, (void *)buffer));
}

template <typename T>
void cl_SVM_Buffer_2D_Flat<T>::bind(cl_kernel kernel, int arg,int i)
{
	CHECK_ERRORS(clSetKernelArgSVMPointer(kernel, arg, (void *)buffer[i]));
}

template <typename T>
void cl_SVM_Buffer_2D_Flat<T>::generateRandomData()
{
	srand(12345);
	T temp;

	for(int i = 0; i < size0; i++)
		for (int j = 0; j < size1; ++j)
		{
			temp = (T)(rand()%128);
			buffer[i][j] = temp;
		}

}

#if 0
template <typename T>
class cl_SVM_Buffer_2D
{
public:
	cl_SVM_Buffer_2D(uint size0, uint size1);
	cl_SVM_Buffer_2D(uint size0);
	~cl_SVM_Buffer_2D();

	//class T;
	cl_uint size0;
	cl_uint size1;

	void alloc(int i, int size1);

	//Reset value to 0
	void reset();

	//Set Kernel Args
	void bind(cl_kernel kernel, int arg);
	void bind(cl_kernel kernel, int arg, int i);

	//Generate Random Data
	void generateRandomData();

#if 1
	T *operator[](int i)
	{ return buffer[i];}

	//T operator()(int i, int j)
	//const { return buffer[i][j]; }

	//T& *operator[](int i)
	//{ return buffer[i];}
/*
	T operator[][](int i,int j)
	const { return buffer[i][j];}

	T& operator[][](int i,int j)
	{ return buffer[i][j];}
*/
	T **ptr()
	{
		return buffer;
	}

	T *ptr(int i)
	{
		return buffer[i];
	}

#endif

private:
	//ocl_args_d_t *ocl;
	uint buffer_size;
	//cl_svm_mem_flags mem_flags;
	T **buffer;
};

template <typename T>
cl_SVM_Buffer_2D<T>::cl_SVM_Buffer_2D(uint size0, uint size1):
	//ocl(ocl),
	//mem_flags(mem_flags)
	size0(size0),
	size1(size1)
{
	if(!ocl)
		throw "You need a global ocl_args_d_t pointer!";

		LogInfo("Creating a 2D array of size0 = %d | size1 = %d\n",size0,size1);

		cl_svm_mem_flags mem_flags = CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_SVM_ATOMICS;;
		// the buffer should be aligned with 4K page and size should fit 64-byte cached line
		buffer_size = (((sizeof(T *) * size0) - 1) / 64 + 1) * 64;

		buffer = (T **)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);
	//buffer = (T **)malloc(sizeof(T *)*size0);

	if(NULL == buffer)
	{
		LogError("Error: malloc failed to allocate buffers. \n");
		throw "Error: malloc failed to allocate buffers. \n";
	}


	buffer_size = (((sizeof(T) * size1) - 1) / 64 + 1) * 64;

	for(int i = 0; i < size0; i++)
	{
		buffer[i] = (T *)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);

		if (NULL == buffer[i])
		{
			LogError("Error: clSVMAlloc (2D) failed to allocate buffers. \n");
			throw "Error: clSVMAlloc (2D) failed to allocate buffers. \n";
		}
	}
	reset();

}

template <typename T>
cl_SVM_Buffer_2D<T>::cl_SVM_Buffer_2D(uint size0):
	//ocl(ocl),
	//mem_flags(mem_flags)
	size0(size0)
{
	if(!ocl)
		throw "You need a global ocl_args_d_t pointer!";

		LogInfo("Creating a 2D array of size0 = %d\n",size0);

		cl_svm_mem_flags mem_flags = CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_SVM_ATOMICS;;
		// the buffer should be aligned with 4K page and size should fit 64-byte cached line
		buffer_size = (((sizeof(T *) * size0) - 1) / 64 + 1) * 64;

		buffer = (T **)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);
	//buffer = (T **)malloc(sizeof(T *)*size0);

	if(NULL == buffer)
	{
		LogError("Error: malloc failed to allocate buffers. \n");
		throw "Error: malloc failed to allocate buffers. \n";
	}

	for(int i = 0; i < size0; i++)
	{
		buffer[i] = NULL;
	}

}

#if 1
template <typename T>
cl_SVM_Buffer_2D<T>::~cl_SVM_Buffer_2D()
{
	for(int i = 0; i < size0; i++)
		if(buffer[i])
			clSVMFree(ocl->context,(void *)buffer[i]);

	if(buffer)
		free(buffer);

}
#endif

template <typename T>
void cl_SVM_Buffer_2D<T>::alloc(int i, int size1)
{
	cl_svm_mem_flags mem_flags = CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_SVM_ATOMICS;;
	buffer_size = (((sizeof(T) * size1) - 1) / 64 + 1) * 64;

	buffer[i] = (T *)clSVMAlloc(ocl->context,mem_flags, buffer_size, 0);

	if (NULL == buffer[i])
	{
		LogError("Error: clSVMAlloc (2D) failed to allocate buffers. \n");
		throw "Error: clSVMAlloc (2D) failed to allocate buffers. \n";
	}

}

template <typename T>
void cl_SVM_Buffer_2D<T>::reset()
{
	for(int i = 0; i < size0; i++)
		if(buffer[i])
			memset((void *)buffer[i],0,buffer_size);
}

template <typename T>
void cl_SVM_Buffer_2D<T>::bind(cl_kernel kernel, int arg)
{
	CHECK_ERRORS(clSetKernelArgSVMPointer(kernel, arg, (void *)buffer));
}

template <typename T>
void cl_SVM_Buffer_2D<T>::bind(cl_kernel kernel, int arg,int i)
{
	CHECK_ERRORS(clSetKernelArgSVMPointer(kernel, arg, (void *)buffer[i]));
}

template <typename T>
void cl_SVM_Buffer_2D<T>::generateRandomData()
{
	srand(12345);
	T temp;

	for(int i = 0; i < size0; i++)
		for (int j = 0; j < size1; ++j)
		{
			temp = (T)(rand()%128);
			buffer[i][j] = temp;
		}

}
#endif
