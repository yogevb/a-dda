This tutorial is a slightly modified version of the one, carried out for participants of the DDA workshop in Bremen at 23.03.2007. It was based on the `ADDA` v.0.76, but we try to ensure that it applies to the current version. The tutorial is designed to be self-contained and we believe it is best enjoyed this way. However, feel free to refer to the [manual](http://a-dda.googlecode.com/svn/trunk/doc/manual.pdf), if you are stuck. It is assumed that you [have compiled](CompilingADDA.md) the sequential version of `ADDA` or [have downloaded](Downloads.md) executables. Before starting please make sure that you have the following files in your working directory:
```
adda or adda.exe
alldir_params.dat
avg_params.dat
libfftw3-3.dll (for Windows)
```
In the command lines of this tutorial it is assumed that `ADDA` can be run by typing `adda`, when positioned in the working directory. This can differ depending on the particular operating system.

1. Running `ADDA` is simple. Go and try it:
```
adda
```
This simulates light scattering by a default sphere (size parameter _x_ = 3.367, refractive index _m_ = 1.5, size of computational box along the _x_-axis _n<sub>x</sub>_ = 16 dipoles). You see the basic output of `ADDA` on the screen. It shows the directory name used for all output and problem parameters: incident wavelength _λ_, number of dipoles per _λ_ "dpl", required relative residual norm of the iterative solver _ε_. Then some derived values are shown, such as total number of non-void dipoles in the computational box and total memory requirements. It is followed by the progress report of the iterative solver, which shows the relative residual norm for each iteration. Finally, the extinction and absorption cross sections (_C_<sub>ext</sub>, _C_<sub>abs</sub>) and efficiencies (_Q_<sub>ext</sub>, _Q_<sub>abs</sub>) are shown.

This output is not designed to be complete but rather to provide the information on `ADDA` progress when simulation takes longer than a few seconds. A systematic overview of the simulation is provided in the output directory. It is located in the working directory and is named like `run000_sphere_g16m1_5`. Enter into this directory and observe three files: `log`, `mueller`, `CrossSec-Y`. The file `log` contains a detailed description of problem parameters, the convergence behavior of the iterative solver, used memory, and computational time divided into several different parts. The file `mueller` contains the calculated Mueller matrix as a function of polar angle in one scattering plane. The file `CrossSec-Y` contains the calculated integral scattering quantities, e.g. cross sections, for incident polarization along the _y_-axis.

2. Most of the problem parameters can be specified in the command line. Try the following:
```
adda –grid 16 –m 1.5 0 –dpl 15
```
The result is essentially the same as before, because the specified parameters (_n<sub>x</sub>_, _m_, and dpl) are set to the same as default values.

3. To simulate the scattering of a sphere with diameter equal to _λ_ type in
```
adda –grid 16 –m 1.5 0 –size 1 –lambda 1
```
This sets both _D<sub>x</sub>_ (size of the computational box along the _x_-axis) and _λ_ equal to 1 µm. However, the relevant value is the _D<sub>x</sub>_/_λ_ or some kind of size parameter.

4. If _λ_ is not given in the command line, the default value 2<i>π</i> µm is used. That means that `-size` effectively specifies the value of _kD<sub>x</sub>_ (_k_ is the free space wavenumber). The most natural way to run `ADDA` is to specify only the size parameter and the refractive index of a scatterer:
```
adda –m 1.5 0 –size 6
```
This simulates light scattering by a sphere with _m_ = 1.5 and size parameter 3. The discretization of the scatterer is managed automatically by `ADDA`, for that the “rule of thumb” is used: dpl = 10|_m_|.

5. The performance of `ADDA` significantly depends on the refractive index). Try the index-matching scatterer:
```
adda –m 1.05 0.1 –size 6
```
The convergence is now much faster (5 instead of 18 iterations). Since you specified nonzero imaginary part of refractive index, nonzero _C_<sub>abs</sub> is obtained.

6. Now it is time to ask `ADDA` some questions. Type in
```
adda -h
```
As expected, you get help, more specifically a list of all possible command line options. `ADDA` also informs you, how you can get more detailed help on any option .

7. You are probably tired of spheres, since that is not what you want to use the DDA for. To see, which other shapes `ADDA` can handle, run:
```
adda –h shape
```
The list is quite short; however, it contains an option `-read`, which allows specifying almost any shape you like in a file.

8. Get information on predefined shape, describing a coated sphere:
```
adda –h shape coated
```

9. Now you can simulate scattering by sphere with a spherical inclusion, not necessarily concentric. Run
```
adda –size 6 –shape coated 0.5 0.2 0 0
```
This should simulate a sphere with _x_ = 3, having spherical inclusion with twice less _x_. Inner sphere is also shifted by 20% of outer sphere diameter along the _x_-axis. However, the above command line results in an error! `ADDA` detects almost all possible errors in input parameters and produces informative error messages. Read carefully the error message.

