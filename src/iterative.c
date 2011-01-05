/* File: iterative.c
 * $Author$
 * $Date::                            $
 * Descr: a few iterative techniques to solve DDA equations; currently CGNR,BiCGStab,BiCG-CS,QMR-CS
 *        are implemented
 *
 *        CGNR and BiCGStab are based on "Templates for the Solution of Linear Systems: Building
 *        Blocks for Iterative Methods", http://www.netlib.org/templates/Templates.html .
 *
 *        BiCG-CS and QMR-CS are based on: Freund R.W. "Conjugate gradient-type methods for linear
 *        systems with complex symmetric coefficient matrices", SIAM Journal of Scientific
 *        Statistics and Computation, 13(1):425-448, 1992.
 *
 *        BiCG-CS is identical to COCG, described in: van der Vorst H.A., Melissen J.B.M. "A
 *        Petrov-Galerkin type method for solving Ax=b, where A is symmetric complex", IEEE
 *        Transactions on Magnetics, 26(2):706-708, 1990.
 *
 *        CGNR was first implemented by Alfons Hoekstra.
 *
 *        The linear system is composed so that diagonal terms are equal to 1, therefore use of
 *        Jacobi preconditioners does not have any effect.
 *
 *        CS methods still converge to the right result even when matrix is slightly non-symmetric
 *        (e.g. -int so), however they do it much slowly than usually. It is recommended then to use
 *        BiCGStab.
 *
 * Copyright (C) 2006-2008 University of Amsterdam
 * Copyright (C) 2009,2010 Institute of Chemical Kinetics and Combustion & University of Amsterdam
 * This file is part of ADDA.
 *
 * ADDA is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * ADDA is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with ADDA. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <time.h> // for time_t & time
#include <string.h>
#include <math.h>
#include "vars.h"
#include "cmplx.h"
#include "const.h"
#include "comm.h"
#include "linalg.h"
#include "io.h"
#include "timing.h"
#include "function.h"
#include "debug.h"
#include "memory.h"

// SEMI-GLOBAL VARIABLES

// defined and initialized in CalculateE.c
extern const TIME_TYPE tstart_CE;
// defined and initialized in calculator.c
extern doublecomplex *rvec; // can't be declared restrict due to SwapPointers
extern doublecomplex * restrict vec1,* restrict vec2,* restrict vec3,* restrict Avecbuffer;
// defined and initialized in param.c
extern const double eps;
extern enum init_field InitField;
// defined and initialized in timing.c
extern TIME_TYPE Timing_OneIter,Timing_OneIterComm,Timing_InitIter,Timing_InitIterComm,
	Timing_IntFieldOneComm;
extern size_t TotalIter;

// LOCAL VARIABLES

#define RESID_STRING "RE_%03d = "EFORM // string containing residual value
#define FFORM_PROG "% .6f"  // format for progress value

static double inprodR;     // used as |r_0|^2 and best squared norm of residual up to some iteration
static double inprodRp1;   // used as |r_k+1|^2 and squared norm of current residual
static double epsB;        // stopping criterion
static double resid_scale; // scale to get square of relative error
  // previous relative error; used in ProgressReport, initialized in IterativeSolver
static double prev_err;
static int ind_m;          // index of iterative method
static int niter;          // iteration count
static int counter;        // number of successive iterations without residual decrease
static bool chp_exit;      // checkpoint occurred - exit
static bool complete;      // complete iteration was performed (not stopped in the middle)
typedef struct // data for checkpoints
{
	void *ptr; // pointer to the data
	int size;  // size of one element
} chp_data;
chp_data * restrict scalars,* restrict vectors;
enum phase {
	PHASE_VARS, // Initialization of variables, and linking them to scalars and vectors
	PHASE_INIT, // Initialization of iterations (after loading checkpoint)
	PHASE_ITER  // Each iteration
};
struct iter_params_struct {
	enum iter meth;   // identifier
	int mc;           // maximum allowed number of iterations without residual decrease
	int sc_N;         // number of additional scalars to describe the state
	int vec_N;        // number of additional vectors to describe the state
	void (*func)(const enum phase); // pointer to implementation of the iterative solver
};

#define ITER_FUNC(name) static void name(const enum phase ph)

ITER_FUNC(CGNR);
ITER_FUNC(BiCG_CS);
ITER_FUNC(BiCGStab);
ITER_FUNC(QMR_CS);
/* TO ADD NEW ITERATIVE SOLVER
 * Add the line to this list in the alphabetical order, analogous to the ones already present. The
 * variable part is the name of the function, implementing the method. The macros expands to a
 * function prototype.
 */

static const struct iter_params_struct params[]={
	{IT_BICG_CS,50000,1,0,BiCG_CS},
	{IT_BICGSTAB,30000,3,3,BiCGStab},
	{IT_CGNR,10,1,0,CGNR},
	{IT_QMR_CS,50000,8,3,QMR_CS}
	/* TO ADD NEW ITERATIVE SOLVER
	 * Add its parameters to this list in the alphabetical order. The parameters, in order of
	 * appearance, are identifier (specified in const.h), maximum allowed number of iterations
	 * without the residual decrease, numbers of additional scalars and vectors to describe the
	 * state of the iterative solver (see comment before function SaveCheckpoint), and name of a
	 * function, implementing the method.
	 */
};

// EXTERNAL FUNCTIONS

// matvec.c
void MatVec(doublecomplex * restrict in,doublecomplex * restrict out,double * inprod,bool her,
	TIME_TYPE *comm_timing);

//============================================================

INLINE void SwapPointers(doublecomplex **a,doublecomplex **b)
/* swap two pointers of (doublecomplex *) type; should work for others but will give
 * "Suspicious pointer conversion" warning.
 * While this is a convenient function that can save some copying between memory blocks, it doesn't
 * allow consistent usage of 'restrict' keyword for affected pointers. This may hamper some
 * optimization. Hopefully, the most important optimizations are those in the linalg.c, which
 * can be improved by using 'restrict' keyword in the functions themselves.
 */
{
	doublecomplex *tmp;

	tmp=*a;
	*a=*b;
	*b=tmp;
}

