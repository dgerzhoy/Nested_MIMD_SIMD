# Nested_MIMD_SIMD

This codebase provides two examples of Nested MIMD-SIMD Parallelism as described in:

Gerzhoy, D., Sun, X., Zuzak, M., and Yeung, D. "Nested MIMD-SIMD Parallelization for Heterogeneous Microprocessors." ACM Transactions on Architecture and Code Optimization. Published December 2019.

The two example benchmarks are MD and FFT6 from the openmp source code repository.

These benchmarks are edited to launch their inner loops as kernels onto the GPU.

In order to have a low-latency kernel launch, the GPU launch mechanism for each benchmark is adapted from:

Michael Mrozek and Zbigniew Zdanowicz. 2016. GPU daemon: Road to zero cost submission. In Proceedings of the
4th International Workshop on OpenCL.

Rather than launching an OpenCL kernel every time we want to do work on the GPU. A "daemon" kernel is launched when the benchmark begins that polls a communication buffer, waiting to be given work. When we want to do work on the GPU, we write which threadblocks we want to execute into the communication buffer, and the daemon kernel executes the work.

# Hardware Requirements

CPU with Integrated GPU (code is written for Intel, but AMD should work as well with some edits) with OpenCL 2.0 capability.

OpenCL Shared Virtual memory (SVM) with "Fine-Grain SVM System" for atomics is required.

The specific SDK you install depends highly on your system.

# File Descriptions

## Common Directory
    clBuffer.h
        This file provides a templates for interacting with SVM buffers in a convenient way.
        Simplifies accessing elements in 1D and 2D arrays, and provides interface for binding arguments to kernels.
    clSetup.[h|cpp]
        clSetup provides the means for starting OpenCL, selecting a platform (your hardware and installed openCL SDK), 
        allocating queues, compiling kernels, and important global buffers.
    utils.[h|cpp]
        Provides some utility functions including file IO for the kernel files, and timers for recording performance.

## Benchmark Directory

Each benchmark contains not only the normal files the benchmark is implemented in (e.g. MD.cpp, fft6.cpp) but also a
custom (per benchmark) structure CL_Buffers_t defined in the commBuffer.[h|cpp] files.

    commBuffer.[h|cpp] 
        CL_Buffers_t is used first to hold/manage SVM pointers.
            Any parameters to the GPU kernel must be SVM pointers, so that they can be updated any time a low-latency launch is desired.

        Some benchmark specific non-GPU arrays and variables are also kept in this structure for convinience.

        The structure also defines several functions that setup the low-latency launch system (daemon), and schedule (launch) kernels to it.
    
    Kernels.cl
        This file contains the GPU launch daemon, which is a wrapper around the actual GPU kernel we wish to execute on the GPU.

# Compiling

cd << benchmark_directory >>
make

# Running

Each benchmark directory has a runall.sh script that shoes how to run the kernel.