10. `ADDA` uses default _m_ = 1.5 if it is not specified, but only for single-domain scatterers. Hence, you must specify _m_ for both core and outer spheres:
```
adda –size 6 –shape coated 0.5 0.2 0 0 –m 1.05 0 1.2 0
```

Now the simulation goes on fluently, but you see the output of two instances of the iterative solver. That is because the scatterer is not spherically symmetric, for the first time in the tutorial. In this general case scattering of two incident polarizations should be simulated to get the Mueller matrix. The good thing about `ADDA` is that it cuts the simulation time twice for scatterers that are symmetric with regard to rotation by 90° over the _z_-axis.

Look inside the last output directory. There you will find an additional file `CrossSec-X`, which contains integral scattering quantities for incident _x_-polarization.

11. Usually, _C_<sub>sca</sub> is calculated as _C_<sub>ext</sub> − _C_<sub>abs</sub>. However, in some cases it is desirable to calculate it directly by integrating the amplitude of the scattered field over the solid angle:
```
adda -Csca
```
The parameters of integration are specified in the file `alldir_params.dat`, but it is not considered in the tutorial – the default parameters are used. `ADDA` uses the [Romberg integration](http://en.wikipedia.org/wiki/Romberg%27s_method) that provides the estimation of the integration error, which is shown in the output. This error is extremely small because of the spherical symmetry of the scatterer. However, if you look at the values of _C_<sub>ext</sub> and _C_<sub>sca</sub>, which should be equal for non-absorbing particles, relative difference between them is a few orders of magnitude larger than this error. That is because the iterative solver itself introduces an error, which is about 10<sup>–5</sup> by default.

12. Decrease the required _ε_ to 10<sup>–10</sup>:
```
adda –Csca –eps 10
```
The simulation time becomes about 1.5 times longer and results in _C_<sub>ext</sub> and _C_<sub>sca</sub> being identical up to all shown digits.

13. Now switch to the orientation averaging. To make it non-trivial consider a general ellipsoid. First read the information on the predefined shape `ellipsoid`:
```
adda –h shape ellipsoid
```

14. Then simulate light scattering by an ellipsoid averaged over all possible orientations. Consider a small one to make it fast (computational grid 4×8×12 dipoles, dpl = 15):
```
adda -grid 4 -shape ellipsoid 2 3 -orient avg
```
The simulation takes some time producing a lot of output (it shows the convergence progress of each instance of the iterative solver). Consider the final part of this output. It shows the error estimate of averaging over the _α_ Euler angle, which is done in a single DDA run. Then the status of the convergence for two other Euler angles is shown. They both converged to the relative error of 10<sup>–3</sup> (measured for _Q_<sub>ext</sub>). This value together with other parameters for orientation averaging is specified in the file `avg_params.dat`, which is also not considered in the tutorial.

The powerful part of orientation averaging in `ADDA` is its adaptability. It reaches the specified accuracy using the minimum number of single DDA simulations (this number is shown in the output). More detailed information on the orientation averaging can be found in the file `log_orient_avg` in the output folder.

15. One of the “jewels” in `ADDA` is the granule generator. You may never use it in the future, but it is extremely powerful for certain applications. Suppose you want to simulate light scattering by a sphere randomly filled with small spherical granules. You need only to specify diameters of the sphere and the granules, volume fraction of the granules, and two refractive indices:
```
adda –size 16 –granul 0.3 1 –m 1.05 0 1.2 0
```
This corresponds to granules with size parameter 0.5, volume fraction 0.3, and _m_ = 1.2 in a sphere with _x_ = 8 and _m_ = 1.05. You can find some information about the progress of random granule placement in the output.

This tutorial covers only the basics of using `ADDA` and a few selected advanced features. At the same time it does not consider a lot of other [Features](Features.md), for example: parallel execution enabling handling very large particles, Gaussian incident beam, different iterative solvers and prescriptions for polarizability, using symmetry of the scatterer to accelerate orientation averaging, checkpoint system, and calculation of scattering for an arbitrary set of scattering angles. If you are not yet fed up with `ADDA`, here is a final task:

16. Calculate _Q_<sub>ext</sub> for a spherical cell with diameter 2 µm, placed in the origin. Inside the cell there is a spherical nucleus with diameter 1 µm, centered in (0,0,0.2) µm. The outer shell (cytoplasm) is filled with granules of diameter 0.2 µm and volume fraction 0.2. The refractive indices of the cytoplasm, the nucleus, and the granules are 1.02, 1.08, and 1.2 respectively. The incident wavelength is 0.4936 µm. There is no command line provided here, however, you are encouraged to use `ADDA` help system or the [manual](http://a-dda.googlecode.com/svn/trunk/doc/manual.pdf). If you do it correctly, you should get _Q_<sub>ext</sub> in the range 1.146 ± 0.029 (95% confidence interval). Good luck!