//============================================================

/* Checkpoint systems saves the current state of the iterative solver to the file. By default (for
 * every iterative solver) a number of scalars and vectors are saved. The scalars include, among
 * others, inprodR. There are 4 default vectors: rvec, rvec, pvec, and Avecbuffer. If the iterative
 * solver requires any other scalars or vectors to describe its state, this information should be
 * specified in structure arrays 'scalars' and 'vectors'.
 */

static void SaveIterChpoint(void)
/* save a binary checkpoint; only limitedly foolproof - user should take care to load checkpoints
 * on the same machine (number of processors) and with the same command line.
 */
{
	int i;
	char fname[MAX_FNAME];
	FILE * restrict chp_file;
	TIME_TYPE tstart;

	tstart=GET_TIME();
	if (IFROOT) {
		// create directory "chp_dir" if needed and open info file
		SnprintfErr(ONE_POS,fname,MAX_FNAME,"%s/"F_CHP_LOG,chp_dir);
		if ((chp_file=fopen(fname,"w"))==NULL) {
			MkDirErr(chp_dir,ONE_POS);
			chp_file=FOpenErr(fname,"w",ONE_POS);
		}
		// write info and close file
		fprintf(chp_file,
			"Info about the run, which produced the checkpoint, can be found in ../%s",directory);
		FCloseErr(chp_file,fname,ONE_POS);
	}
	// wait to ensure that directory exists
	Synchronize();
	// open output file; writing errors are checked only for vectors
	SnprintfErr(ALL_POS,fname,MAX_FNAME,"%s/"F_CHP,chp_dir,ringid);
	chp_file=FOpenErr(fname,"wb",ALL_POS);
	// write common scalars
	fwrite(&ind_m,sizeof(int),1,chp_file);
	fwrite(&nlocalRows,sizeof(size_t),1,chp_file);
	fwrite(&niter,sizeof(int),1,chp_file);
	fwrite(&counter,sizeof(int),1,chp_file);
	fwrite(&inprodR,sizeof(double),1,chp_file);
	fwrite(&prev_err,sizeof(double),1,chp_file); // written on ALL processors but used only on root
	fwrite(&resid_scale,sizeof(double),1,chp_file);
	// write specific scalars
	for (i=0;i<params[ind_m].sc_N;i++) fwrite(scalars[i].ptr,scalars[i].size,1,chp_file);
	// write common vectors
	if (fwrite(xvec,sizeof(doublecomplex),nlocalRows,chp_file)!=nlocalRows)
		LogError(ALL_POS,"Failed writing to file '%s'",fname);
	if (fwrite(rvec,sizeof(doublecomplex),nlocalRows,chp_file)!=nlocalRows)
		LogError(ALL_POS,"Failed writing to file '%s'",fname);
	if (fwrite(pvec,sizeof(doublecomplex),nlocalRows,chp_file)!=nlocalRows)
		LogError(ALL_POS,"Failed writing to file '%s'",fname);
	if (fwrite(Avecbuffer,sizeof(doublecomplex),nlocalRows,chp_file)!=nlocalRows)
		LogError(ALL_POS,"Failed writing to file '%s'",fname);
	// write specific vectors
	for (i=0;i<params[ind_m].vec_N;i++)
		if (fwrite(vectors[i].ptr,vectors[i].size,nlocalRows,chp_file)!=nlocalRows)
			LogError(ALL_POS,"Failed writing to file '%s'",fname);
	// close file
	FCloseErr(chp_file,fname,ALL_POS);
	// write info to logfile after everyone is finished
	Synchronize();
	if (IFROOT) PrintBoth(logfile,"Checkpoint (iteration) saved\n");
	Timing_FileIO+=GET_TIME()-tstart;
	Synchronize(); // this is to ensure that message above appears if and only if OK
}

//============================================================

static void LoadIterChpoint(void)
/* load a binary checkpoint; only limitedly foolproof - user should take care to load checkpoints
 * on the same machine (number of processors) and with the same command line.
 * */
{
	int i;
	int ind_m_new;
	size_t nlocalRows_new;
	char fname[MAX_FNAME],ch;
	FILE * restrict chp_file;
	TIME_TYPE tstart;

	tstart=GET_TIME();
	// open input file; reading errors are checked only for vectors
	SnprintfErr(ALL_POS,fname,MAX_FNAME,"%s/"F_CHP,chp_dir,ringid);
	chp_file=FOpenErr(fname,"rb",ALL_POS);
	/* check for consistency. This implies that the same index corresponds to the same iterative
	 * solver in list params. So if the ADDA executable was changed, e.g. by adding a new iterative
	 * solver, between writing and reading checkpoint, this test may fail.
	 */
	fread(&ind_m_new,sizeof(int),1,chp_file);
	if (ind_m_new!=ind_m) LogError(ALL_POS,"File '%s' is for different iterative method",fname);
	fread(&nlocalRows_new,sizeof(size_t),1,chp_file);
	if (nlocalRows_new!=nlocalRows)
		LogError(ALL_POS,"File '%s' is for different vector size",fname);
	// read common scalars
	fread(&niter,sizeof(int),1,chp_file);
	fread(&counter,sizeof(int),1,chp_file);
	fread(&inprodR,sizeof(double),1,chp_file);
	fread(&prev_err,sizeof(double),1,chp_file); // read on ALL processors but used only on root
	fread(&resid_scale,sizeof(double),1,chp_file);
	// read specific scalars
	for (i=0;i<params[ind_m].sc_N;i++)
		fread(scalars[i].ptr,scalars[i].size,1,chp_file);
	// read common vectors
	if (fread(xvec,sizeof(doublecomplex),nlocalRows,chp_file)!=nlocalRows)
		LogError(ALL_POS,"Failed reading from file '%s'",fname);
	if (fread(rvec,sizeof(doublecomplex),nlocalRows,chp_file)!=nlocalRows)
		LogError(ALL_POS,"Failed reading from file '%s'",fname);
	if (fread(pvec,sizeof(doublecomplex),nlocalRows,chp_file)!=nlocalRows)
		LogError(ALL_POS,"Failed reading from file '%s'",fname);
	if (fread(Avecbuffer,sizeof(doublecomplex),nlocalRows,chp_file)!=nlocalRows)
		LogError(ALL_POS,"Failed reading from file '%s'",fname);
	// read specific vectors
	for (i=0;i<params[ind_m].vec_N;i++)
		if (fread(vectors[i].ptr,vectors[i].size,nlocalRows,chp_file)!=nlocalRows)
			LogError(ALL_POS,"Failed reading from file '%s'",fname);
	// check if EOF reached and close file
	if(fread(&ch,1,1,chp_file)!=0) LogError(ALL_POS,"File '%s' is too long",fname);
	FCloseErr(chp_file,fname,ALL_POS);
	// initialize auxiliary variables
	epsB=eps*eps/resid_scale;
	// print info
	if (IFROOT) {
		PrintBoth(logfile,"Checkpoint (iteration) loaded\n");
		// if residual is stagnating print info about last minimum
		if (counter!=0) fprintf(logfile,
			"Residual has been stagnating already for %d iterations since:\n"
			RESID_STRING"\n...\n",counter,niter-counter-1,sqrt(resid_scale*inprodR));
	}
	Timing_FileIO+=GET_TIME()-tstart;
}

