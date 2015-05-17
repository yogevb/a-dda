# Introduction #

Sparse DDA is a direct implementation of DDA without FFT. Its advantage is that the empty dipoles are not included at all in the data model, which is an advantage for particles that have relatively few dipoles spread out over a large volume.

The inability to use FFT usually causes a speed hit, but the computer memory footprint is much smaller, which allows one to compute targets that the available memory could not accommodate in FFT mode. With very sparse targets, one may also get a speed increase because the empty dipoles also waste computation time.

# Compiling `ADDA` in sparse mode #

Due to the different data model used in sparse mode, it has to be enabled when compiling `ADDA`. Use the option `SPARSE` to the makefile to build `ADDA` in sparse mode. If you use `gcc` and your hardware supports SSE3 (if you do not know, it probably does - just remove the option if you encounter problems), it is also recommended that you use the `USE_SSE3` option. That is: compile `ADDA` using
```
make seq OPTIONS="SPARSE USE_SSE3"
```
(sequential mode) or
```
make mpi OPTIONS="SPARSE USE_SSE3"
```
(MPI parallel mode).

# Running `ADDA` in sparse mode #

Since the sparse mode is currently an experimental feature, it is recommended that you test the resulting executable against one that was compiled without the `SPARSE` option and compare the outputs.

Otherwise, the sparse-mode executable is used just like the FFT (normal) one, but see the limitations below.

# Current limitations #

  * Only reading the dipoles from a file (i.e. the `-shape read` option) is currently supported in sparse mode. This complicates testing, but is usually not a problem in real use, as the the sparse mode is mainly useful with complicated targets, which have to be read from a file anyway. Likewise, the granule generator and saving of geometry are not supported.
  * While the sparse `ADDA` uses MPI effectively for CPU parallelization, GPU parallelization using OpenCL is not currently available.
  * WKB initial field (`-init_field wkb`) and precise timing (separate compilation mode) are not supported in sparse mode.
  * All polarization and dipole interaction terms used in normal `ADDA` mode are available. However, optimized computation of the interaction term is currently implemented only for the point-dipole interaction (the `-int poi` option, which is the default). For the other interaction terms, sparse `ADDA` falls back to the default implementation, which is slower and also does not benefit from the SSE3 acceleration. Because of this, the point-dipole interaction is recommended for sparse-mode computations.
  * See also [corresponding issues](http://code.google.com/p/a-dda/issues/list?q=label:Sparse).