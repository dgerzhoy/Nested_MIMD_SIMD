# Nested_MIMD_SIMD

This codebase provides two examples of Nested MIMD-SIMD Parallelism as described in:

Gerzhoy, D., Sun, X., Zuzak, M., and Yeung, D. "Nested MIMD-SIMD Parallelization for Heterogeneous Microprocessors." ACM Transactions on Architecture and Code Optimization. Published December 2019.

The two example benchmarks are MD and FFT6 from the openmp source code repository.

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
custom (per benchmark) class