//============================================================

static void ProgressReport(void)
// Do common procedures; show progress in logfile and stdout; also check for checkpoint condition
{
	double err,progr,elapsed;
	char progr_string[MAX_LINE];
	char temp[5];
	time_t wt;

	if (inprodRp1<=inprodR) {
		inprodR=inprodRp1;
		counter=0;
	}
	else counter++;
	if (IFROOT) {
		err=sqrt(resid_scale*inprodRp1);
		progr=1-err/prev_err;
		if (counter==0) strcpy(temp,"+ ");
		else if (progr>0) strcpy(temp,"-+");
		else strcpy(temp,"- ");
		SnprintfErr(ONE_POS,progr_string,MAX_LINE,RESID_STRING"  %s",niter,err,temp);
		if (!orient_avg) fprintf(logfile,"%s  progress ="FFORM_PROG"\n",progr_string,progr);
		printf("%s\n",progr_string);
		prev_err=err;
	}
	niter++;
	TotalIter++;
	// check condition for checkpoint; checkpoint is saved at first time
	if (chp_type!=CHP_NONE && chp_time!=UNDEF && complete) {
		time(&wt);
		elapsed=difftime(wt,last_chp_wt);
		if (chp_time<elapsed) {
			SaveIterChpoint();
			time(&last_chp_wt);
			if (chp_type!=CHP_REGULAR) chp_exit=true;
		}
	}
}

//============================================================

ITER_FUNC(BiCG_CS)
// Bi-Conjugate Gradient for Complex Symmetric systems
{
#define EPS1L 1E-10 // for (rT.r)/(r.r), low bound
#define EPS1H 1E+10 // for (rT.r)/(r.r), high bound (indicates residual increase)
#define EPS2  1E-10 // for (pT.A.p)/(rT.r)
	static doublecomplex alpha, mu;
	static doublecomplex beta,ro_new,ro_old,temp;
	static double dtmp,abs_ro_new;

	if (ph==PHASE_VARS) {
		scalars[0].ptr=&ro_old;
		scalars[0].size=sizeof(doublecomplex);
	}
	else if (ph==PHASE_INIT); // no specific initialization required
	// main iteration cycle
	else if (ph==PHASE_ITER) {
		// ro_k-1=r_k-1(*).r_k-1; check for ro_k-1!=0 (and very high values as well)
		nDotProdSelf_conj(rvec,ro_new,&Timing_OneIterComm);
		abs_ro_new=cAbs(ro_new);
		dtmp=abs_ro_new/inprodR;
		Dz("(rT.r)/(r.r)="GFORM_DEBUG,dtmp);
		if (dtmp<EPS1L || dtmp>EPS1H) LogError(ONE_POS,
			"BiCG_CS fails: (rT.r)/(r.r) is out of bounds ("GFORM_DEBUG").",dtmp);
		if (niter==1) nCopy(pvec,rvec); // p_1=r_0
		else {
			// beta_k-1=ro_k-1/ro_k-2
			cDiv(ro_new,ro_old,beta);
			// p_k=beta_k-1*p_k-1+r_k-1
			nIncrem10_cmplx(pvec,rvec,beta,NULL,&Timing_OneIterComm);
		}
		// q_k=Avecbuffer=A.p_k
		MatVec(pvec,Avecbuffer,NULL,false,&Timing_OneIterComm);
		// mu_k=p_k.q_k; check for mu_k!=0
		nDotProd_conj(pvec,Avecbuffer,mu,&Timing_OneIterComm);
		dtmp=cAbs(mu)/abs_ro_new;
		Dz("(pT.A.p)/(rT.r)="GFORM_DEBUG,dtmp);
		if (dtmp<EPS2) LogError(ONE_POS,
			"BiCG_CS fails: (pT.A.p)/(rT.r) is too small ("GFORM_DEBUG").",dtmp);
		// alpha_k=ro_k/mu_k
		cDiv(ro_new,mu,alpha);
		// x_k=x_k-1+alpha_k*p_k
		nIncrem01_cmplx(xvec,pvec,alpha,NULL,&Timing_OneIterComm);
		// r_k=r_k-1-alpha_k*A.p_k and |r_k|^2
		cInvSign2(alpha,temp);
		nIncrem01_cmplx(rvec,Avecbuffer,temp,&inprodRp1,&Timing_OneIterComm);
		// initialize ro_old -> ro_k-2 for next iteration
		cEqual(ro_new,ro_old);
	} // end of PHASE_ITER
	else LogError(ONE_POS,"Unknown phase of the iterative solver");
}
#undef EPS1L
#undef EPS1H
#undef EPS2

