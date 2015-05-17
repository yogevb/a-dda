All places in the code, where something should be added, are commented. These comments start with "`TO ADD POLARIZABILITY FORMULATION`" and contain detailed description; the text below is less detailed. Added code should be analogous to polarizability formulations already present. The procedure is the following:
  * Add new polarizability descriptor in the file [const.h](http://code.google.com/p/a-dda/source/browse/trunk/src/const.h). Starting descriptor name with `POL_` is recommended, as well as adding a descriptive comment.
  * Add information about the new interaction formulation in the file [param.c](http://code.google.com/p/a-dda/source/browse/trunk/src/param.c).
    * Modify string constants after `PAR(pol)` in the constant array `options`: add new argument to list `{...}` and its description to the next string.
    * Add one line to the else-if sequence in the function `PARSE_FUNC(pol)`. If new formulation allows sub-arguments, process them here.
    * If the new polarizability formulation is incompatible with anisotropic material, add an exception in function `VariablesInterconnect`.
    * Add a case to the relevant switch in the function `PrintInfo`.
  * Main part of the code should be added to [calculator.c](http://code.google.com/p/a-dda/source/browse/trunk/src/calculator.c).
    * The main logic should be placed in function `CoupleConstant`. The corresponding case should either be added to `asym` list (then three components of polarizability are calculated from one refractive index) or to another one, then a scalar function is used.
    * If new formulation depends on the incident polarization (unlikely) update the test in function `calculate_one_orientation`.

Please refer to section _Polarizability prescription_ of the [manual](http://a-dda.googlecode.com/svn/trunk/doc/manual.pdf) for general overview and precise definition of the dipole polarizability inside `ADDA`.

If you add a new polarizability formulation to `ADDA` according to the described procedure, please consider [contributing your code](InstructionCommitters.md) to be incorporated in future releases.