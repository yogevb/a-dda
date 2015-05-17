

# Basic installation #
To be able to compile `ADDA` on Windows you need to install `MSYS` and `MinGW`.

[MSYS](http://www.mingw.org/wiki/MSYS) is a collection of 32-bit Unix system utils, such as `make`, which work fine with both 32- and 64-bit Windows. To install it do the following (based on [MinGW instructions](http://mingw.org/wiki/Getting_Started)):
  * Download the latest version of the [mingw-get](http://sourceforge.net/projects/mingw/files/Installer/mingw-get/) installer and extract the files to the `C:\MinGW` folder. Other paths can also be used.
  * Open command prompt and run
```
cd C:\MinGW\bin
mingw-get install msys-base msys-coreutils-ext
```
> The latter part of `MSYS` (`msys-coreutils-ext`) is not required for `ADDA` compilation, but may be needed for executing Bash scripts, e.g. `tests/2exec/comp2exec`.
  * Add `C:\MinGW\MSYS\1.0\bin` to Windows environmental variable `PATH`.

Unfortunately the original [MinGW project](http://www.mingw.org) currently targets only 32-bit Windows. Therefore installation instructions for 32- and 64-bit are different.

> ## 32-bit Windows ##
> Continuing along the [MinGW instructions](http://mingw.org/wiki/Getting_Started) use the `mingw-get` described above:
    * Open command prompt and run
```
cd C:\MinGW\bin
mingw-get install gcc fortran c++
```
    * Add `C:\MinGW\bin` and to Windows environmental variable `PATH`.

> ## 64-bit Windows ##
> A 64-bit alternative to the `MinGW` is [MinGW-w64 project](http://mingw-w64.sourceforge.net/). To make installation easier it is recommended to use [TDM-GCC interface](http://tdm-gcc.tdragon.net/). You need only to download and run an [installer](http://sourceforge.net/projects/tdm-gcc/files/TDM-GCC%20Installer/tdm-gcc-webdl.exe/download) and specify the packages, you want to install. The default installation includes `gcc`, `g++`, and `gdb`. You need to also to check "Components -> gcc -> Fortran" to install `gfortran`. The installer will take care of everything else, including modification of `PATH` environmental variable. By default, the installation directory is `C:\TDM-GCC-64`.

Now you are ready to compile `ADDA`. The only drawback of the concise procedure, described above, is that you need to manually specify include and library paths for external packages, such as [FFTW 3](InstallingFFTW3.md), in makefiles.

# Advanced options #
These options may be of interest to developers or to those, who plan to compile `ADDA` many times with different options. These steps should be performed after the basic steps, described above.

To avoid need to manually  specify include and library paths for external packages in makefiles, do the following (based on [MinGW instructions](http://www.mingw.org/wiki/SpecsFileHOWTO)):
  * Copy all headers and libraries of external packages (such as [FFTW 3](InstallingFFTW3.md), [MPI](InstallingMPI.md), [OpenCL](OpenCL.md), [clAmdFft](InstallingclAmdFft.md), or [clAmdBlas](InstallingclAmdBlas.md)) to `C:\MinGW\MSYS\1.0\local\include` and `C:\MinGW\MSYS\1.0\local\lib` respectively (creating these folders if they do not exist).
  * Locate the library folder of your gcc installation, its path (`<gcclib-path>`) should be one of the following
```
C:/MinGW/lib/gcc/mingw32/<gcc-version>
C:/TDM-GCC-64/lib/gcc/x86_64-w64-mingw32/<gcc-version>
```
> for 32- and 64-bit installations (described above), respectively. Here `<gcc-version>` is the installed version of `gcc`, e.g., `4.5.2`.
  * Look for file `specs` in folder `<gcclib-path>`. If it does not exist, execute
```
gcc -dumpspecs > <gcclib-path>/specs
```
  * Modify `specs` file:
    * Add the following to the beginning:
```
*local_prefix:
c:/MinGW/msys/1.0/local/

*local_includes:
-I%(local_prefix)include

*local_lib_search:
-L%(local_prefix)lib
```
    * Add `%(local_includes)` to end of definition for `cpp`, changing it to, e.g.,
```
*cpp:
%{posix:-D_POSIX_SOURCE} %{mthreads:-D_MT} %(local_includes)
```
    * Add `%(local_includes)` to end of definition for `cc1plus`, changing it to, e.g.,
```
*cc1plus:
%(local_includes)
```
    * Add `%(local_lib_search)` to end of definition for `link_libgcc`, changing it to, e.g.,
```
*link_libgcc:
%D %(local_lib_search)
```

To debug `ADDA`, `gdb` need to be installed. This is done automatically during the installation of TDM\_GCC for 64-bit Windows, described above. To install `gdb` in the `MinGW` environment for 32-bit Windows, run
```
mingw-get gdb
```

# Multilib configuration on Windows #
Below we describe additional steps to set up the environment on 64-bit Windows 7, which allows convenient building of both 32- and 64-bit `ADDA` executables. We also outline installation of [FFTW 3](InstallingFFTW3.md), [MPI](InstallingMPI.md), [OpenCL](OpenCL.md), [clAmdFft](InstallingclAmdFft.md), and [clAmdBlas](InstallingclAmdBlas.md) slightly differently than given in separate wiki pages. Such complicated environment is needed exclusively for producing Windows binaries for `ADDA` releases from a single machine.
  * Create folder `C:\MinGW\MSYS\1.0\local\lib32` (additionally to `lib` and `include`, which were already created above).

  * [Download](http://www.fftw.org/install/windows.html) 32- and 64-bit Windows packages of FFTW3. Then do the following, using 64-bit file manager with administrative privileges.
    * Extract `fftw3.h` from any of the packages and move it to `C:\MinGW\MSYS\1.0\local\include`.
    * Extract `libfftw3-3.dll` from 32-bit package and move it to `C:\Windows\SysWOW64`.
    * Extract `libfftw3-3.dll` from 64-bit package and move it to `C:\Windows\System32`.

  * [Download](http://www.mcs.anl.gov/research/projects/mpich2/downloads/index.php?s=downloads) both 32- and 64-bit Windows installers of MPICH2. First, install 32-bit version, then - 64-bit one. Then follow [the standard steps](InstallingMPI#Windows.md) to set up `spmd` and `PATH` for the 64-bit version. But first install 32-bit `smpd`, running it from `C:\Program Files (x86)\MPICH2\bin`, and then 64-bit `smpd`. The latter will remove the 32-bit service. It seems that both versions of `MPICH2` coexist fine together. Moreover, 32-bit `adda_mpi` works fine with 64-bit `mpiexec` and `smpd`. The only problem is that `MPICH2` headers slightly differ between the two packages. It is not clear, whether this difference is significant, so below we substitute proper headers for each compilation mode.

  * Create folder `C:\MinGW\MSYS\1.0\local\include\CL` and [download](http://www.khronos.org/registry/cl/) into it OpenCL headers (`*.h`) of the version, supported by GPU drivers (e.g., version 1.1 for Nvidia drivers 301.42).

  * [Download](http://developer.amd.com/tools/heterogeneous-computing/amd-accelerated-parallel-processing-math-libraries/) the installer of `clAmdFft` for Windows and install it to the default directory.

  * [Download](http://developer.amd.com/tools/heterogeneous-computing/amd-accelerated-parallel-processing-math-libraries/) the installer of `clAmdBlas` for Windows and install it to the default directory.

  * Create symbolic links (with administrative privileges):
```
cd C:\MinGW\MSYS\1.0\local
mklink lib32\libfftw3.dll C:\Windows\SysWOW64\libfftw3-3.dll
mklink lib\libfftw3.dll C:\Windows\System32\libfftw3-3.dll
mklink /D include\mpich2 "C:\Program Files\MPICH2\include"
mklink /D include\mpich2_32 "C:\Program Files (x86)\MPICH2\include"
mklink /D lib\mpich2 "C:\Program Files\MPICH2\lib"
mklink /D lib32\mpich2 "C:\Program Files (x86)\MPICH2\lib"
mklink lib32\OpenCL.dll C:\Windows\SysWOW64\OpenCL.dll
mklink lib\OpenCL.dll C:\Windows\System32\OpenCL.dll
mklink /D include\clAmdFft "C:\Program Files (x86)\AMD\clAmdFft\include\"
mklink lib32\clAmdFft.Runtime.dll "C:\Program Files (x86)\AMD\clAmdFft\bin32\clAmdFft.Runtime.dll"
mklink lib\clAmdFft.Runtime.dll "C:\Program Files (x86)\AMD\clAmdFft\bin64\clAmdFft.Runtime.dll"
mklink C:\Windows\SysWOW64\clAmdFft.Runtime.dll "C:\Program Files (x86)\AMD\clAmdFft\bin32\clAmdFft.Runtime.dll"
mklink C:\Windows\System32\clAmdFft.Runtime.dll "C:\Program Files (x86)\AMD\clAmdFft\bin64\clAmdFft.Runtime.dll"
mklink /D include\clAmdBlas "C:\Program Files (x86)\AMD\clAmdBlas\include\"
mklink lib32\clAmdBlas.dll "C:\Program Files (x86)\AMD\clAmdBlas\bin32\clAmdBlas.dll"
mklink lib\clAmdBlas.dll "C:\Program Files (x86)\AMD\clAmdBlas\bin64\clAmdBlas.dll"
mklink C:\Windows\SysWOW64\clAmdBlas.dll "C:\Program Files (x86)\AMD\clAmdBlas\bin32\clAmdBlas.dll"
mklink C:\Windows\System32\clAmdBlas.dll "C:\Program Files (x86)\AMD\clAmdBlas\bin64\clAmdBlas.dll"
```

  * Additionally to modification of file `specs`, described in previous section, change the added head of this file to
```
*local_prefix:
c:/MinGW/msys/1.0/local/

*local_incl:
%(local_prefix)include

*local_includes:
-I%(local_incl) -I%(local_incl)/mpich2%{m32:_32} -I%(local_incl)/clAmdFft -I%(local_incl)/clAmdBlas

*local_lib:
%(local_prefix)lib%{m32:32}

*local_lib_search:
-L%(local_lib) -L%(local_lib)/mpich2
```

After the above setup, 64-bit version of `ADDA` can be compiled by running `make ...` out of box. For 32-bit version, add `EXTRA_FLAGS=-m32` to the invocation of `make`.