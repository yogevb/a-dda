The following description is for latest stable version of the released packages, which can be found at [Downloads](Downloads.md). Alternatively, the most recent (potentially unstable) versions of the constituent files may be extracted directly from the [repository](http://code.google.com/p/a-dda/source/browse/trunk/).


# Source #
`adda_1.2.zip`
  * `devtools/` - tools used by `ADDA` developers
    * `default.svnprops` - default Subversion properties (to be used by TortoiseSVN)
    * `make_branch` - script to make new branch in Subversion repository
    * `make_tag` - script to make new tag in Subversion repository
    * `release_sequence.txt` - a sequence of tasks to be made during release
    * `versions.txt` - note on files that need to be checked prior to release (e.g. contain version number)
    * `win_all.sh` - script to perform all Windows-specific tasks during release
    * `win_build_adda.sh` - script to build `ADDA` Windows binaries and copy to `win32/` or `win64/`
    * `win_build_misc.sh` - script to build `misc/` Windows binaries and copy to `win32/` or `win64/`
    * `win_commit.sh` - script to commit `doc/`, `sample/`, `win32/`, and `win64/`
    * `zip_packages` - script to checkout and zip packages on Unix system
  * `doc/` – documentation
    * `doxy/Doxyfile` - config file for [doxygen](http://www.doxygen.org) to automatically produce code documentation
    * `gv/` - sources for schemes used in CodeDesign, they are processed into svg files using [GraphViz](http://www.graphviz.org/)
    * `copyleft` – GNU General Public License
    * `history` – complete history of `ADDA` development
    * `manual.doc` – source of the manual in MS Word format
    * `manual.pdf` – the manual in PDF format
  * `input/` – default input files
    * `tables/` – 10 auxiliary files with tables of integrals
    * `alldir_params.dat` – parameters for integral scattering quantities
    * `avg_params.dat` – parameters for orientation averaging
    * `scat_params.dat` – parameters for grid of scattering angles
  * `misc/` – additional files, relatively independent from the main part of `ADDA`
    * `hyperfun/` - tool chain to create shape files for `ADDA` using Hyperfun ([description](http://a-dda.googlecode.com/svn/trunk/misc/hyperfun/README))
    * `near_field/` - package to calculate near field using internal fields produced by `ADDA` ([description](http://a-dda.googlecode.com/svn/trunk/misc/near_field/nearfield_manual.txt))
    * `parse_log/` - Mathematica package for parsing `ADDA` log files ([description](http://a-dda.googlecode.com/svn/trunk/misc/parse_log/README))
    * `pip/` - package "Point in Polyhedron" to transform `.obj` files into shape format readable by `ADDA` ([description](http://a-dda.googlecode.com/svn/trunk/misc/pip/README))
  * `sample/` – sample output and other files
    * `run000_sphere_g16_m1.5/` – sample output directory
      * `log` - log of sample simulation
      * `mueller` - sample angle-resolved Mueller matrix
      * `CrossSec-Y` - sample cross sections
    * `test.pbs` – sample PBS script for MPI system
    * `test.sge` – sample SGE script for MPI system
    * `stdout` – standard output of a sample simulation
  * `src/` - source files and makefiles
    * `cpp/` - C++ sources
      * `clFFT.h`, `fft_base_kernels.h`, `fft_execute.cpp`, `fft_internal.h`, `fft_kernelstring.cpp`, `fft_setup.cpp` - source and header files for Apple clFFT
    * `fort/` - Fortran sources
      * `cfft99D.f` - source file for Temperton FFT
      * `propaesplibreintadda.f`, `d07hre.f`, `d09hre.f`, `d113re.f`, `d132re.f`, `dadhre.f`, `dchhre.f`, `dcuhre.f`, `dfshre.f`, `dinhre.f`, `drlhre.f`, `dtrhre.f` - subroutine to compute IGT and auxiliary numerical routines.
    * `mpi/` - holds all files, produced during compilation of MPI version of `ADDA`
      * `Makefile` - makefile for MPI version (called from the main `Makefile`)
    * `ocl/` - holds all files, produced during compilation of OpenCL version of `ADDA`
      * `Makefile` - makefile for OpenCL version (called from the main `Makefile`)
    * `seq/` - holds all files, produced during compilation of sequential version of `ADDA`
      * `Makefile` - makefile for sequential version (called from the main `Makefile`)
    * `Makefile` – main makefile
    * `common.mk` - common part of child makefiles, including all compilation directives.
    * `ADDAmain.c`, `CalculateE.c`, `calculator.c`, `chebyshev.c`, `cmplx.h`, `comm.c/h`, `const.h`, `crosssec.c/h`, `debug.c/h`, `fft.c/h`, `function.h`, `GenerateB.c`, `interaction.c/h`, `io.c/h`, `iterative.c`, `linalg.c/h`, `make_particle.c`, `matvec.c`, `memory.c/h`, `oclcore.c/h`, `os.h`, `param.c/h`, `parbas.h`, `prec_time.c/h`, `Romberg.c/h`, `sinint.c`, `sparse_ops.h`, `timing.c/h`, `types.h`, `vars.c/h` – C source and header files of `ADDA` (see CodeDesign)
    * `oclkernels.cl` - OpenCL kernels
    * `mt19937ar.c/h` – source and header files for Mersenne twister random generator
    * `updsvnrev.sh` - script to obtain revision number of the source and store it into generated `svnrev.h`
  * `tests/` - automatic tools to test `ADDA` executables
    * `2exec/` - extensive test of two `ADDA` executables (in terms of output) against each other
      * `comp2exec` - main script to run the test
      * `diff_numeric.awk` - script to compare two files with given numerical tolerance
      * `README` - a short description
      * `suite` - the default test suite
      * `suite_sparse` - shortened suite for testing sparse mode
      * ... - many sample input files to be used in tests

# Windows executables #
We provide executables for 32-bit (IA32) and 64-bit (x86\_64) Windows. They should be downloaded together with the source package (in particular, the latter contains sample input files).

`adda_1.2_win32.zip`.
  * `win32/`
    * `misc/` - executables for miscellaneous packages
      * ...
    * `adda.exe` - executable of sequential version of `ADDA`
    * `adda_mpi.exe` - executable of MPI version of `ADDA`
    * `adda_ocl.exe` - executable of OpenCL version of `ADDA`
    * `adda_spa.exe` - executable of sequential sparse version of `ADDA`
    * `adda_spa_mpi.exe` - executable of MPI sparse version of `ADDA`
    * `clAmdFft.Runtime.dll` - DLL for `clAmdFft` 1.10
    * `libfftw3-3.dll` - DLL for `FFTW` 3.3
    * `libgcc_s_dw2-1.dll`, `libgfortran-3.dll`, `libquadmath-0.dll` -  DLLs for Fortran parts of `ADDA` and `misc/` packages
    * `README` - short description of the package

`adda_1.2_win64.zip`.
  * `win64/`
    * `misc/` - executables for miscellaneous packages
      * ...
    * `adda.exe` - executable of sequential version of `ADDA`
    * `adda_mpi.exe` - executable of MPI version of `ADDA`
    * `adda_ocl.exe` - executable of OpenCL version of `ADDA`
    * `adda_spa.exe` - executable of sequential sparse version of `ADDA`
    * `adda_spa_mpi.exe` - executable of MPI sparse version of `ADDA`
    * `clAmdFft.Runtime.dll` - DLL for `clAmdFft` 1.10
    * `libfftw3-3.dll` - DLL for `FFTW` 3.3
    * `libgfortran_64-3.dll`, `libquadmath_64-0.dll` -  DLLs for Fortran parts of `ADDA` and `misc/` packages
    * `README` - short description of the package