//============================================================

ITER_FUNC(BiCGStab)
// Bi-Conjugate Gradient Stabilized
{
#define EPS1 1E-16 // for (r~.r)/(r.r)
#define EPS2 1E-10 // for 1/|beta_k|
	static double denumOmega,dtmp;
	static doublecomplex beta,ro_new,ro_old,omega,alpha,temp1,temp2;
	static doublecomplex * restrict v,* restrict s,* restrict rtilda;

	if (ph==PHASE_VARS) {
		/* rename some vectors; this doesn't contradict with 'restrict' keyword, since new
		 * names are not used together with old names
		 */
		v=vec1;
		s=vec2;
		rtilda=vec3;
		// initialize data structure for checkpoints
		scalars[0].ptr=&ro_old;
		scalars[1].ptr=&omega;
		scalars[2].ptr=&alpha;
		scalars[0].size=scalars[1].size=scalars[2].size=sizeof(doublecomplex);
		vectors[0].ptr=v;
		vectors[1].ptr=s;
		vectors[2].ptr=rtilda;
		vectors[0].size=vectors[1].size=vectors[2].size=sizeof(doublecomplex);
	}
	else if (ph==PHASE_INIT) {
		if (!load_chpoint) nCopy(rtilda,rvec); // r~=r_0
	}
	else if (ph==PHASE_ITER) {
		// ro_k-1=r_k-1.r~ ; check for ro_k-1!=0
		nDotProd(rvec,rtilda,ro_new,&Timing_OneIterComm);
		dtmp=cAbs(ro_new)/inprodR;
		Dz("(r~.r)/(r.r)="GFORM_DEBUG,dtmp);
		if (dtmp<EPS1) LogError(ONE_POS,
			"BiCGStab fails: (r~.r)/(r.r) is too small ("GFORM_DEBUG").",dtmp);
		if (niter==1) nCopy(pvec,rvec); // p_1=r_0
		else {
			// beta_k-1=(ro_k-1/ro_k-2)*(alpha_k-1/omega_k-1)
			cMult(ro_new,alpha,temp1);
			cMult(ro_old,omega,temp2);
			// check that omega_k-1!=0
			dtmp=cAbs(temp2)/cAbs(temp1);
			Dz("1/|beta_k|="GFORM_DEBUG,dtmp);
			if (dtmp<EPS2) LogError(ONE_POS,
				"Bi-CGStab fails: 1/|beta_k| is too small ("GFORM_DEBUG").",dtmp);
			cDiv(temp1,temp2,beta);
			// p_k=beta_k-1*(p_k-1-omega_k-1*v_k-1)+r_k-1
			cMult(beta,omega,temp1);
			cInvSign(temp1);
			nIncrem110_cmplx(pvec,v,rvec,beta,temp1);
		}
		// calculate v_k=A.p_k
		MatVec(pvec,v,NULL,false,&Timing_OneIterComm);
		// alpha_k=ro_new/(v_k.r~)
		nDotProd(v,rtilda,temp1,&Timing_OneIterComm);
		cDiv(ro_new,temp1,alpha);
		// s=r_k-1-alpha*v_k-1
		cInvSign2(alpha,temp1);
		nLinComb1_cmplx(s,v,rvec,temp1,&inprodRp1,&Timing_OneIterComm);
		// check convergence at this step; if yes, checkpoint should not be saved afterwards
		if (inprodRp1<epsB && chp_type!=CHP_ALWAYS) {
			// x_k=x_k-1+alpha_k*p_k
			nIncrem01_cmplx(xvec,pvec,alpha,NULL,&Timing_OneIterComm);
			complete=false;
		}
		else {
			// t=Avecbuffer=A.s
			MatVec(s,Avecbuffer,&denumOmega,false,&Timing_OneIterComm);
			// omega_k=s.t/|t|^2
			nDotProd(s,Avecbuffer,temp1,&Timing_OneIterComm);
			cMultReal(1/denumOmega,temp1,omega);
			// x_k=x_k-1+alpha_k*p_k+omega_k*s
			nIncrem011_cmplx(xvec,pvec,s,alpha,omega);
			// r_k=s-omega_k*t and |r_k|^2
			cInvSign2(omega,temp1);
			nLinComb1_cmplx(rvec,Avecbuffer,s,temp1,&inprodRp1,&Timing_OneIterComm);
			// initialize ro_old -> ro_k-2 for next iteration
			cEqual(ro_new,ro_old);
		}
	} // end of PHASE_ITER
	else LogError(ONE_POS,"Unknown phase of the iterative solver");
}
#undef EPS1
#undef EPS2

//============================================================

ITER_FUNC(CGNR)
// Conjugate Gradient applied to Normalized Equations with minimization of Residual Norm
{
	static double alpha, denumeratorAlpha;
	static double beta,ro_new,ro_old;

	if (ph==PHASE_VARS) {
		scalars[0].ptr=&ro_old;
		scalars[0].size=sizeof(double);
	}
	else if (ph==PHASE_INIT); // no specific initialization required
	else if (ph==PHASE_ITER) {
		// p_1=Ah.r_0 and ro_new=ro_0=|Ah.r_0|^2
		if (niter==1) MatVec(rvec,pvec,&ro_new,true,&Timing_OneIterComm);
		else {
			// Avecbuffer=AH.r_k-1, ro_new=ro_k-1=|AH.r_k-1|^2
			MatVec(rvec,Avecbuffer,&ro_new,true,&Timing_OneIterComm);
			// beta_k-1=ro_k-1/ro_k-2
			beta=ro_new/ro_old;
			// p_k=beta_k-1*p_k-1+AH.r_k-1
			nIncrem10(pvec,Avecbuffer,beta,NULL,&Timing_OneIterComm);
		}
		// alpha_k=ro_k-1/|A.p_k|^2
		// Avecbuffer=A.p_k
		MatVec(pvec,Avecbuffer,&denumeratorAlpha,false,&Timing_OneIterComm);
		alpha=ro_new/denumeratorAlpha;
		// x_k=x_k-1+alpha_k*p_k
		nIncrem01(xvec,pvec,alpha,NULL,&Timing_OneIterComm);
		// r_k=r_k-1-alpha_k*A.p_k and |r_k|^2
		nIncrem01(rvec,Avecbuffer,-alpha,&inprodRp1,&Timing_OneIterComm);
		// initialize ro_old -> ro_k-2 for next iteration
		ro_old=ro_new;
	}
	else LogError(ONE_POS,"Unknown phase of the iterative solver");
}

