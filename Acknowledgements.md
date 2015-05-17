

# Contributors #
  * Clive Temperton is the author of the CFFT99 Fortran routine (1980).
  * Matteo Frigo and Steven G. Johnson (fftw@fftw.org) are the authors of the `FFTW3` package, to which `ADDA` is linked.
  * Makoto Matsumoto and Takuji Nishimura are the authors of the Mersenne twister random number generator.
  * Michel D. Grimminck implemented the first versions of FFT part of `ADDA` ([fft.c](http://code.google.com/p/a-dda/source/browse/trunk/src/fft.c)), matrix–vector product ([matvec.c](http://code.google.com/p/a-dda/source/browse/trunk/src/matvec.c)), and most of the non-standard beam types ([GenerateB.c](http://code.google.com/p/a-dda/source/browse/trunk/src/GenerateB.c)). He also contributed to the particle generation routines ([make\_particle.c](http://code.google.com/p/a-dda/source/browse/trunk/src/make_particle.c)).
  * Martin Frijlink implemented the first versions of the MPI part of `ADDA` ([comm.c](http://code.google.com/p/a-dda/source/browse/trunk/src/comm.c)), 2D Romberg integration ([Romberg.c](http://code.google.com/p/a-dda/source/browse/trunk/src/Romberg.c)), and calculation of the radiation force and scattering quantities obtained by integration ([crosssec.c](http://code.google.com/p/a-dda/source/browse/trunk/src/crosssec.c)).
  * Konstantin A. Semyanov added several new shapes to the older version of particle generation routine ([make\_particle.c](http://code.google.com/p/a-dda/source/browse/trunk/src/make_particle.c)).
  * Daniel Hahn and Richard Joseph implemented `egg` and `capsule` shapes.
  * Konstantin Gilev provided the basis of the code for the shape `axisymmetric`.
  * Patrick C. Chaumet and Adel Rahmani contributed Fortran routines to integrate Green’s tensor.
  * Jin You Lu provided the basis of the code for the shapes `bicoated` and `bisphere`.
  * Alexander Moskalensky provided the basis of the code for the shape `biellipsoid`.
  * Sergei Lebedev implemented shape `plate`.
  * Roman Schuh and Thomas Wriedt contributed "Point in Polyhedron" tool ([misc/pip](http://code.google.com/p/a-dda/source/browse/trunk/misc/pip)), using the code by John Burkardt.
  * Josef Gasteiger contributed tool chain to work with Hyperfun models ([misc/hyperfun](http://code.google.com/p/a-dda/source/browse/trunk/misc/hyperfun)).
  * Code for iterative solver `bcgs2` is based on [zbcg2.f90](http://www.math.uu.nl/people/vorst/zbcg2.f90) by M.A. Botchev and D.R. Fokkema.
  * Neoklis Kyriazis is the author of NEC2C, from which Sommerfeld integration routines are used.
  * Stefania D'Agostino contributed the first version of the code to calculate dipole decay-rate enhancement.

The above list doesn't include [current contributors](http://code.google.com/p/a-dda/people/list).

# We also thank #

  * Bruce T. Draine and Piotr J. Flatau for creating and developing the first publicly available DDA code [DDSCAT](http://www.astro.princeton.edu/~draine/DDSCAT.html). They maintain a high standard, which we try to match, both for highly productive convenient code and clear detailed manual. Parts of the [user guide for DDSCAT 6.1](http://arxiv.org/abs/astro-ph/0409262) were used when writing the [manual](http://a-dda.googlecode.com/svn/trunk/doc/manual.pdf) for `ADDA`.
  * Alexander N. Shvalov for information on [MinGW](http://mingw.org/) and encouraging the authors to produce executables for Windows.
  * Michiel Min for fruitful discussions about the `-anisotr` option and his motivation to implement the FCD formulation.
  * R. Scott Brock for ideas to greatly accelerate computation of the scattered field.
  * All `ADDA` users who provided bug reports and suggestions, especially Antti Penttila and Vitezslav Karasek.

# Funding sources #

  * The University of Amsterdam (The Netherlands)
  * The Dutch Science Foundation NWO (The Netherlands)
  * The Department of Defense (USA)
  * The NATO Science for Peace program (project 977976, 2002-2006)
  * European Research Council (ERC) Starting Grant FP7 Project DEDOM (grant 207441, 2008-2013).
  * Program of the Russian Government "Research and educational personnel of innovative Russia" - contract P2497 (2009-2011) and grant 8752 (2012-2013).