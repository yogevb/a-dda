All places in the code, where something should be added, are commented. These comments start with "`TO ADD NEW INTERACTION FORMULATION`" and contain detailed description; the text below is less detailed. Added code should be analogous to interaction formulations already present. The procedure is the following:
  * Add new interaction term descriptor in the file [const.h](http://code.google.com/p/a-dda/source/browse/trunk/src/const.h). Starting descriptor name with `G_` is recommended, as well as adding a descriptive comment.
  * Add information about the new interaction formulation in the file [param.c](http://code.google.com/p/a-dda/source/browse/trunk/src/param.c).
    * Modify string constants after `PAR(int)` in the constant array `options`: add new argument to list `{...}` and its description to the next string.
    * Add one line to the else-if sequence in the function `PARSE_FUNC(int)`. If new formulation allows sub-arguments, process them here.
    * If the new Green's tensor is non-symmetric (very unlikely) update the definition of `reduced_FFT` in function `VariablesInterconnect` accordingly.
    * Add a case to the relevant switch in the function `PrintInfo`.
  * Main part of the code should be added to [interaction.c](http://code.google.com/p/a-dda/source/browse/trunk/src/interaction.c).
    * Add functions that actually perform the calculation of the interaction term, according to the new formulae. At the end you need to have two functions according to the declarations `InterTerm_int` and `InterTerm_real` in [interaction.h](http://code.google.com/p/a-dda/source/browse/trunk/src/interaction.h). There are two ways to proceed:
      * either (_recommended_) create one main function with the following declaration
```
static inline void InterTerm_<name>(double qvec[static 3],doublecomplex result[static 6],const bool unitsGrid)
```
> > > which works for real input vector and accepts additional bool argument, specifying the units. After the function definition you specify `WRAPPERS_INTER(InterTerm_<name>)`, which automatically produces wrappers compatible with declarations in [interaction.h](http://code.google.com/p/a-dda/source/browse/trunk/src/interaction.h).
      * or create two separate functions named `InterTerm_<name>_int` and `InterTerm_<name>_real`. If the new formulation does not support arbitrary real input vector (e.g. it is based on tables), then use `NO_REAL_WRAPPER(InterTerm_<name>)` instead of the real-input declaration.
    * Update assignment of function pointers in function `InitInteraction`. If new formulation requires initialization, add it here as well, preferably by a separate function. Also test for possible incompatibilities with other `ADDA` parameters, if any, and add an exception.
    * If you allocate any memory (for tables, etc.), free it in function `FreeInteraction`.

Please refer to section _Interaction Term_ of the [manual](http://a-dda.googlecode.com/svn/trunk/doc/manual.pdf) for general overview and precise definition of the interaction term inside `ADDA`.

If you add a new interaction formulation to `ADDA` according to the described procedure, please consider [contributing your code](InstructionCommitters.md) to be incorporated in future releases. You may also consider generalizing the new formulation for particles near surface, and [adding a new reflection formulation](AddingReflection.md).