Here "reflection" denotes the part of the total Green's tensor for two interacting dipoles, which comes from the surface. So if you want to implement a new formulation of the total Green's tensor, you may also consider the [direct interaction](AddingInteraction.md).

All places in the code, where something should be added, are commented. These comments start with "`TO ADD NEW REFLECTION FORMULATION`" and contain detailed description; the text below is less detailed. Added code should be analogous to reflection formulations already present. The procedure is the following:
  * Add new reflection term descriptor in the file [const.h](http://code.google.com/p/a-dda/source/browse/trunk/src/const.h). Starting descriptor name with `GR_` is recommended, as well as adding a descriptive comment.
  * Add information about the new reflection formulation in the file [param.c](http://code.google.com/p/a-dda/source/browse/trunk/src/param.c).
    * Modify string constants after `PAR(int_surf)` in the constant array `options`: add new argument to list `{...}` and its description to the next string.
    * Add one line to the else-if sequence in the function `PARSE_FUNC(int_surf)`. If new formulation allows sub-arguments, process them here.
    * Take a look at the logic for interrelation between reflection formulations and perfectly conducting surface in function `VariablesInterconnect` and revise it, if needed.
    * Add a case to the relevant switch in the function `PrintInfo`.
  * Main part of the code should be added to [interaction.c](http://code.google.com/p/a-dda/source/browse/trunk/src/interaction.c).
    * Add functions that actually perform the calculation of the reflection term, according to the new formulae. At the end you need to have two functions according to the declarations `ReflTerm_int` and `ReflTerm_real` in [interaction.h](http://code.google.com/p/a-dda/source/browse/trunk/src/interaction.h). There are two ways to proceed:
      * either (_recommended_) create one main function with the following declaration
```
static inline void ReflTerm_<name>(double qvec[static 3],doublecomplex result[static 6],const bool unitsGrid)
```
> > > which works for real input vector and accepts additional bool argument, specifying the units. After the function definition you specify `WRAPPERS_REFL(InterTerm_<name>)`, which automatically produces wrappers compatible with declarations in [interaction.h](http://code.google.com/p/a-dda/source/browse/trunk/src/interaction.h).
      * or create two separate functions named `ReflTerm_<name>_int` and `ReflTerm_<name>_real`. If the new formulation does not support arbitrary real input vector (e.g. it is based on tables), then use `NO_REAL_WRAPPER(ReflTerm_<name>)` instead of the real-input declaration.
    * Update assignment of function pointers in function `InitInteraction`. If new formulation requires initialization, add it here as well, preferably by a separate function. Also test for possible incompatibilities with other `ADDA` parameters, if any, and add an exception.
    * If you allocate any memory (for tables, etc.), free it in function `FreeInteraction`.

Please refer to section _Interaction Term_ of the [manual](http://a-dda.googlecode.com/svn/trunk/doc/manual.pdf) for general overview and precise definition of the reflection term inside `ADDA`.

If you add a new refletion formulation to `ADDA` according to the described procedure, please consider [contributing your code](InstructionCommitters.md) to be incorporated in future releases.