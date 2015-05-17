Many researchers have used `ADDA` and compared it with other codes based on the DDA or other light scattering methods. The purpose of this page is to give a few representative examples instead of providing a full overview. Figures on this page are either in vector format (svg) or can be clicked to open a larger image.



# Other DDA codes #

Comparison of four different DDA codes, including `ADDA` 0.7a and [DDSCAT 6.1](http://www.astro.princeton.edu/~draine/DDSCAT.6.1.html), was performed by Penttila et al. [[1](#References.md)] for simulation of different wavelength-sized particles both in single orientation and averaged over all orientations. Here we present a brief summary and a few sample results from this paper. `ADDA` is faster than other codes by a factor of 2.3 to 16 and it is the least memory consuming code. Results of simulations of light scattering by a sphere in a fixed orientation are shown in the following figure. All scatterers considered in this section have volume-equivalent size parameter 5.1 and refractive index 1.313. `SIRRI` and `ZDD` denote codes by Simulintu Oy in Finland and Zubko and Shkuratov respectively. All codes result in almost identical relative error of _S_<sub>11</sub>(_θ_), as compared with the Mie theory, showing that the codes use the same formulation of DDA, except `SIRRI` that is slightly different.

![http://a-dda.googlecode.com/svn/wiki/sphere_dda.svg](http://a-dda.googlecode.com/svn/wiki/sphere_dda.svg)

On contrary, orientation averaging schemes of the codes are different. Orientation averaged results for a randomly constructed aggregate of 50 spheres are shown in the next figure (here and below reference results are obtained using the T-matrix method). One can see that results differ, and the errors of `ADDA` results are the largest. That is explained by the fact that for `ADDA` we used 16×9×8 sample points for Euler angles _α_, _β_, and _γ_ respectively. Other codes used less _α_ sample angles (11, 4, and 1) but more _β_ and _γ_ sample angles, resulting in approximately the same total number of sample angles. Therefore, `ADDA` is faster at expense of possible deterioration of accuracy. However, for other asymmetric particles studied in [[1](#References.md)] `ADDA` results in comparable to other codes accuracy even with current settings (particle showed here was the least favorable for `ADDA`).

![http://a-dda.googlecode.com/svn/wiki/aggregate_dda.svg](http://a-dda.googlecode.com/svn/wiki/aggregate_dda.svg)

The results for axisymmetric particles are principally different. Here we consider only a prolate spheroid with aspect ratio 2 (see the next figure). One can see that `ADDA` result (the first curve) is much less accurate than other codes. The accuracy of `ADDA` is unusually bad, even compared to the previous figure. In the following we explain this fact. For axisymmetric particles the dipole array is symmetric with respect to 90° rotation over the symmetry axis. Moreover, the range from 0° to 90° for _γ_ is mirror symmetric with respect to the 45°. Hence, the most efficient way of orientation averaging for such particles is to sample _γ_ uniformly in the range from 0° to 45°, and also to use less _γ_ sample angles but more _β_ sample angles. Sampling by other codes satisfies these conditions. The situation for `ADDA` is remarkably different. 8 _γ_ angles are used, which are spaced with interval 45°, hence they map in only two distinct angles (0° and 45°) inside the reduced range. That means that for axisymmetric particles `ADDA` repeats calculation for four equivalent sets of orientations. Moreover, Romberg integration, which is a higher order method, performs unsatisfactory when applied to a range that contains several periods of the periodic function to be integrated and only a few points per each period. An analogous discussion applies to the _β_ angle since both spheroid and cylinder have a symmetry plane perpendicular to the symmetry axis.

![http://a-dda.googlecode.com/svn/wiki/spheroid_dda.svg](http://a-dda.googlecode.com/svn/wiki/spheroid_dda.svg)

It is important to note that `ADDA` is perfectly suitable to calculate orientation averaging of axisymmetric particles efficiently, but it requires a user to manually modify the sampling intervals for Euler angles in the input files. For instance, for the discussed scatterers the choice of 17 _β_ angles from 0° to 90° and 5 _γ_ angles from 0° to 45° is appropriate. The result of this tuned orientation averaging for spheroid, obtained using `ADDA` 0.76, is shown as the last curve in the previous figure. It requires about 20% more computational time than the initial scheme, but still it is significantly faster than other codes. One can see that its accuracy is comparable to or even better than that of other codes. This result was not included in [[1](#References.md)], because such modifications of the input files were considered a ‘fine-tuning’. `DDSCAT` may also benefit from reduction of angle ranges, but in much lesser extent (in these particular cases).

Therefore, we may conclude that `ADDA` is clearly superior for particles in single orientation. Its orientation averaging scheme is not that flexible, especially when small number of sample angles is used. However, in most cases it results in accuracy not worse than that of other codes, if particle symmetries are properly accounted for. Moreover, inflexibility is compensated by the adaptability of the `ADDA` orientation averaging scheme and its ability to estimate the averaging error. We should also note, that the optimum strategy for orientation averaging is still an open question (see e.g. [issue 40](https://code.google.com/p/a-dda/issues/detail?id=40)).

Finally, the described comparison is already outdated. Both `ADDA` and `DDSCAT` have undergone significant improvement since that time. Moreover, a new code [OpenDDA](http://www.opendda.org/) have been released, claiming superior computational efficiency.

# Multiple multipole program (Gaussian beam) #

To validate the part of `ADDA`, which simulates scattering by a Gaussian beam, we conducted a simple test and compared results with that of the [multiple multipole program](http://www.scattport.org/index.php/programs-menu/generalized-multipole-menu/49-3d-mmp). The latter were provided by Roman Schuh and Thomas Wriedt. Sphere with size parameter 5 and refractive index 1.5 is illuminated with a Gaussian beam with wavelength 1 µm, width 2 µm, and position of the beam center (1,1,1) µm. The beam propagates along the _z_-axis. We calculated _S_<sub>11</sub>(_θ_) in the _yz_-plane, scattering angle _θ_ was considered from the _z_-axis to the _y_-axis. We used 64 dipoles per diameter of the sphere. `ADDA` 0.76 was used with a command line:
```
adda -grid 64 -lambda 1 -size 1.59155 -beam barton5 2 1 1 1 -ntheta 180
```
The results are shown in the following figure, indicating perfect agreement between two methods.

![http://a-dda.googlecode.com/svn/wiki/mmp_dda.svg](http://a-dda.googlecode.com/svn/wiki/mmp_dda.svg)

# Null-field method with discrete sources (anisotropic material) #

Schmidt and Wriedt [[2](#References.md)] extended the null-field method with discrete sources ([NFM-DS](http://www.scattport.org/index.php/programs-menu/t-matrix-codes-menu/239-nfm-ds)) to anisotropic scatterers and verified their code by comparison with `ADDA` and `DDSCAT`. The following figure shows one particular example for ellipsoid with semi-axes _ka_=2, _kb_=3, _kc_=4 (_k_ is the wavenumber) and refractive index _m<sub>x</sub>_=1.5, _m<sub>y</sub>_=_m<sub>z</sub>_=1.1. Incident wave propagates along the _z_-axis and _S_<sub>11</sub> is calculated in _xz_-plane. NFM-DS data was provided by Vladimir Schmidt. `ADDA` 0.79 was run with the following command line.
```
adda -size 4 -shape ellipsoid 1.5 2 -m 1.5 0 1.1 0 1.1 0 -anisotr -orient 90 0 0
```
where the last option (`-orient`) was used for convenience to make the first two columns of the file `mueller`  produced by `ADDA` (corresponding to _yz_-plane in particle reference frame) equivalent to the sought _S_<sub>11</sub> in _xz_-plane in laboratory reference frame.

![http://a-dda.googlecode.com/svn/wiki/nfmds_dda.svg](http://a-dda.googlecode.com/svn/wiki/nfmds_dda.svg)

# Finite difference time domain method #

A systematic comparison of the DDA and the finite difference time domain (FDTD) method was performed for large dielectric scatterers by Yurkin et al. [[3](#References.md)]. Here we summarize these results. Simulations were performed for spheres with size parameter _x_ from 20 to 80 and refractive index _m_ from 1.02 to 2 (all _m_ also had a fixed negligible imaginary part). Since both methods are potentially exact if infinite discretization is employed, a certain accuracy was required – relative error of _Q_<sub>ext</sub> less than 1% and the root-mean-square relative error of _S_<sub>11</sub> over the whole range of _θ_ less than 25%. Then simulation times were compared. `ADDA` v.0.76 was used as DDA implementation, and FDTD implementation was developed in the Biomedical Laser Laboratory at East Carolina University, based on the algorithms described by Yang and Liou with numerical dispersion correction (see [[3](#References.md)] for details).

The main conclusion is summarized in the following figure, where a ratio of computational times of two methods is shown versus _x_ for different _m_. The DDA is more than an order of magnitude faster for _m_ ≤1.2 and _x_>30, while for _m_ ≥1.7 the FDTD is faster by the same extent. _m_=1.4 is a boundary value, for which both methods perform comparably. The DDA errors of _S_<sub>11</sub>(_θ_) for _m_=1.02 are mostly due to the shape errors, which are expected to be smaller for rough and/or inhomogeneous particles. Simulations for a few sample biological cells lead to the same conclusions. Although these conclusions depend on particular scattering quantity and on the implementations of both methods, they will not change principally unless a major improvement of one of the method is made. However, for general complex refractive indices the results of the comparison are expected to be significantly different.

![http://a-dda.googlecode.com/svn/wiki/fdtd_dda.svg](http://a-dda.googlecode.com/svn/wiki/fdtd_dda.svg)

# Pseudo-spectral time domain method #

A systematic comparison of the DDA and the pseudo-spectral time domain (PSTD) method was performed for large dielectric scatterers by Liu et al. [[4](#References.md)]. Here we summarize these results. Overall method of comparison was analogous to [that for the FDTD](ComparisonOtherCodes#Comparison_with_the_finite_difference_time_domain_method.md). Simulations were performed for spheres with size parameter _x_ from 10 to 100 and refractive index _m_ from 1.2 to 2. The following accuracy was required – relative error of _Q_<sub>ext</sub> less than 1% and the root-mean-square relative error of _S_<sub>11</sub> over the whole range of _θ_ less than 25%. Then simulation times were compared. `ADDA` v.0.79 with the default settings was used as DDA implementation, and PSTD implementation was developed in the Department of Atmospheric Sciences of Texas A&M University (see [[4](#References.md)] for details).

The main conclusion is summarized in the following figure, where the regions of superiority of the DDA and the PSTD are shown. Differences of the simulation times (to reach the same accuracy) were up to a factor of 100 (both ways). Simulations for a few sample spheroidal particles lead to the same conclusions.

![http://a-dda.googlecode.com/svn/wiki/pstd_dda.svg](http://a-dda.googlecode.com/svn/wiki/pstd_dda.svg)

Although these conclusions depend on particular scattering quantity and on the implementations of both methods, they will not change principally unless a major improvement of one of the method is made. In particular, we have tried to push `ADDA` to the limit for two spheres (_x_=10, _m_=2 and _x_=80, _m_=1.2). We tried different DDA formulations and iterative solvers, resulting in 14 and 4 times decrease of computational time for these two cases, respectively. However, these times were still two-three times larger than that of the PSTD. However, for general complex refractive indices and/or inhomogeneous particles the results of the comparison are expected to be significantly different.

In particular, a more recent paper [[5](#References.md)] considered refractive index of ice in a wide spectral range (including absorbing ones). It was shown that absorption improves the performance of both methods, however, the DDA especially benefits from it. For strongly absorbing ice (Im(_m_)>0.1) the DDA is about twice faster than the PSTD for all tested size parameters (up to 100).

# Improved geometric optics method #

Bi et al. compared `ADDA` with the improved geometric optics method (IGOM) for simulation of randomly-oriented ellipsoids [[6](#References.md)] and hexahedra [[7](#References.md)]. The accuracy of the `ADDA` was assumed good enough (unfortunately, the details of the DDA simulations were not specified), while accuracy of IGOM is tested against it. A particular example from [[7](#References.md)] is shown in the following figure, depicting integrated scattering properties of randomly oriented nonsymmetric hexahedra versus the size parameter. One can see that both methods agree in intermediate size parameter range, thus effectively closing the gap between geometric-optics and wave-optics methods. However, the deviations for angle-resolved light scattering properties ,e.g. _S_<sub>11</sub>, can still be significant (see [[7](#References.md)] for details).

![![](http://a-dda.googlecode.com/svn/wiki/igom_dda_small.png)](http://a-dda.googlecode.com/svn/wiki/igom_dda_large.png)

# `Tsym` for cubes #

`ADDA` was compared with `Tsym`, a T-matrix code employing discrete symmetries of the scatterer, for a number of cubes with _kD_=0.1 and 8 (_k_ is the wavenumber, _D_ is the cube edge) and refractive index _m_=1.6+0.01i, 0.1+i, and 10+10i [[8](#References.md)], pushing the accuracy of both codes to the limit. Here we present only the results for one case - _kD_=8 and _m_=1.6+0.01i. The cube is in fixed orientation with edges along the axes. `ADDA` v.0.78.2 was used with default settings, except for the FCD formulation and 10<sup>−10</sup> convergence threshold for the iterative solver. The command line was the following:
```
adda -size 8 -m 1.6 0.01 -ntheta 180 -shape box -int fcd -pol fcd -eps 10 -grid <nx>
```
where 5 grid sizes (`<nx>`) were used from 256 to 512, and extrapolation technique [[9](#References.md)] was employed afterwards. Also, a special technique was used for `Tsym` results to estimate computational accuracy. Results for the integral scattering quantities are summarized in the following table. Uncertainties (±) are in the last shown digits of the corresponding value and have nominal confidence level of 95%.
| | _Q_<sub>ext</sub> | _Q_<sub>sca</sub> | _Q_<sub>abs</sub> |
|:|:------------------|:------------------|:------------------|
| `ADDA` | 4.2480442 ± 3    | 3.9715646 ± 3    | 0.2764796 ± 1    |
| `Tsym` | 4.24775 ± 33     | 3.97156 ± 24     | 0.2757 ± 7       |

Below is the results for intensity _I_=_S_<sub>11</sub> in the _yz_-plane, showing relative differences between the two methods and their internal error estimates.

![http://a-dda.googlecode.com/svn/wiki/tsym_dda.svg](http://a-dda.googlecode.com/svn/wiki/tsym_dda.svg)

# Particles near surface #

To validate the part of `ADDA`, which implements simulation of optical properties of particles near surface, we compared it with a number of other methods.

First test is for Fe prolate spheroid (semi-axes 25nm and 50nm, refractive index 1.35+1.97i) placed on Si substrate (refractive index 4.37+0.08i), illuminated by plane wave from above propagating at 45° relative to the surface normal. Wavelength is 488nm. `ADDA` v.1.3b2 ([r1266](https://code.google.com/p/a-dda/source/detail?r=1266)) was used with a command line
```
adda -shape ellipsoid 1 2 -size 0.05 -lambda 0.488 -surf 0.05 4.37 0.08 -m 1.35 1.97 -grid <nx> -prop 1 0 -1 -ntheta 180
```
where `<nx>` varied between 16 and 64. Shown below is normalized (to have maximum value of 1) perpendicular and parallel (to scattering plane) scattering intensities. Scattering angle corresponds to the spherical angle _θ_ of laboratory reference frame inside the main scattering plane (given by incident and reflected direction).

![http://a-dda.googlecode.com/svn/wiki/Fe_spheroid.svg](http://a-dda.googlecode.com/svn/wiki/Fe_spheroid.svg)

Reference results are T-matrix simulations, digitized from Fig.4.7 of [[10](#References.md)] with appropriate modification of scattering angle definition. The agreement is perfect for perpendicular intensity, but there are some discrepancies for parallel. The latter, however, clearly diminish with refining discretization, and the remaining difference is compared to that between the T-matrix and discrete-sources-method in [[10](#References.md)] (data not shown).

A second test is for Ag sphere (radius _R_=50nm, refractive index 0.25+3.14i) placed on glass substrate (refractive index _m_<sub>s</sub>=1.5), illuminated by plane wave from below propagating at 60° relative to the surface normal (evanescent illumination). Wavelength is 488nm. `ADDA` v.1.3b4 was used with command line
```
adda -size 0.1 -lambda 0.488 -surf 0.05 1.5 0 -m 0.25 3.14 -prop 1 0 0.577350 -ntheta 180 -grid 64
```
Shown below are perpendicular and parallel scattering intensities (_S_<sub>11</sub>−_S_<sub>12</sub> and _S_<sub>11</sub>+_S_<sub>12</sub> respectively) in the main scattering plane.

![http://a-dda.googlecode.com/svn/wiki/Ag_sphere.svg](http://a-dda.googlecode.com/svn/wiki/Ag_sphere.svg)

Reference T-matrix results were provided by Vladimir Schmidt (calculated using [NFM-DS](http://www.scattport.org/index.php/programs-menu/t-matrix-codes-menu/239-nfm-ds)) and coincide with that in Fig.4.10 of [[10](#References.md)]. The former raw results were multiplied by a factor _π_(_kR_)<sup>2</sup>/_m_<sub>s</sub>, where _k_ is the free-space wavenumber.

# Decay rate enhancements #
The test data for this section were provided by Stefania D'Agostino. The goal is to validate the part of `ADDA`, related to the point-dipole incident field, by comparing it with exact electromagnetic theory (EET) for a silver sphere (diameter 10 nm). EET is described in [[11](#References.md)], and the data were calculated with the [online applet](http://erbium.amolf.nl/). The refractive index of silver is from Palik [[12](#References.md)] with linear interpolation. First, the decay-rate enhancements (total as well as radiative and non-radiative parts) at the wavelength of 354nm were calculated as a function of dipole distance from sphere surface for parallel and perpendicular orientations of the dipole with respect to sphere surface. `ADDA` v.1.3b4 was used with a command line
```
adda -beam dipole 0 0 <z> -prop <ori> -size 10 -lambda 354 -m 0.20959 1.43592 -sym enf -grid 160
```
where `<z>` is the distance from dipole to sphere center (in nm), i.e. distance from surface + 5, and `<ori>` is "1 0 0" or "0 0 1" for parallel and perpendicular orientations of the dipole respectively. The results are similar to Fig.1 of [[13](#References.md)], which were obtained with a previous version of `ADDA` using a number of code hacks and workarounds. Now only `ADDA` itself is required for such simulations.

![http://a-dda.googlecode.com/svn/wiki/DRE_distance.svg](http://a-dda.googlecode.com/svn/wiki/DRE_distance.svg)

Second, convergence of the results with refining discretization was studied for total decay-rate enhancement for a dipole (in two orientations) at 5 nm from the sphere surface as a function of the wavelength. The command line is
```
adda -beam dipole 0 0 10 -prop <ori> -size 10 -lambda <lam> -m <mre> <mim> -sym enf -grid <nx>
```
where `<nx>` is either 80 or 160, `<lam>` is the wavelength (in nm), and `<mre>` and `<mim>` are the corresponding real and imaginary parts of the refractive index.

![http://a-dda.googlecode.com/svn/wiki/DRE_lambda.svg](http://a-dda.googlecode.com/svn/wiki/DRE_lambda.svg)

The agreement with EET becomes better with refining discretization. Moreover, if we use the simplest Richardson-type extrapolation of the DDA results to zero dipole size (see [[9](#References.md)] for justification), i.e. 2×(result for _n<sub>x</sub>_=160) − (result for _n<sub>x</sub>_=80), the agreement (green vs. blue line in the figure) is almost perfect (mostly within the line width).

# Inhomogeneous anisotropic sphere #
A new method was developed to simulate light scattering by spheres with radially varying anisotropic dielectric permittivity [[14](#References.md)]. It solves the electric-field volume-integral equation using Dini-type spherical basis functions. It was compared with `ADDA` for a sphere with size parameter _ka_=_π_ and the following diagonal permittivity tensor: _ε<sub>xx</sub>_(_r_) = _ε<sub>yy</sub>_(_r_) = 4/[1+(_r_/_a_)<sup>2</sup>], _ε<sub>zz</sub>_(_r_) = [4−(_r_/_a_)<sup>2</sup>].

For DDA simulations the sphere was divided into _n_<sub>L</sub> 1-dipole-wide layers, each having a constant permittivity equal to its value at the layer middle. `ADDA` 1.3b4 was modified (recompiled) to support up to 192 different values of refractive index (up to 64 anisotropic layers). The command line is like
```
adda -lambda 1 -size 1 -shape read 64layers.geom -ntheta 180 -sym enf -anisotr -m <mxx1> 0 <myy1> 0 <mzz1> 0 ... <mzz64> 0
```
where both shape file and the list of different refractive indices was generated by a separate program. The following comparison results are adapted from Fig.7 of [[14](#References.md)].

![http://a-dda.googlecode.com/svn/wiki/inhom_anis_sphere.svg](http://a-dda.googlecode.com/svn/wiki/inhom_anis_sphere.svg)

The agreement improves with increasing the number of layers. Moreover, if we use the simplest Richardson-type extrapolation of the DDA results to zero layer (dipole) size (see [[9](#References.md)] for justification), i.e. 2×(result for _n<sub>L</sub>_=64) − (result for _n<sub>L</sub>_=32), the agreement (green vs. blue line in the figure) is almost perfect (mostly within the line width).

# References #
  1. A. Penttila, E. Zubko, K. Lumme, K. Muinonen, M. A. Yurkin, B. T. Draine, J. Rahola, A. G. Hoekstra, and Y. Shkuratov, “Comparison between discrete dipole implementations and exact techniques,” [J. Quant. Spectrosc. Radiat. Transfer 106, 417-436 (2007)](http://dx.doi.org/10.1016/j.jqsrt.2007.01.026).
  1. V. Schmidt and T. Wriedt, “T-matrix method for biaxial anisotropic particles,” [J. Quant. Spectrosc. Radiat. Transfer 110, 1392-1397 (2009)](http://dx.doi.org/10.1016/j.jqsrt.2009.01.025).
  1. M. A. Yurkin, A. G. Hoekstra, R. S. Brock, and J. Q. Lu, “Systematic comparison of the discrete dipole approximation and the finite difference time domain method for large dielectric scatterers,” [Opt. Express 15, 17902-17911 (2007)](http://dx.doi.org/10.1364/OE.15.017902).
  1. C. Liu, L. Bi, R. L. Panetta, P. Yang, and M. A. Yurkin, “Comparison between the pseudo-spectral time domain method and the discrete dipole approximation for light scattering simulations,” [Opt. Express 20, 16763–16776 (2012)](http://dx.doi.org/10.1364/OE.20.016763).
  1. D. I. Podowitz, C. Liu, P. Yang, and M. A. Yurkin, “Comparison of the pseudo-spectral time domain method and the discrete dipole approximation for light scattering by ice spheres,” [J. Quant. Spectrosc. Radiat. Transfer 146, 402–409 (2014)](http://dx.doi.org/10.1016/j.jqsrt.2014.02.032).
  1. L. Bi, P. Yang, G. W. Kattawar, and R. Kahn, “Single-scattering properties of triaxial ellipsoidal particles for a size parameter range from the Rayleigh to geometric-optics regimes,” [Appl. Opt. 48, 114-126 (2009)](http://dx.doi.org/10.1364/AO.48.000114).
  1. L. Bi, P. Yang, G. W. Kattawar, and R. Kahn, “Modeling optical properties of mineral aerosol particles by using nonsymmetric hexahedra,” [Appl. Opt. 49, 334-342 (2010)](http://dx.doi.org/10.1364/AO.49.000334).
  1. M. A. Yurkin and M. Kahnert, “Light scattering by a cube: accuracy limits of the discrete dipole approximation and the T-matrix method,” [J. Quant. Spectrosc. Radiat. Transfer 123, 176–183 (2013)](http://dx.doi.org/10.1016/j.jqsrt.2012.10.001).
  1. M. A. Yurkin, V. P. Maltsev, and A. G. Hoekstra, “Convergence of the discrete dipole approximation. II. An extrapolation technique to increase the accuracy,” [J. Opt. Soc. Am. A 23, 2592–2601 (2006)](http://dx.doi.org/10.1364/JOSAA.23.002592).
  1. A. Doicu, R. Schuh, and T. Wriedt, “Scattering by particles on or near a plane surface,” in _Light Scattering Reviews 3_, A. A. Kokhanovsky, Ed., [pp. 109–130, Springer, Berlin (2008)](http://link.springer.com/chapter/10.1007/978-3-540-48546-9_4).
  1. H. Mertens, A. Koenderink, and A. Polman, “Plasmon-enhanced luminescence near noble-metal nanospheres: Comparison of exact theory and an improved Gersten and Nitzan model,” [Phys. Rev. B 76 (2007)](http://dx.doi.org/10.1103/PhysRevB.76.115123).
  1. D. W. Lynch and W. R. Hunter, “Comments on the optical constants of metals and an introduction to the data for several metals,” in _Handbook of Optical Constants of Solids_, E. D. Palik, Ed., pp. 275–267, Academic Press, San Diego (1985).
  1. S. D’Agostino, F. Della Sala, and L. C. Andreani, “Dipole-excited surface plasmons in metallic nanoparticles: Engineering decay dynamics within the discrete-dipole approximation,” [Phys. Rev. B 87, 205413 (2013)](http://dx.doi.org/10.1103/PhysRevB.87.205413).
  1. G. P. Zouros and G. C. Kokkorakis, “Electromagnetic scattering by an inhomogeneous gyroelectric sphere using volume integral equation and orthogonal Dini-type basis functions,” [IEEE Trans. Antennas Propag. accepted (2015)](http://dx.doi.org/10.1109/TAP.2015.2415851).