//============================================================

ITER_FUNC(QMR_CS)
// Quasi Minimum Residual for Complex Symmetric systems
{
#define EPS1L  1E-10 // for (vT.v)/(b.b), low bound
#define EPS1H  1E+20 // for (vT.v)/(b.b), high bound
#define EPS2   1E-40 // for overflow of exponent number
	static double c_old,c_new,omega_old,omega_new,zetaabs,dtmp1,dtmp2;
	static doublecomplex alpha,beta,theta,eta,zeta,zetatilda,tau,tautilda;
	static doublecomplex s_new,s_old,temp1,temp2,temp3,temp4;
	static doublecomplex *v,*vtilda,*p_new,*p_old; // can't be declared restrict due to SwapPointers

	if (ph==PHASE_VARS) {
		// rename some vectors
		v=vec1;      // v_k
		vtilda=vec2; // also v_k-1
		p_new=pvec;  // p_k
		p_old=vec3;  // p_k-1
		// initialize data structure for checkpoints
		scalars[0].ptr=&omega_old;
		scalars[1].ptr=&omega_new;
		scalars[2].ptr=&c_old;
		scalars[3].ptr=&c_new;
		scalars[4].ptr=&beta;
		scalars[5].ptr=&tautilda;
		scalars[6].ptr=&s_old;
		scalars[7].ptr=&s_new;
		scalars[0].size=scalars[1].size=scalars[2].size=scalars[3].size=sizeof(double);
		scalars[4].size=scalars[5].size=scalars[6].size=scalars[7].size=sizeof(doublecomplex);
		vectors[0].ptr=v;
		vectors[1].ptr=vtilda;
		vectors[2].ptr=p_old;
		vectors[0].size=vectors[1].size=vectors[2].size=sizeof(doublecomplex);
	}
	else if (ph==PHASE_INIT) {
		if (load_chpoint) {
			// change pointers names according to count parity
			if (IS_EVEN(niter)) SwapPointers(&v,&vtilda);
			else SwapPointers(&p_old,&p_new);
		}
		else {
			// omega_0=||v_0||=0
			omega_old=0.0;
			// beta_1=sqrt(v~_1(*).v~_1); omega_1=||v~_1||/|beta_1|; (v~_1=r_0)
			nDotProdSelf_conj(rvec,temp1,&Timing_InitIterComm);
			cSqrt(temp1,beta);
			omega_new=sqrt(inprodR)/cAbs(beta); // inprodR=nNorm2(r_0)
			// v_1=v~_1/beta_1
			cInv(beta,temp1);
			nMult_cmplx(v,rvec,temp1);
			// tau~_1=omega_1*beta_1
			cMultReal(omega_new,beta,tautilda);
			// c_0=c_-1=1; s_0=s_-1=0
			c_new=c_old=1.0;
			s_new[RE]=s_new[IM]=s_old[RE]=s_old[IM]=0.0;
		}
	}
	else if (ph==PHASE_ITER) {
		// check for zero or very high beta
		dtmp1=cAbs2(beta)*resid_scale;
		Dz("(vT.v)/(b.b)="GFORM_DEBUG,dtmp1);
		if (dtmp1<EPS1L || dtmp1>EPS1H) LogError(ONE_POS,
			"QMR_CS fails: (vT.v)/(b.b) is out of bounds ("GFORM_DEBUG").",dtmp1);
		// A.v_k; alpha_k=v_k(*).(A.v_k)
		MatVec(v,Avecbuffer,NULL,false,&Timing_OneIterComm);
		nDotProd_conj(v,Avecbuffer,alpha,&Timing_OneIterComm);
		// v~_k+1=-beta_k*v_k-1-alpha_k*v_k+A.v_k
		cInvSign2(alpha,temp2);
		// use explicitly that v_0=0
		if (niter==1) nLinComb1_cmplx(vtilda,v,Avecbuffer,temp2,NULL,&Timing_OneIterComm);
		else {
			cInvSign2(beta,temp1);
			nIncrem110_cmplx(vtilda,v,Avecbuffer,temp1,temp2);
		}
		// theta_k=s_k-2(*)*omega_k-1*beta_k
		cMultReal(omega_old,beta,temp3); // temp3=omega_k-1*beta_k
		s_old[IM]=-s_old[IM]; // s_old is only used here, hence can be changed
		cMult(s_old,temp3,theta);
		// eta_k=c_k-1*c_k-2*omega_k-1*beta_k+s_k-1(*)*omega_k*alpha_k
		cMultReal(omega_new,alpha,temp4); // temp4=omega_k*alpha_k
		cMultReal(c_old*c_new,temp3,eta);
		cConj(s_new,temp1);
		cMult(temp1,temp4,temp2);
		cAdd(eta,temp2,eta);
		// zeta~_k=c_k-1*omega_k*alpha_k-s_k-1*c_k-2*omega_k-1*beta_k
		cMult(s_new,temp3,temp1);
		cLinComb(temp4,temp1,c_new,-c_old,zetatilda);
		// beta_k+1=sqrt(v~_k+1(*).v~_k+1); omega_k+1=||v~_k+1||/|beta_k+1|
		omega_old=omega_new;
		nDotProdSelf_conj_Norm2(vtilda,temp1,&dtmp1,&Timing_OneIterComm); // dtmp1=||v~||^2
		cSqrt(temp1,beta);
		omega_new=sqrt(dtmp1)/cAbs(beta);
		// |zeta_k|=sqrt(|zeta~_k|^2+omega_k+1^2*|beta_k+1|^2)
		dtmp2=cAbs2(zetatilda); // dtmp2=|zeta~_k|^2
		zetaabs=sqrt(dtmp2+dtmp1);
		dtmp1=sqrt(dtmp2); // dtmp1=|zeta~_k|
		// if (|zeta~_k|==0) zeta_k=|zeta_k|; else zeta=|zeta_k|*zeta~_k/|zeta~_k|
		if (dtmp1<EPS2) {
			zeta[RE]=zetaabs;
			zeta[IM]=0.0;
		}
		else cMultReal(zetaabs/dtmp1,zetatilda,zeta);
		// c_k=zeta~_k/zeta_k = |zeta~_k|/|zeta_k|
		c_old=c_new;
		c_new=dtmp1/zetaabs;
		// s_k+1=omega_k+1*beta_k+1/zeta_k
		cEqual(s_new,s_old);
		cInv(zeta,temp4); // temp4=1/zeta
		cMult(beta,temp4,temp1);
		cMultReal(omega_new,temp1,s_new);
		// p_k=(-theta_k*p_k-2-eta_k*p_k-1+v_k)/zeta_k
		if (niter==1) nMult_cmplx(p_new,v,temp4); // use implicitly that p_0=p_-1=0
		else {
			cMult(eta,temp4,temp2);
			cInvSign(temp2); // temp2=-eta_k/zeta_k
			if (niter==2) nLinComb_cmplx(p_old,p_new,v,temp2,temp4,NULL,&Timing_OneIterComm);
			else {
				cMult(theta,temp4,temp1);
				cInvSign(temp1); // temp1=-theta_k/zeta_k
				nIncrem111_cmplx(p_old,p_new,v,temp1,temp2,temp4);
			}
			SwapPointers(&p_old,&p_new);
		}
		// tau_k=c_k*tau~_k
		cMultReal(c_new,tautilda,tau);
		// tau~_k+1=-s_k*tau~_k
		cMult(s_new,tautilda,temp1);
		cInvSign2(temp1,tautilda);
		// x_k=x_k-1+tau_k*p_k
		nIncrem01_cmplx(xvec,p_new,tau,NULL,&Timing_OneIterComm);
		// v_k+1=v~_k+1/beta_k+1
		cInv(beta,temp1);
		nMultSelf_cmplx(vtilda,temp1);
		SwapPointers(&v,&vtilda); // v~ is as v_k-1 at next iteration
		// r_k=r_k-1+(c_k*tau~_k+1/omega_k+1)*v_k+1
		cMultReal(c_new/omega_new,tautilda,temp1);
		nIncrem11_d_c(rvec,v,cAbs2(s_new),temp1,&inprodRp1,&Timing_OneIterComm);
	} // end of PHASE_ITER
	else LogError(ONE_POS,"Unknown phase of the iterative solver");
}
#undef EPS1L
#undef EPS1H
#undef EPS2

