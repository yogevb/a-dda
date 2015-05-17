The development of `ADDA` started in 1990 when the original author, Alfons Hoekstra, started with his PhD research. His assignment was to implement the coupled dipole method (as we called it back then) on a recently acquired computer at the University of Amsterdam. This was a distributed memory parallel computer. The research question was two-fold:
  * demonstrate parallel computing performance of a production code on a parallel computer,
  * use it for investigation of elastic light scattering from biological particles.

So, `ADDA` was a parallel program from the start. The computer in question was a Meiko CS1, containing a whopping 64 T800 [Transputers](http://www.wikipedia.org/wiki/Transputer).  At that time this was a revolutionary machine based on a revolutionary processor. However, there was a price to be paid. When the first version of `ADDA` was implemented, there were no compilers available on the Meiko CS1 for the standard programming languages such as C or Fortran. A bit later they arrived, but we decided not to wait and have a first implementation in a language that was co-developed with the Transputer, and this language was Occam [[1](#References.md)]. Moreover, the Meiko was hardly equipped with modern communication libraries (like we now have in MPI), so all the routing needed to be hard coded on the machine. Below are some short code snippets to get a feeling how the first `ADDA` code looked like. Unfortunately, the Occam source codes are no longer really accessible, as the Occam editor/compiler used the concept of a folding editor (a bit like Mathematica notebooks), and this editor/compiler does not run on current versions of Unix. Below is some code to create a ring of Transputers:

```
VAL m IS (nTransputers-1):
CHAN OF ANY toHost, fromHost:
[nTransputers+1] CHAN OF ringChannel linkClockWise, linkNotClockWise :
-- linkClockWise[0] is the link, talking clockwise between the transputers 0-1
-- linkNotClockWise[3] is the link between the transputers 3-4, talking in
-- not ClockWise direction
PLACED PAR PROCESSOR 0 T8
PLACE toHost AT toL2:
PLACE fromHost AT fromL2:
PLACE linkClockWise[0] AT toL0:
PLACE linkNotClockWise[0] AT fromL0:
...
```

Next, some Occam compute code:

```
--     This procedure calculates the operator F (see     --;
--     internal report "The Radiating Dipole", by        --;
--     A.G. Hoekstra) which couples the dipole moment    --;
--     located at rdip with the electric field observed  --;
--     at robs. The result is stored in f.r and f.i,     --;
--     the real and imaginary part of the complex        --;
--     interaction tensor.                               --;
--                                                       --;
___________________________________________________________;
--                                                       --;
--                          A.G. Hoekstra, spring 1991   --;
--                                                       --;

  #USE maths64.lib
  #USE cmplx.lib
  -- calls to the complex lib could be omitted          
  -- this will improve the performance
  [3]REAL64 r:  -- vector from dipole to observer2
  REAL64 rabs, a.r, a.i, b.r, b.i, eikr.r, eikr.i:
  SEQ
    r[0],r[1],r[2] := (robs[0]-rdip[0]),(robs[1]-rdip[1]), (robs[2]-rdip[2]):
    rabs := DSQRT(((r[0]*r[0]) + (r[1]*r[1])) + (r[2]*r[2])):
    r[0], r[1], r[2] := r[0]/rabs, r[1]/rabs, r[2]/rabs
...
```

Note the use of the `SEQ` call. In Occam we were forced to specify if a computation should be done sequentially (`SEQ`) or in parallel (`PAR`). Anyway, a beautiful language, and we got good parallel performance (see e.g. [[2,3](#References.md)]), but the code was off course absolutely not portable. We proved that the coupled dipole method could reach very high efficiencies on a parallel computer, but then we needed a more portable and maintainable code.

Around 1992 a next parallel computer arrived at the University of Amsterdam, the Parsytec CGel, equipped with 512 T805 Transputers. It had C-compilers, and a decent library for Single Program Multiple Data (SPMD) type parallel computing (the Parix library). So, end of 1992 `ADDA` was ported to C and Parix and was running on 512 transputers (see e.g. [[4](#References.md)]). This was the time that the parallel computing community was working hard on developing SPMD libraries, and a number of competing environments were available. We used `ADDA` to compare Parix, Express and [PVM](http://www.csm.ornl.gov/pvm/pvm_home.html)  [[5,6](#References.md)]. The PVM emerged as the _de-facto_ standard, and as `ADDA` was ported to PVM, we kept that line of development and ported the code to some other parallel computers, as well as to clusters of workstations (for which PVM was originally developed).

The PVM-`ADDA` code of around 1994 was very powerful in terms of computational speed and parallel performance [[7](#References.md)]. However, it missed one very important feature to make it a true production code. It did not include the FFT to do the matrix–vector products in the iterative solver. So, albeit parallel, the code had an unacceptable O(_N_<sup>2</sup>) computational complexity. The point was, at that time there were not highly portable, tailored parallel FFTs available. Fortunately, a gifted student, Michiel Grimminck, developed a parallel FFT for `ADDA`, reaching impressive performances at that time [[8](#References.md)]. `ADDA` was also ported to MPI which is up until now, the standard for SPMD style parallel programming.

That version of `ADDA` was used for a long time. We used it to report DDA simulation of a sphere with size parameter 40 in the year 1997 [[9](#References.md)]. This was hardly believed by the audience, as it was commonly believed that size parameters much larger than say 15 or 20 were not possible. Indeed, on single processor workstations that was more or less the limit, but on a parallel supercomputer we could move to much larger particles.

`ADDA` was used for many different light scattering studies, but its implementation was hardly changed. The number of pre-defined particles were slowly increased, new observables were added (e.g. the option to compute radiative forces [[10](#References.md)]) and the code was constantly ported to new parallel machines. The later was an easy job, as the combination of a C code with MPI was easily portable.

With the arrival of Maxim Yurkin in Amsterdam in 2004 a next wave of `ADDA` development started. The code was completely redone, an optimized parallel FFT package ([the FFTW](http://www.fftw.org/)) was included, a Windows version was created, finally reaching its current state. `ADDA` was then put into the public domain, and can now be used by the community at large. The history of the latter stage is documented in [Release notes](ReleaseNotes.md).

# References #
  1. INMOS Ltd., _Occam 2 Reference Manual_, Prentice Hall, 1988.
  1. A.G. Hoekstra, L.O. Hertzberger, and P.M.A. Sloot, “Simulation of elastic light scattering on distributed memory machines,” _Bull. Am. Phys. Soc._ **36**, 1798 (1991).
  1. P.M.A. Sloot and A.G. Hoekstra, “Implementation of a parallel conjugate gradient method for simulation of elastic light scattering,” _Comp. Phys._ **6**, 323 (1992).
  1. P.M.A. Sloot and A.G. Hoekstra, “Light scattering simulations on a massively parallel computer at the IC3A,” _Transput. Applic._ **1**, 42-49 (1993).
  1. A.G. Hoekstra, P.M.A. Sloot, F. van der Linden, M. van Muiswinkel, J.J.J. Vesseur, and L.O. Hertzberger, “Native and generic parallel programming environments on a transputer and a PowerPC platform,” [Concurrency Pract. Ex. 8, 19-46 (1996)](http://dx.doi.org/10.1002/(SICI)1096-9128(199601)8:1<19::AID-CPE193>3.0.CO;2-9).
  1. P.M.A. Sloot, A.G. Hoekstra, and L.O. Hertzberger, “A comparison of the Iserver-Occam, Parix, Express, and PVM programming environments on a Parsytec GCel,” _High-Performance Computing and Networking: International Conference and Exhibition_, W. Gentzsch and U. Harms, eds.,  Berlin: Springer-Verlag, 1994, pp. 253-259.
  1. A.G. Hoekstra and P.M.A. Sloot, “Coupled dipole simulations of elastic light scattering on parallel systems,” [Int. J. Mod. Phys. C 6, 663-679 (1995)](http://dx.doi.org/10.1142%2FS0129183195000563).
  1. A.G. Hoekstra, M.D. Grimminck, and P.M.A. Sloot, “Large scale simulations of elastic light scattering by a fast discrete dipole approximation,” [Int. J. Mod. Phys. C 9, 87-102 (1998)](http://dx.doi.org/10.1142%2FS012918319800008X).
  1. A.G. Hoekstra, P.J.H. Brinkhorst, and P.M.A. Sloot, “First results of DDA simulations of elastic light scattering by red blood cell,” _Proceedings of Workshop on Light Scattering by Non-spherical Particles_, K. Lumme, J.W. Hovenier, K. Muinonen, J. Rahola, and H. Laitinen, eds.,  Helsinki: University of Helsinki, 1997, pp. 39-40.
  1. A.G. Hoekstra, M. Frijlink, L.B.F.M. Waters, and P.M.A. Sloot, “Radiation forces in the discrete-dipole approximation,” [J. Opt. Soc. Am. A 18, 1944-1953 (2001)](http://dx.doi.org/10.1364%2FJOSAA.18.001944).