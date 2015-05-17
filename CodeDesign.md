Modern software tools, like [doxygen](http://www.doxygen.org), allow automatic generation of docs, call graphs, etc. from the source. Thus, we supply the corresponding config file - `doc/doxy/Doxyfile`. Executing `doxygen` in `doc/doxy/` should produce a folder `html` with a lot of files. The first file in alphabetical order in this folder (`_a_d_d_amain_8c.html`), will be the head file for docs. We also plan to extend this feature in the future ([issue 165](https://code.google.com/p/a-dda/issues/detail?id=165)). `Doxyfile` is set to use `dot` from [GraphViz package](http://www.graphviz.org/) to produce beautiful call and include graphs. Thus, it is recommended to install `GraphViz` before running `doxygen`.

In the following we provide a relatively short description of the code illustrated by schemes created with `dot`. Bold red lines denote the main execution flow corresponding to sequential execution of functions, while blue lines correspond to the call dependence (i.e. one function is called during the execution of another one). Dashed lines denote alternative routes and calls, e.g. those that are relevant only for MPI or OpenCL versions. Different functions are denoted by white boxes, grouped by source (or header) files. Gold box denote the main (enclosing) function for the corresponding block, while cyan box denote a function which is described extensively on another (usually next) scheme. The sources for these schemes are available as `.gv` files in `doc/gv`.



# main #
![http://a-dda.googlecode.com/svn/wiki/main.svg](http://a-dda.googlecode.com/svn/wiki/main.svg)

The structure of the `main` function is rather complicated but can be divided into the following major parts:
  * Initialization of timing, MPI, and internal variables (`StartTime`, `InitComm`, and `InitVariables`).
  * Parsing of the command line options (`ParseParameters` and `VariablesInterconnect`), including initialization of rotation averaging and incident beam.
  * Initialization of shape (`InitShape` and `FinalizeSymmetry`), log file (`DirectoryLog`), and FFT grid (`ParSetup`).
  * Generation of particle shape (`MakeParticle`), optionally save it to a file, and log all simulation parameters (`PrintInfo`).
  * Main simulation part ([Calculator](CodeDesign#Calculator.md)).
  * Timing and other statistics (`FinalStatistics`) and exit (`Stop`).

# Calculator #
![http://a-dda.googlecode.com/svn/wiki/calc.svg](http://a-dda.googlecode.com/svn/wiki/calc.svg)

`Calculator` starts with initialization of OpenCL (`oclinit`), dipole-interaction function (`InitInteraction`) including reading of precomputed tables, Fourier transform of interaction matrix (`i111nitDmatrix`), and all large vectors (`AllocateEverything`). `initDmatrix` is relevant only for the FFT mode, it performs initialization of [FFT routines](CodeDesign#FFT_routines.md) and calls dipole-interaction function, which in turn may call one of the specialized routines. In MPI mode `InitDmatrix` also employs all-to-all communication (`BlockTranspose_Dm`) during the FFT. The main calculation part is done in [calculate\_one\_orientation](CodeDesign#calculate_one_orientation.md), which is called either directly or through the `Romberg2D` to perform orientation averaging. In the latter case additional function `SaveMuellerAndCS` is called afterwards. The function is finalized by freeing all the memory (`FreeEverything`).

# calculate\_one\_orientation #
![http://a-dda.googlecode.com/svn/wiki/oneori.svg](http://a-dda.googlecode.com/svn/wiki/oneori.svg)

`calculate_one_orientation` initializes coupling constants (dipole polarizabilities), corresponding to the main diagonal of the interaction matrix (`InitCC`), and rotates reference frame of the incident beam relative to the particle (`InitRotation`, only for orientation averaging). Then [CalculateE](CodeDesign#CalculateE.md) is executed for two polarizations of the incident field, and the Mueller matrix is calculated for a set of scattering angles (`MuellerMatrix`, optionally integrated over the azimuthal angle).

# CalculateE #
![http://a-dda.googlecode.com/svn/wiki/calcE.svg](http://a-dda.googlecode.com/svn/wiki/calcE.svg)

`CalculateE` starts with generation of incident beam on dipole locations (`GenerateB`), optionally reading it from file, then [IterativeSolver](CodeDesign#IterativeSolver.md) is run, followed by post-processing. The latter starts with calculation of the scattered fields in one plane (`CalcEplane`), on a dense grid over the whole solid angle (`CalcAlldir`), and/or on the user-specified grid (`CalcScatGrid`). Relevant functions are used when needed, all of them are wrappers around `CalcField`. This is followed by calculation of integral scattering quantities (`CalcIntegralScatQuantities`), some of which (scattering cross sections and asymmetry vector) are based on integration of scattered fields calculated during `CalcAlldir`. Radiation forces may be calculated by function `Frp_mat`, which currently requires extensive communication between processors in MPI mode (`AllGather`). Finally, incident beam, internal fields and/or dipole polarizations may be saved.

# IterativeSolver #
![http://a-dda.googlecode.com/svn/wiki/iter.svg](http://a-dda.googlecode.com/svn/wiki/iter.svg)

`IterativeSolver` is devoted to the solution of the large system of linear equations, central to the DDA. It starts with the calculation of the initial field (`CalcInitField`), ranging from a trivial zero field to the result of WKB approximation or the one read from file. Then one of the iterative solver is run (denoted by `iterFunc` on the scheme), in three consecutive stages: definition of variables (`PHASE_VARS`), initialization (`PHASE_INIT`), and loop of iterations (`PHASE_ITER`) until sufficiently small residual is obtained. Each iteration is followed by printing out the residual value (`ProgressReport`). When checkpoints are used, loading of checkpoint (`LoadIterChpoint`) may replace calculation of the initial field, and saving of checkpoints occurs after certain iterations (`SaveIterChpoint`). The interaction matrix is accessed from several functions, but only through matrix-vector product ([MatVec](CodeDesign#MatVec.md)). Moreover, `IterativeSolver` heavily uses linear-algebra functions for large vectors from `linalg.c/h`.

# MatVec #
![http://a-dda.googlecode.com/svn/wiki/matvec.svg](http://a-dda.googlecode.com/svn/wiki/matvec.svg)

`MatVec` contains two independent execution routes: for FFT and sparse versions. FFT route consists of forward 3D FFT of input vector (denoted by argument -1), element-wise product of the vector with the Fourier-transform of the interaction matrix (see `initDmatrix` above), and backward 3D FFT (denoted by argument +1). These FFTs are decomposed into 1D FFTs (see
[FFT routines](CodeDesign#FFT_routines.md)) with appropriate transpose operations. See ([Huntemann 2011](http://dx.doi.org/10.1016/j.jocs.2011.05.011)) for more detailed description of this algorithm. MPI mode requires additional all-to-all communications (`BlockTranspose`).

In sparse mode, `MatVec` consists in direct calculation of matrix-vector product (`AijProd`) with the elements of the interaction matrix being computed on the go. In MPI mode preliminary collection of the argument vector is performed (`AllGather`).

# FFT routines #
![http://a-dda.googlecode.com/svn/wiki/fft.svg](http://a-dda.googlecode.com/svn/wiki/fft.svg)

FFTs are performed during `initDmatrix` and during each call to `MatVec` (when not in sparse mode). Currently, ADDA may use one of the two CPU implementations: `FFTW3` (`fftw3.h`) or Temperton FFT (`cfft99D.f`), and one of the two GPU (OpenCL) implementations: `clAmdFft` (`clAmdFft.h`) or Apple FFT. `FFTW3` and `clAmdFft` are external packages, requiring separate [installation](CompilingADDA.md), while other two are supplied in the main `ADDA` package. The particular FFT implementation is chosen at compile time. However, the FFT of interaction matrix (`initDmatrix`) can currently be performed only using a CPU implementation.

Overall, the usage of FFT routines is very similar: initialization including creation of "plans" is done once followed by multiple execution of these plans for the fixed setting. There is, however, a certain difference in sophistication of plans between different implementations.

# Auxiliary functions #
The following describes common auxiliary functions, headers, etc. which are used throughout the code (in alphabetical order):
  * `cmplx.h` - inline functions for complex arithmetics and operations on 3D vectors and matrices
  * `comm.c/h` - all MPI functionality (major functions are explicitly mentioned above)
  * `const.h` - main header file, containing a lot of constants (including version number) and a few macros
  * `debug.c/h` - simple debug functionality, used when compiled with `DEBUGFULL`
  * `function.h` - function attributes
  * `io.c/h` - IO functionality, including working with files and processing of messages and errors
  * `memory.c/h` - functions to allocate and free memory. When `FFTW3` is used, complex vectors are handled differently for compatibility
  * `oclcore.c/h` - all OpenCL functionality, uses `oclkernels.cl`
  * `os.h` - determines the operation system
  * `param.h` - handy functions for testing the command line arguments
  * `parbas.h` - wrapper for `mpi.h`, which additionally test the version of the MPI
  * `prec_time.c/h` - functionality for precise timing of `initDmatrix` and `MatVec`, used when compiled with `PRECISE_TIMING`
  * `svnrev.h` - header containing revision number of the source, which is automatically generated (updated) by `updsvnrev.sh` during compilation
  * `types.h` - definitions of a few structures
  * `vars.c/h` - global variables