//============================================================

/* TO ADD NEW ITERATIVE SOLVER
 * Add the function implementing the iterative method to the list above in the alphabetical order.
 * The template for the function is provided below together with additional comments. Please also
 * look at the iterative solvers, already present, for examples.
 * For operations on complex numbers you are advised to use functions from cmplx.h, for switching
 * vectors - SwapPointers (above), for linear algebra - functions from linalg.c, for multiplication
 * of vector with matrix of the linear system - MatVec function from matvec.c. Some of these
 * functions take account of the time spent on communication between different processors (in
 * parallel mode), and increment their last argument by the corresponding amount.
 * You may also use  values of variables, defined in the beginning of this source
 * file, especially niter, resid_scale, and epsB.
 */
#if 0
ITER_FUNC(_name_) // only '_name_' should be changed, the macro expansion will do the rest
// Short comment, providing full name of the iterative solver
{
// It is recommended to define all nontrivial constants here. Do not forget to undef them
// at the end of this function to avoid conflicts with other iterative solvers.
#define EPS1 1E-30
	// all internal variables should be defined here as static, since the function will be called
	// many times (once per iteration).
	static double xxx;

	// The function accepts a single argument 'ph' describing a phase, which it should perform at a
	// particular run. This is done to move all common parts to the function IterativeSolver.
	// Possible phases are defined and briefly explained in the definition of 'enum phase' in the
	// beginning of this source file.
	if (ph==PHASE_VARS) {
		// Here variables are linked to structure arrays 'scalars' and 'vectors' to initialize
		// checkpoint system (see comment before function SaveCheckpoint). For example:
		scalars[0].ptr=&xxx;
		scalars[0].size=sizeof(double);
		// Also, if auxiliary vectors vec1,... are used, their names may be changed to a more
		// meaningful ones (using pointer assignments)
	}
	else if (ph==PHASE_INIT) {
		// Initialization of the iterative solver. You may use 'load_chpoint' to distinguish between
		// the plain run and the one restarted from a checkpoint. Actual loading of checkpoint
		// happens just before this phase. For gathering communication time use variable
		// Timing_InitIterComm.
	}
	else if (ph==PHASE_ITER) {
		// Performs a general iteration. As a result, inprodRp1 (current residual) should be
		// calculated. For gathering communication time use variable Timing_OneIterComm.

		if (xxx<EPS1) // an example for checking of convergence failure (optional)
			LogError(ONE_POS,"_name_ fails: xxx is too small ("GFORM_DEBUG").",xxx);

		// _Some_ iterative solvers contain extra checks for convergence in the _middle_ of an
		// iteration, designed to save time of, e.g., one matrix-vector product in some cases.
		// They should be performed as follows. In particular, the intermediate test will be skipped
		// if final checkpoint is required (checkpoint of type 'always').
		if (inprodRp1<epsB && chp_type!=CHP_ALWAYS) {
			// Additional code, e.g. to set xvec to the final value
			complete=false; // this is required to skip saving checkpoint and some timing
		}
		else {
			// Continue the iteration normally
		}
		// Common check for convergence at the end of an iteration should not be done here, because
		// it is performed in the function IterativeSolver.
	}
	else LogError(ONE_POS,"Unknown phase of the iterative solver");
#undef EPS1
}
#endif

