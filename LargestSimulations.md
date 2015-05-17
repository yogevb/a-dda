The aim of this page is to give a feeling of how large light scattering simulations can be done using `ADDA`. Hopefully, it will also present a variety of different applications, so that each example will tell an interesting short story by itself. Complexity of the light scattering simulation with `ADDA` depends on the many (connected) factors: scatterer size parameter, required discretization or accuracy, number of the dipoles in a computational grid, convergence speed of the iterative solver, and need for repeated calculations (e.g. orientation averaging). Therefore, this page consists of several categories (nominations) with several examples in each of them.



Currently there are only a very small number of examples. Users are encouraged to submit new examples based on their own work/experience. This can be done either by a comment to this wiki page or contacting the [maintainer of this page](mailto:yurkin@gmail.com) directly. Before proceeding to the description of particular examples, we present a summary table.

| **Parameter** | **X1** | **N1** | **N2** | **T1** |
|:--------------|:-------|:-------|:-------|:-------|
| Shape         | sphere | slab   | red blood cell | sphere |
| Volume-equivalent size parameter | **320** | 16     | 36     | 130    |
| Refractive index (average) | 1.05   | 1.31+0.1i | 1.03   | **1.2** |
| Dipoles per wavelength | 10     | **257** | 93     | 12     |
| Largest 1D grid size | 1024   | **8192** | 1408   | 512    |
| Number of dipoles in a grid, 10<sup>6</sup> | **1073** | **1073** | 1035   | 134    |
| Number of occupied dipoles, 10<sup>6</sup> | 562    | **1073** | 619    | 70     |
| Relative residual norm (`-eps`) | 10<sup>−4</sup> | **10<sup>−5</sup>** | **10<sup>−5</sup>** | **10<sup>−5</sup>** |
| Iterative solver used | Bi-CGStab | QMR    | QMR    | Bi-CGStab |
| Number of iterations (average) | 334    | 23     | 10     | **29200** |
| Number of runs of iterative solver | 1      | **2**  | 1      | 1      |
| Number of processor cores | 512    | **4096** | 560    | 64     |
| Processor time, THz×hours | 8.8    | 0.2    | 1.1    | **90** |
| Total memory used, GB | 698    | **1093** | 772    | 71     |
| `ADDA` version | 0.79a2 | **1.2** | 0.79   | 0.75b  |
| Supercomputer | [MareNostrum](http://www.bsc.es/plantillaA.php?cat_id=5) | [Sisu](http://www.csc.fi/english/research/Computing_services/computing)| [LISA](https://subtrac.sara.nl/userdoc/wiki/lisa/description) | [LISA](https://subtrac.sara.nl/userdoc/wiki/lisa/description) |

_The values in bold are the largest (the hardest to reach) ones for a particular parameter among all examples_

# Largest size parameter #
## X1. Sphere with size parameter 320 and refractive index 1.05 ##
This was a benchmark simulation, performed to test the performance and extreme capabilities of `ADDA` on [MareNostrum](http://www.bsc.es/plantillaA.php?cat_id=5). `ADDA` was executed with a command line:
```
adda_mpi -m 1.05 0 -grid 1024 -size 640 -ntheta 1800 -iter bicgstab -eps 4
```

# Largest number of dipoles #
## N1. Truncated slab ##
This was a part of a scalability study of `ADDA` on [Sisu](http://www.csc.fi/english/research/Computing_services/computing), performed by Antti Penttila. The overall parallel efficiency for the largest number of cores (4096) was estimated to be 63%, but it was 77% and 83% for 2048 and 512 cores respectively. `ADDA` was executed with a command line:
```
adda -size 100 -m 1.31 0.1 -grid 4096 32 8192 -shape box 0.0078125 2 
```
The size was chosen relatively small (corresponding to very large number of dipoles per wavelength) to keep the simulations fast. However, at least 10 times larger sizes seem feasible with such hardware. In particular, such simulations can be used to study optical properties of effectively infinite inhomogeneous (particulate) slabs.

## N2. Red blood cell with very fine discretization ##
This simulation was a part of comparison of two methods for simulation of light scattering, published in K.V. Gilev, E. Eremina, M.A. Yurkin, and V.P. Maltsev, “Comparison of the discrete dipole approximation and the discrete source method for simulation of light scattering by red blood cells,” [Opt. Express 18, 5681-5690 (2010)](http://dx.doi.org/10.1364/OE.18.005681).

Since both considered methods involve certain numerical errors, there was need for some reference, so that accuracy of the methods can be independently evaluated. Described simulation for very fine discretization was used as such a reference.
`ADDA` was executed with a command line:
```
adda_mpi -lambda 0.4936 -m 1.03 0.0 -shape axisymmetric fung_100points.txt -size 7.5 -grid 1408 -scat_grid_inp scat_params_eryth.dat -yz
```
Moreover, convergence of DDA results with refining discretization was studied to estimate the accuracy of the reference itself.

# Largest processor time #
## T1. Sphere with size parameter 130 and refractive index 1.2 ##
This simulation was performed in frame of a benchmark study: M.A. Yurkin, V.P. Maltsev, and A.G. Hoekstra, “The discrete dipole approximation for simulation of light scattering by particles much larger than the wavelength,” [J. Quant. Spectrosc. Radiat. Transfer 106, 546-557 (2007)](http://dx.doi.org/10.1016/j.jqsrt.2007.01.033). `ADDA` was executed 25 times in a row with a command line:
```
adda -m 1.2 0 -size 260 -grid 512 -ntheta 1440 -iter bicgstab -chpoint 17h [-chp_load]
```
where the last option was used for every incantation except the first one. Use of checkpoint system was necessary, since at that time no single job could use more than 1200 processor-hours on [LISA](https://subtrac.sara.nl/userdoc/wiki/lisa/description).