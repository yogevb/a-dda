

`clAmdFft` library provides the default way to perform the FFTs in OpenCL version of `ADDA`. This library already significantly outperforms the built-in Apple clFFT routines and is expected to develop further. Therefore, its use on any system with GPU is highly recommended. The following guidelines are based on `clAmdFft` version 1.10.274 (or 321).

# Unix #

  * [Download](http://developer.amd.com/tools-and-sdks/opencl-zone/opencl-libraries/amd-accelerated-parallel-processing-math-libraries/) the package of `clAmdFft` for Linux and install it. The default installation directory is `/opt/clAmdFft-<version>`. It is also possible to specify any other directory, for example, inside the home directory of a particular user.
  * Either
    * specify paths to `include` and `lib64` (or `lib32`) subdirectories of the install directory in file `src/ocl/Makefile`, as described in [CompilingADDA](CompilingADDA.md).
    * or add `.../include` to environmental variable `C_INCLUDE_PATH` and `.../lib64` (or `lib32`) - to `LIBRARY_PATH` and `LD_LIBRARY_PATH` (for linking and runtime). Setting `LD_LIBRARY_PATH` can be replaced by modifying `/etc/ld.so.conf` or by sourcing `appmlEnv.sh` in the installation directory.
  * There is a known bug when using `clAmdFft` together with `libOpenCL.so` provided by Nvidia GPU driver. During linking spurious errors appear, like:
```
...libclAmdFft.Runtime.so: undefined reference to `cl...@OPENCL_1.0'
```
> It has been discussed in a few places ([1](http://devgurus.amd.com/thread/159586), [2](http://devgurus.amd.com/thread/158982), [3](http://devgurus.amd.com/thread/157052)). Currently, the simplest workaround seems to uncomment the line
```
LDFLAGS +=-Wl,--unresolved-symbols=ignore-in-shared-libs
```
> in `src/ocl/Makefile`. On Unix it is even better to switch to open-source implementation of `libOpenCL.so` (see [OpenCL#Headers\_and\_libraries](OpenCL#Headers_and_libraries.md))
  * Also on Unix `clAmdFft` may need pthread library, which is not linked by default. One way to fix this problem is to uncomment the following line in `src/ocl/Makefile`.
```
LDLIBS += -lpthread
```

# Windows #

If you just want to use [ADDA executables for Windows](PackageDescription#Windows_executables.md), do not worry about `clAmdFft` at all. An appropriate DLL is included in the corresponding package.

If you want to compile `adda_ocl` on Windows yourself, proceed further:
  * [Download](http://developer.amd.com/tools/heterogeneous-computing/amd-accelerated-parallel-processing-math-libraries/) the installer of `clAmdFft` for Windows and install it. The default installation directory is `C:\Program Files (x86)\AMD\clAmdFft` or `C:\Program Files\AMD\clAmdFft`.
  * Add `bin64` or `bin32` (depending on the Windows bitness) subdirectory in the installation directory to Windows environmental variable `PATH`
  * Either
    * specify paths to `include` and `bin64` (or `bin32`) subdirectories of the install directory in file `src/ocl/Makefile`, as described in [CompilingADDA](CompilingADDA.md). The compiler will link directly to `*.dll` (instead of `*.lib`).
    * or copy contents of `include` folder and `clAmdFft.Runtime.dll` of `bin` folder to corresponding `include` and `lib` folders of the [MinGW/MSYS environment](InstallingMinGW#Advanced_options.md).