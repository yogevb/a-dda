/* File: vars.h
 * $Author$
 * $Date::                            $
 * Descr: all the global variables are declared here
 *
 *        Global means: used in three or more source files. Variables that are used in only two
 *        source files are called 'semi-global' and not listed here. They are defined in one file
 *        and referenced with 'extern' in another one.
 *
 * Copyright (C) 2006-2008 University of Amsterdam
 * Copyright (C) 2009 Institute of Chemical Kinetics and Combustion & University of Amsterdam
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
#ifndef __vars_h
#define __vars_h

#include <stdio.h>   // for FILE and size_t
#include <time.h>    // for time_t
#include <stdbool.h> // for bool
#include "const.h"   // for MAX_NMAT
#include "types.h"   // for doublecomplex, angle_set, scat_grid_angles
#include "timing.h"  // for TIME_TYPE

// basic variables
extern int boxX,boxY,boxZ;
extern double gridspace,kd,ka_eq,inv_G,WaveNum;
extern double *DipoleCoord;
extern unsigned short *position;
extern double memory;
extern enum inter IntRelation;
extern enum pol PolRelation;
extern enum beam beamtype;

// symmetries
extern bool symX,symY,symZ,symR;

// flags
extern bool prognose,yzplane,all_dir,scat_grid,phi_integr,sh_granul,reduced_FFT,orient_avg,
            load_chpoint,beam_asym,anisotropy,save_memory;
// 3D vectors
extern double prop[3],incPolX[3],incPolY[3],beam_center[3],box_origin_unif[3];

// file info
extern char directory[];
extern FILE *logfile;
extern int term_width;

// refractive index
extern int Nmat,Ncomp;
extern doublecomplex ref_index[MAX_NMAT];
extern doublecomplex cc_sqrt[MAX_NMAT][3];
extern unsigned char *material;

// iterative solver
extern enum iter IterMethod;
extern int maxiter;
extern doublecomplex *xvec,*pvec,*Einc;

// scattering at different angles
extern int nTheta;
extern double alph_deg, bet_deg, gam_deg;
extern angle_set alpha_int;
extern scat_grid_angles angles;
extern doublecomplex *EgridX,*EgridY;
extern double *Egrid_buffer;

// checkpoint
extern enum chpoint chp_type;
extern time_t chp_time;
extern char chp_dir[];

// auxiliary grids and their partition over processors
extern size_t gridX,gridY,gridZ;
extern size_t gridYZ;
extern size_t smallY,smallZ;
extern size_t local_Nsmall;
extern int nprocs,ringid;
extern int local_z0,local_z1,local_z1_coer,local_Nz_unif;
extern size_t local_Nz,local_x0,local_x1,local_Nx;
extern size_t local_Ndip,local_nvoid_Ndip,nlocalRows;
extern double nvoid_Ndip;

// timing
extern time_t wt_start,last_chp_wt;
extern TIME_TYPE Timing_EField,Timing_FileIO,Timing_Integration,Timing_OneIterComm,tstart_main;

#endif // __vars_h