#undef ITER_FUNC

//============================================================

void CalcInitField(char *descr,double zero_resid)
/* Initializes the field as the starting point of the iterative solver. Assumes that pvec contains
 * the right-hand side of equations (b). At the end of this function xvec should contain initial
 * vector for the iterative solver (x_0), rvec - corresponding residual r_0, and inprodR - the norm
 * of the latter residual.
 */
{
	if (InitField==IF_AUTO) {
		/* This code is somewhat inelegant, but there seem to be no easy way to completely reuse
		 * code for other cases. Moreover, this option will probably be changed afterwards.
		 */
		// calculate A.(x_0=b), r_0=b-A.(x_0=b) and |r_0|^2
		MatVec(pvec,Avecbuffer,NULL,false,&Timing_InitIterComm);
		nSubtr(rvec,pvec,Avecbuffer,&inprodR,&Timing_InitIterComm);
		// check which x_0 is better
		if (zero_resid<inprodR) { // use x_0=0
			nInit(xvec);
			nCopy(rvec,pvec);
			inprodR=zero_resid;
			strcpy(descr,"x_0 = 0\n");
		}
		else { // use x_0=Einc
			nCopy(xvec,pvec);
			strcpy(descr,"x_0 = E_inc\n");
		}
	}
	if (InitField==IF_ZERO) {
		nInit(xvec); // x_0=0
		nCopy(rvec,pvec); // r_0=b
		inprodR=zero_resid;
		strcpy(descr,"x_0 = 0\n");
	}
	else if (InitField==IF_INC) {
		nCopy(xvec,pvec); // x_0=b
		// calculate A.(x_0=b), r_0=b-A.(x_0=b) and |r_0|^2
		MatVec(xvec,Avecbuffer,NULL,false,&Timing_InitIterComm);
		nSubtr(rvec,pvec,Avecbuffer,&inprodR,&Timing_InitIterComm);
		strcpy(descr,"x_0 = E_inc\n");
	}
	else if (InitField==IF_WKB) {
		if (prop[2]!=1) LogError(ONE_POS,"WKB initial field currently works only with default "
			"incident direction of the incoming wave (along z-axis)");
		doublecomplex vals[Nmat+1],tmpc;
		int i,k; // for traversing single-axis dimensions
		size_t dip,ind,dip_sl; // for traversing slices or up to nlocalRows
		size_t boxX_l=(size_t)boxX; // to remove type conversion in indexing
#define INDEX_GRID(i) (position[(i)+2]*boxXY+position[(i)+1]*boxX_l+position[i])
		/* can be optimized by reusing material_tmp from make_particle.c or keeping the values
		 * between the calls. But this will require usage of extra memory. So the current option
		 * can be considered as corresponding to '-opt mem'
		 */
		unsigned char * restrict mat; // same as material, but defined on whole grid (local_Ndip)
		doublecomplex * restrict arg; // argument of exponent for corrections of incident field
#ifdef PARALLEL
		doublecomplex * restrict bottom; // value of arg at bottom of current processor
#endif
		doublecomplex * restrict top; // propagating value of arg at planes between the dipoles

		// define all vectors using memory assigned to Xmatrix
		arg=Xmatrix;
#ifdef PARALLEL
		bottom=Xmatrix+local_Ndip;
		top=bottom+boxXY;
#else
		top=Xmatrix+local_Ndip;
#endif
		mat=(unsigned char *)(top + boxXY);
		// calculate function of refractive index
		for (i=0;i<Nmat;i++) { // vals[i]=i*(ref_index[i]-1)*kd/2;
			vals[i][IM]=(ref_index[i][RE]-1)*kd/2;
			vals[i][RE]=-ref_index[i][IM]*kd/2;
		}
		vals[Nmat][RE]=vals[Nmat][IM]=0;
		// calculate values of mat (the same algorithm as in matvec), for void dipoles mat=Nmat
		for (dip=0;dip<local_Ndip;dip++) mat[dip]=(unsigned char)Nmat;
		for (dip=0,ind=0;dip<local_nvoid_Ndip;dip++,ind+=3)
			mat[INDEX_GRID(ind)]=material[dip];
		/* main part responsible for calculation of arg;
		 * arg[i,j,k+1]=arg[i,j,k]+vals[i,j,k]+vals[i,j,k+1]
		 * but that is done with temporary variables (not to index both k and k+1 simultaneously
		 *
		 * 'ind' traverses one slice, and 'dip' - all dipoles
		 */
		// First, calculate shifts relative to the bottom of current processor
		for(ind=0;ind<boxXY;ind++) top[ind][RE]=top[ind][IM]=0;
		for(k=local_z0,dip_sl=0;k<local_z1_coer;k++,dip_sl+=boxXY)
			for(ind=0,dip=dip_sl;ind<boxXY;ind++,dip++) {
				cAdd(top[ind],vals[mat[dip]],arg[dip]);
				cAdd(arg[dip],vals[mat[dip]],top[ind]);
		}
#ifdef PARALLEL
		// Second, fulfill boundary by exchanging shift values at top and bottom
		if (ExchangePhaseShifts(bottom,top,&Timing_InitIterComm))
			// Third (if required) update shift from the obtained values on the bottom
			for(k=local_z0,dip_sl=0;k<local_z1_coer;k++,dip_sl+=boxXY)
				for(ind=0,dip=dip_sl;ind<boxXY;ind++,dip++)
					cAdd(arg[dip],bottom[ind],arg[dip]);
#endif
		// xvec=pvec*Exp(arg), but arg is defined on a set of all (including void) dipoles
		for (ind=0;ind<nlocalRows;ind+=3) {
			cExp(arg[INDEX_GRID(ind)],tmpc);
			cvMultScal_cmplx(tmpc,pvec+ind,xvec+ind);
		}
		// calculate A.(x_0=b), r_0=b-A.(x_0=b) and |r_0|^2
		MatVec(xvec,Avecbuffer,NULL,false,&Timing_InitIterComm);
		nSubtr(rvec,pvec,Avecbuffer,&inprodR,&Timing_InitIterComm);
		strcpy(descr,"x_0 = result of WKB\n");
	}
}

