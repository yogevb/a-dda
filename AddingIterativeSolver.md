All places in the code, where something should be added, are commented. These comments start with "`TO ADD NEW ITERATIVE SOLVER`" and contain detailed description; the text below is less detailed. Added code should be analogous to iterative solvers already present. The procedure is the following:
  * Add new iterative-solver descriptor in the file [const.h](http://code.google.com/p/a-dda/source/browse/trunk/src/const.h). Starting descriptor name with `IT_` is recommended, as well as adding a descriptive comment.
  * Add information about the new iterative solver in the file [param.c](http://code.google.com/p/a-dda/source/browse/trunk/src/param.c).
    * Add a short name of the iterative solver, to be used in the command line, to the list inside `{...}` on the line starting with `PAR(iter)` in the constant array `options`.
    * Add one line to the else-if sequence in the function `PARSE_FUNC(iter)`.
    * If the iterative solver requires computation of product of Hermitian transpose of the matrix with vector, then update the exception for MPI mode in function `VariablesInterconnect` accordingly.
    * If the iterative solver requires computation of the norm of the matrix-vector product together with the product itself (during the call to `MatVec` function), then update the assignment of `ipr_required` in function `VariablesInterconnect` accordingly.
    * Add a case to the relevant switch in the function `PrintInfo`.
  * Main part of the code should be added to [iterative.c](http://code.google.com/p/a-dda/source/browse/trunk/src/iterative.c).
    * Add a function prototype `ITER_FUNC(_name_);`  to the existing list of prototypes. `ITER_FUNC(…)` is a macro for conciseness.
    * Add an element to the structure `params` consisting of:
      * descriptor specified in [const.h](http://code.google.com/p/a-dda/source/browse/trunk/src/const.h) (see above),
      * maximum allowed number of iterations without the residual decrease,
      * numbers of additional scalars and vectors required to describe the state of the iterative solver (see comments in the code),
      * name of a function, implementing the method (denoted by `_name_` above).
    * Implement the function among the list of functions definitions for existing iterative solvers. For operations on complex numbers you are advised to use functions from [cmplx.h](http://code.google.com/p/a-dda/source/browse/trunk/src/cmplx.h), for switching vectors - function `SwapPointers`, for linear algebra - functions from [linalg.c](http://code.google.com/p/a-dda/source/browse/trunk/src/linalg.c), for multiplication of vector with matrix of the linear system - function `MatVec` from [matvec.c](http://code.google.com/p/a-dda/source/browse/trunk/src/matvec.c). A short template for the function is the following:
```
ITER_FUNC(_name_)
// Short comment, providing full name of the iterative solver
{
// It is recommended to define all nontrivial constants here
#define EPS1 1E-30
	// all internal variables should be defined here as static, since the function will be called many times
	static double xxx;

	// The function accepts a single argument 'ph' describing a current phase to execute
	switch (ph) {
		case PHASE_VARS:
			// Here variables are linked to structure arrays 'scalars' and 'vectors'. For example:
			scalars[0].ptr=&xxx;
			scalars[0].size=sizeof(double);
			// Also, if auxiliary vectors vec1,... are used, their names may be changed
			return;
		case PHASE_INIT:
			// Initialization of the iterative solver
			return;
		case PHASE_ITER:
			// Performs a general iteration. As a result, current residual should be calculated

			// an example for checking of convergence failure (optional)
			if (xxx<EPS1) LogError(ONE_POS,"_name_ fails: xxx is too small ("GFORM_DEBUG").",xxx);

			return;
	}
	LogError(ONE_POS,"Unknown phase (%d) of the iterative solver",(int)ph);
#undef EPS1
}
```
  * If the new iterative solver uses extra vectors (like `vec1`, etc.) you should change the code in [calculator.c](http://code.google.com/p/a-dda/source/browse/trunk/src/calculator.c)
    * Update the code for allocation of `vec1`, etc. in the function `AllocateEverything`.
    * Analogously update the code for freeing those vectors in the function `FreeEverything`.


Please refer to section _Iterative Solvers_ of the [manual](http://a-dda.googlecode.com/svn/trunk/doc/manual.pdf) for general overview of how the iterative solvers operate inside `ADDA`.

If you add a new iterative solver to `ADDA` according to the described procedure, please consider [contributing your code](InstructionCommitters.md) to be incorporated in future releases.