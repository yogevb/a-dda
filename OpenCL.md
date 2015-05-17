

[OpenCL](http://www.khronos.org/opencl/) is a framework for programming highly parallel devices like graphic cards (GPUs) and mutlicore CPUs. The implementation works on modern GPUs at current state. The main limitation is the required support of double precision floating point operations.

# Setup of the environment #

## Recent GPU driver ##

There are two options:
  * Download and install it from the corresponding vendor ([Nvidia](http://www.nvidia.com/page/drivers.html) or [AMD](http://support.amd.com/us/gpudownload)).
  * On Unix systems driver packages may be available. It is recommended to use them, since they are more convenient to install and update. Also they cause less trouble with kernel updates. The drawback is a delay in updates relative to original vendor drivers. Moreover, for some systems there are no official packages, since the driver is not open-source. In this case search in non-official repositories, like [RPM Fusion](http://rpmfusion.org/). On Ubuntu search for packages [fglrx-updates](http://packages.ubuntu.com/search?suite=default&section=all&arch=any&keywords=fglrx-updates) (AMD) or [nvidia-###-updates](http://packages.ubuntu.com/search?suite=default&section=all&arch=any&keywords=nvidia+updates).

The recent driver alone is sufficient to run `ADDA` OpenCL executables, e.g. those that are [provided for Windows](http://code.google.com/p/a-dda/wiki/PackageDescription#Windows_executables).

## Headers and libraries ##

These are required to compile OpenCL version of `ADDA`. There are several _alternative_ approaches:
  * On Unix a special package may be available, e.g. [ocl-icd-opencl-dev](http://packages.ubuntu.com/search?keywords=ocl-icd-opencl-dev) for Ubuntu. It is based on [open-source implementation](https://forge.imag.fr/projects/ocl-icd/) of `libOpenCL.so`, which further links to vendor-specific libraries.
  * Install full SDK from the corresponding vendor ([Nvidia CUDA toolkit](http://developer.nvidia.com/cuda-toolkit-sdk) or [AMD APP SDK](http://developer.amd.com/sdks/AMDAPPSDK/Pages/default.aspx)). Additionally to headers and libraries you will get some nice development tools, such as visual profiler. We have, however, encountered several problems with this approach:
    * Intel compiler (version 11.0) has problems with OpenCL headers from CUDA toolkit 3.2. This was fixed in later versions of the toolkit.
    * `OpenCL.lib` provided by both CUDA toolkit and AMD APP SDK on 64-bit Windows is incompatible with `MinGW64`. Compilation goes fine but `ADDA` breaks down as soon as it calls the first OpenCL function. This is probably due to some limitations of `MinGW64` itself. Fortunately, AMD APP SDK also provides `libOpenCL.a`, which links and works fine. To compile `ADDA` on 64-bit Windows with Nvidia GPUs one should either use `libOpenCL.a` from AMD APP SDK or resort to a manual approach described below.
  * Supply missing headers and/or libraries manually
    * Since OpenCL is an open standard, the headers can be obtained from the [official website](http://www.khronos.org/registry/cl/). Ideally, one should use the same version of OpenCL headers as that of library. They may also be available as a package, e.g. [opencl-headers](http://packages.ubuntu.com/search?keywords=opencl-headers)
    * Installed GPU driver always provides libraries (DLLs), which are used at runtime. On Unix systems compiler may link to this libraries directly. Similarly, on Windows GCC can link to `OpenCL.dll` (located in `C:\Windows\System32`). However, on 64-bit Windows it is obligatory to use 64-bit application (Windows Explorer or a file manager) to move this dll to non-system folder, because Windows will supply 32-bit instead of 64-bit version of `OpenCL.dll` to any 32-bit application (such as the compiler).

## Include and library paths ##

If you obtain headers and libraries in a package (on Unix), the latter should also set the paths appropriately (to make them available to the compiler). Otherwise, you should either
  * set these paths yourself:
    * _on Unix_ add headers location to environmental variables `C_INCLUDE_PATH` and `CPLUS_INCLUDE_PATH` and libraries location - to `LIBRARY_PATH` and `LD_LIBRARY_PATH` (for linking and runtime). Setting `LD_LIBRARY_PATH` can be replaced by modifying `/etc/ld.so.conf`, which sometimes is done automatically during driver installation.
    * _on Windows_ copy contents of corresponding folders to `include` and `lib` folders of the [MinGW/MSYS environment](InstallingMinGW#Advanced_options.md).
  * or specify paths to corresponding `include` and `lib` folders in file `src/ocl/Makefile`, as described in [CompilingADDA](CompilingADDA.md).

# Implementation #

In general the whole matrix-vector multiplication is done on the GPU.
For details and benchmarks see [J. Comput. Sci. 2, 262-271 (2011)](http://dx.doi.org/10.1016/j.jocs.2011.05.011). Currently, `ADDA` on high-end gaming GPUs outperform that on high-end processors at least several times (sometimes by a factor of 10 or more). And this factor is significantly limited by relatively slow double-precision computations on current gaming GPUs and very modest performance of OpenCL FFT routines (compared to sophisticated `FFTW3`). Recently, we switched (in the default mode) to [clAmdFft](InstallingclAmdFft.md), which further increased speed a few times.

Although OpenCL version of `ADDA` is fully operational, there are a number of [limitations](http://code.google.com/p/a-dda/issues/list?q=label:OpenCL). In particular, the maximum problem size is limited by the available GPU memory, more specifically, by its part available to `ADDA`.