//============================================================

int IterativeSolver(const enum iter method_in)
// choose required iterative method; do common initialization part
{
	double temp;
	char tmp_str[MAX_LINE];
	TIME_TYPE tstart,time_tmp;

	// redundant initialization to remove warnings
	time_tmp=0;

	/* Instead of solving system (I+D.C).x=b , C - diagonal matrix with couple constants
	 *                                         D - symmetric interaction matrix of Green's tensor
	 * we solve system (I+S.D.S).(S.x)=(S.b), S=sqrt(C), then total interaction matrix is symmetric
	 * and Jacobi-preconditioned for any distribution of refractive index.
	 */
	/* p=b=(S.Einc) is right part of the linear system; used only here. In iteration methods
	 * themselves p is completely different vector. To avoid confusion this is done before any other
	 * initializations, specific to iterative solvers.
	 */
	Timing_InitIterComm=0;
	if (!load_chpoint) {
		nMult_mat(pvec,Einc,cc_sqrt);
		temp=nNorm2(pvec,&Timing_InitIterComm); // |r_0|^2 when x_0=0
		resid_scale=1/temp;
		epsB=eps*eps*temp;
		// Calculate initial field
		CalcInitField(tmp_str,temp);
		// print start values
		if (IFROOT) {
			prev_err=sqrt(resid_scale*inprodR);
			sprintf(tmp_str+strlen(tmp_str),RESID_STRING"\n",0,prev_err);
			if (!orient_avg) fprintf(logfile,"%s",tmp_str);
			printf("%s",tmp_str);
		}
		// initialize counters
		niter=1;
		counter=0;
	}
	/* determine index of the iterative solver, which is further used to get its parameters from
	 * list 'params'. This way it should be resistant to inconsistencies in orders of iterative
	 * solvers inside the list of identifiers in const.h and in the list 'params' above.
	 */
	ind_m=0;
	while (params[ind_m].meth!=method_in) {
		ind_m++;
		if (ind_m>=LENGTH(params)) LogError(ONE_POS,
			"Parameters for the given iterative solver are not found in list 'params'");
	}
	// initialize data required for checkpoints and specific variables
	chp_exit=false;
	complete=true;
	if (params[ind_m].sc_N>0) scalars=(chp_data *)
		voidVector(params[ind_m].sc_N*sizeof(chp_data),ALL_POS,"list of scalars");
	else scalars=NULL;
	if (params[ind_m].vec_N>0) vectors=(chp_data *)
		voidVector(params[ind_m].vec_N*sizeof(chp_data),ALL_POS,"list of scalars");
	else vectors=NULL;
	(*params[ind_m].func)(PHASE_VARS);
	// load checkpoint, if needed, and finish initialization of the iterative solver
	if (load_chpoint) LoadIterChpoint();
	(*params[ind_m].func)(PHASE_INIT);
	// Initialization time includes generating the incident beam
	Timing_InitIter = GET_TIME() - tstart_CE;
	Timing_IntFieldOneComm=Timing_InitIterComm;
	// main iteration cycle
	while (inprodR>epsB && niter<=maxiter && counter<=params[ind_m].mc && !chp_exit) {
		// initialize time
		Timing_OneIterComm=0;
		tstart=GET_TIME();
		// main execution
		(*params[ind_m].func)(PHASE_ITER);
		// finalize time; time for incomplete iteration may be inadequate
		Timing_IntFieldOneComm+=Timing_OneIterComm;
		if (complete) {
			Timing_OneIter=GET_TIME()-tstart;
			time_tmp=Timing_OneIterComm;
		}
		// use result from the previous iteration (assumed to be available by this time)
		else Timing_OneIterComm=time_tmp;
		/* check progress; it takes negligible time by itself (O(1) operations), but may lead to
		 * saving checkpoint. Since the latter is not relevant to the iteration itself, the
		 * ProgressReport is called after finalizing the time of a single iteration.
		 */
		ProgressReport();
	}
	// Save checkpoint of type always
	if (chp_type==CHP_ALWAYS && !chp_exit) SaveIterChpoint();
	// error output
	if (inprodR>epsB) {
		if (niter>maxiter) LogError(ONE_POS,
			"Iterations haven't converged in maximum allowed number of iterations (%d)",maxiter);
		else if (counter>params[ind_m].mc) LogError(ONE_POS,"Residual norm haven't decreased for "
			"maximum allowed number of iterations (%d)",params[ind_m].mc);
	}
	// post-processing
	if (params[ind_m].sc_N>0) Free_general(scalars);
	if (params[ind_m].vec_N>0) Free_general(vectors);
	/* x is a solution of a modified system, not exactly internal field; should not be used further
	 * except for adaptive technique (as starting vector for next system)
	 */
	nMult_mat(pvec,xvec,cc_sqrt); /* p is now vector of polarizations. Can be used to calculate
	                               * e.g. scattered field faster.
	                               */
	if (chp_exit) return CHP_EXIT; // check if exiting after checkpoint
	return (niter-1); // the number of iterations elapsed
}