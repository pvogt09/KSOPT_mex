
#ifdef MATLAB_MEX_FILE
#include "mex.h"
#define free mxFree
#define calloc mxCalloc
#define malloc mxMalloc
#define realloc mxRealloc
#else
#include <stdlib.h>
#include <stdio.h>
#endif
#include <time.h>
#include "ksopt.h"

/* Type definitions */
// objective function called by KSOPT
static int OBJFUN(const integer* ndv, const integer* nobj, const integer* ncon, const doublereal* x, doublereal* obj, doublereal* g);
// gradient function called by KSOPT
static int GRDFUN(const integer* ndv, const integer* nobj, const integer* ncon, const doublereal* x, doublereal* obj, doublereal* g, doublereal* df, doublereal* dg);
// gradient function called by KSOPT
static int ITERFUN(const integer* ndv, const integer* nobj, const integer* ncon, const doublereal* x, const doublereal* obj, const doublereal* g, const doublereal* df, const doublereal* dg, const KSOPTIter iter, const KSOPT_ALGORITHM_STATE state, unsigned char* stop);

// function for calculation of degeneracy measure
static integer iter_fun_arguments(const integer ndv, const integer nobj, const integer ncon, const doublereal* x, const doublereal* lb, const doublereal* ub, const doublereal* obj, const doublereal* g, const doublereal* df, const doublereal* dg, const integer funccount, const integer itercount, const doublereal stepsize, KSOPTIter* iter_argument);
// option checking function
static int checkOptions(const KSOPToptionType options, const int ndv);
#define KSTERMINATE()\
	if (WORK != NULL) {\
		free(WORK);\
		WORK = NULL;\
	}\
	if (XOPT != NULL) {\
		free(XOPT);\
		XOPT = NULL;\
	}\
	if (SCALE != NULL) {\
		free(SCALE);\
		SCALE = NULL;\
	}\
	if (OBJ != NULL) {\
		free(OBJ);\
		OBJ = NULL;\
	}\
	if (G != NULL) {\
		free(G);\
		G = NULL;\
	}\
	if (DF != NULL) {\
		free(DF);\
		DF = NULL;\
	}\
	if (DG != NULL) {\
		free(DG);\
		DG = NULL;\
	}\
	if (LB != NULL) {\
		free(LB);\
		LB = NULL;\
	}\
	if (UB != NULL) {\
		free(UB);\
		UB = NULL;\
	}

// global variables for function pointers to evaluate
static objective_function *OBJ_FUN = ((objective_function*)NULL);
static gradient_function *GRAD_FUN = ((gradient_function*)NULL);
static iteration_function *ITER_FUN = ((iteration_function*)NULL);
static unsigned char HAS_ITER_FUN = false;
int ksmain(const integer ndv, const integer nobj, const integer ncon, doublereal* x, const doublereal* xlb, const doublereal* xub, const KSOPToptionType options, integer* nfeval, integer* ngeval, objective_function objfun, gradient_function gradfun, KSOPTResult* result) {
	/*KSMAIN main function for optimization with KSOPT
	*	Input:
	*		ndv:							number of optimization variables
	*		nobj:							number of objectives
	*		ncon:							number of constraints
	*		x:								initial optimization value
	*		xlb:							vector of lower bounds for optimization variables
	*		xub:							vector of upper bounds for optimization variables
	*		options:						structure with options for optimization
	*	Output:
	*		nfeval:							number of function evaluations
	*		ngeval:							number of gradient evaluations
	*		objfun:							function pointer to objective and constraint function
	*		gradfun:						function pointer to gradient objective and constraint function
	*		result:							structure with information about optimization run
	*		exitstatus:						zero in case of success else nonzero
	*/
	// variable definitions for internal pointers
	integer *NDV = NULL, *NOBJ = NULL, *NCON = NULL, *NSIDE = NULL, *NSCALE = NULL, *ITMAX = NULL, *IGRAD = NULL, *ISDRST = NULL, *IPRNT = NULL;
	doublereal *RDFUN = NULL, *ADFUN = NULL, *FDELT = NULL, *FDMIN = NULL, *RHOMIN = NULL, *RHOMAX = NULL, *RHODEL = NULL;
	// input variables and options to be converted to pointers
	// A non-zero value indicates that side constraints are to be considered during optimization.
	integer nside = 0;
	integer nscale = 0;
	integer itmax = 100;
	integer igrad = 0;
	integer isdrst = ndv + 1;
	doublereal rdfun = 0.01;
	doublereal adfun = 0.0;
	doublereal fdelt = 0.01;
	doublereal fdmin = 0.0001;
	doublereal rhomin = 5.0;
	doublereal rhomax = 100.0;
	doublereal rhodel = 0.0;// 0.0 means calculate value interally
	integer ireq = 0;
	integer infoiout[15];
	doublereal inforout[25];

	// variables for separation of inputs and working variables
	integer number_variables = ndv;
	integer number_objectives = nobj;
	integer number_constraints = ncon;
	doublereal *LB = NULL;
	doublereal *UB = NULL;
	doublereal *XOPT = NULL;

	//unsigned short int displayprogress = options.print > 0;
	integer iprnt = 0;
	integer iout = 0;

	// working matrices
	doublereal* SCALE = NULL;
	doublereal* WORK = NULL;
	doublereal* OBJ = NULL;
	doublereal* G = NULL;
	doublereal* DF = NULL;
	doublereal* DG = NULL;
	integer NWORK = 0;

	// output variables
	integer* IOUT = NULL;
	integer* IREQ = NULL;
	integer* NFEVAL = nfeval;
	integer* NGEVAL = ngeval;

	// local variables to be defined
	integer ISEL = 0;
	integer *NOMAX = NULL, *NGMAX = NULL;
	double tolcon = 0.0, tolopt = 0.0, normDF = 0.0;
	int retval = 0;
	int ii = 0, jj = 0;
	clock_t starttime = 0;
	clock_t endtime = 0;
	KSOPTIter iter_fun_argument;
	unsigned char stop_optimization = false;
	int lastiter = 0;

	// argument checking
	if (ndv <= 0) {
		return 100;
	}
	if (nobj <= 0) {
		return 101;
	}
	if (objfun == NULL) {
		return 102;
	}
	else {
		OBJ_FUN = objfun;
	}
	if (OBJ_FUN == NULL) {
		return 102;
	}
	if (gradfun == NULL) {
		return 103;
	}
	else {
		GRAD_FUN = gradfun;
	}
	if (GRAD_FUN == NULL) {
		return 102;
	}
	if (result == NULL) {
		return 400;
	}
	if (result->ndv != ndv) {
		return 401;
	}
	if (result->nobj != nobj) {
		return 402;
	}
	if (result->ncon != ncon) {
		return 403;
	}
	if (result->xopt == NULL) {
		return 404;
	}
	if (result->fopt == NULL) {
		return 405;
	}
	if (ncon > 0) {
		if (result->conopt == NULL) {
			return 406;
		}
	}

	retval = checkOptions(options, ndv);
	if (retval != 0) {
		return retval;
	}

	nscale = ((integer)options.scale);
	itmax = ((integer)options.maxiter);
	igrad = ((integer)options.gradient);
	isdrst = ((integer)options.gradient);
	rdfun = ((doublereal)options.tolfunrel);
	adfun = ((doublereal)options.tolfunabs);
	fdelt = ((doublereal)options.findiffstepsize);
	fdmin = ((doublereal)options.findiffstepsizemin);
	rhomin = ((doublereal)options.rhomin);
	rhomax = ((doublereal)options.rhomax);
	rhodel = ((doublereal)options.rhodel);
	if (options.iterfun != NULL) {
		ITER_FUN = options.iterfun;
		HAS_ITER_FUN = true;
	}
	else {
		ITER_FUN = ((iteration_function*)NULL);
		HAS_ITER_FUN = false;
	}

	// estimate for the size of WORK array
	NWORK = 58 + 3*nobj + ncon + 11*ndv + ndv*(ndv - 1)/2 + nobj*ndv + ncon*ndv + 2*max(2*ndv, nobj + ncon);
	// TODO: possibly add some more space to mitigate memory corruction errors in internal ksopt functions
	NWORK = 2*NWORK;

	// internal pointers
	NDV = &number_variables;
	NOBJ = &number_objectives;
	NCON = &number_constraints;
	NSCALE = &nscale;
	ITMAX = &itmax;
	IGRAD = &igrad;
	ISDRST = &isdrst;
	RDFUN = &rdfun;
	ADFUN = &adfun;
	FDELT = &fdelt;
	FDMIN = &fdmin;
	RHOMIN = &rhomin;
	RHOMAX = &rhomax;
	RHODEL = &rhodel;
	for (ii = 0; ii < 15; ++ii) {
		infoiout[ii] = 0;
	}
	for (ii = 0; ii < 25; ++ii) {
		inforout[ii] = 0;
	}

	// output variables
	IOUT = &iout;
	IREQ = &ireq;

	XOPT = (doublereal*) calloc(*NDV, sizeof(doublereal));
	if (XOPT == NULL) {
		KSTERMINATE();
		return 90;
	}
	LB = (doublereal*) calloc(*NDV, sizeof(doublereal));
	if (LB == NULL) {
		KSTERMINATE();
		return 90;
	}
	UB = (doublereal*) calloc(*NDV, sizeof(doublereal));
	if (UB == NULL) {
		KSTERMINATE();
		return 90;
	}
	for (ii = 0; ii < ndv; ++ii) {
		LB[ii] = xlb[ii];
		UB[ii] = xub[ii];
		XOPT[ii] = x[ii];
		if (xlb[ii] > -HUGE_VAL || xub[ii] < HUGE_VAL) {
			// take bound constraints into account
			nside = 1;
		}
	}
	NSIDE = &nside;

	// initialization of working variables
	WORK = (doublereal*) calloc(NWORK, sizeof(doublereal));
	if (WORK == NULL) {
		KSTERMINATE();
		return 90;
	}
	SCALE = (doublereal*) calloc(*NDV, sizeof(doublereal));
	if (SCALE == NULL) {
		KSTERMINATE();
		return 90;
	}
	OBJ = (doublereal*) calloc(*NOBJ, sizeof(doublereal));
	if (OBJ == NULL) {
		KSTERMINATE();
		return 90;
	}
	G = (doublereal*) calloc(*NCON, sizeof(doublereal));
	if (*NCON > 0 && G == NULL) {
		KSTERMINATE();
		return 90;
	}
	DF = (doublereal*) calloc((*NOBJ)*(*NDV), sizeof(doublereal));
	if (DF == NULL) {
		KSTERMINATE();
		return 90;
	}
	DG = (doublereal*) calloc((*NCON)*(*NDV), sizeof(doublereal));
	if (*NCON > 0 && DG == NULL) {
		KSTERMINATE();
		return 90;
	}

	// use user supplied scaling vector
	if (nscale < 0) {
		for (ii = 0; ii < ndv; ++ii) {// scalefctor has ndv elements because otherwise option checking would have crashed
			SCALE[ii] = (doublereal)options.scalefactor[ii];
		}
	}


	if (options.print == 0) iprnt = 000;
	if (options.print == 1) iprnt = 001;
	if (options.print == 2) iprnt = 012;
	if (options.print == 3) iprnt = 013;
	IPRNT = &iprnt;
	starttime = clock();
	retval = ksinit_(XOPT, LB, UB, SCALE, WORK, NDV, NCON, NOBJ, NSIDE, NSCALE, IPRNT, ITMAX, IGRAD, ISDRST, RDFUN, ADFUN, FDELT, FDMIN, RHOMIN, RHOMAX, RHODEL, IOUT, IREQ);
	if (retval != 0) {
		KSTERMINATE();
		return 200;
	}
	// resize WORK according to the needs of KSOPT if it was too small
	if (NWORK <= *IREQ) {
		// TODO: possibly add some more space to mitigate memory corruction errors in internal ksopt functions
		WORK = (doublereal*) realloc(WORK, (max(NWORK, *IREQ) + max(10*ndv, 0))*sizeof(doublereal));
		if (WORK == NULL) {
			KSTERMINATE();
			return 201;
		}
	}
	NOMAX = NOBJ;
	NGMAX = NCON;

	if (HAS_ITER_FUN) {
		retval = ksinfo_(infoiout, inforout);
		if (retval != 0) {
			KSTERMINATE();
			return 300;
		}
		retval = iter_fun_arguments(*NDV, *NOBJ, *NCON, XOPT, LB, UB, OBJ, G, DF, DG, ((const int)*NFEVAL), ((const int)infoiout[3]), (const double)inforout[5], &iter_fun_argument);
		if (retval != 0) {
			KSTERMINATE();
			return retval;
		}
		retval = ITERFUN(NDV, NOBJ, NCON, XOPT, OBJ, G, DF, DG, iter_fun_argument, KSOPT_ALGORITHM_STATE_INIT, &stop_optimization);
		if (retval != 0) {
			KSTERMINATE();
			return retval;
		}
	}

	result->exitflag = 0;
	if (stop_optimization) {
		ISEL = 0;
	}
	else {
		ISEL = -1;
	}
	lastiter = (int)infoiout[3];
	while (ISEL != 0) {
		//LABEL10:;
		retval = ksopt_(&ISEL, XOPT, OBJ, G, DF, DG, NOMAX, NGMAX, WORK);
		if (retval != 0) {
			result->exitflag = retval;
			break;
		}
		endtime = clock();
		if (options.maxtime > 0.0 && endtime - starttime > 0 && options.maxtime <= ((double)(endtime - starttime))/((double) CLOCKS_PER_SEC)) {
			result->exitflag = 6;
			retval = 0;
			break;
		}
		if ((int)*NFEVAL >= options.maxfunevals) {
			result->exitflag = 5;
			retval = 0;
			break;
		}
		if (HAS_ITER_FUN) {
			retval = ksinfo_(infoiout, inforout);
			if (retval != 0) {
				KSTERMINATE();
				return 300;
			}
			if (lastiter != (int)infoiout[3]) {
				retval = iter_fun_arguments(*NDV, *NOBJ, *NCON, XOPT, LB, UB, OBJ, G, DF, DG, ((const int)*NFEVAL), ((const int)infoiout[3]), (const double)inforout[5], &iter_fun_argument);
				if (retval != 0) {
					KSTERMINATE();
					return retval;
				}
				retval = ITERFUN(NDV, NOBJ, NCON, XOPT, OBJ, G, DF, DG, iter_fun_argument, KSOPT_ALGORITHM_STATE_ITER, &stop_optimization);
				if (retval != 0) {
					KSTERMINATE();
					return retval;
				}
				lastiter = (int)infoiout[3];
			}
		}
		if (ISEL == 1) {
			//LABEL20:;
			retval = OBJFUN(NDV, NOBJ, NCON, XOPT, OBJ, G);
			if (retval != 0) {
				result->exitflag = retval;
				break;
			}
			if (options.hasobjectivelimit) {
				for (ii = 0; ii < nobj; ++ii) {
					if (OBJ[ii] < options.objectivelimit) {
						result->exitflag = 7;
					}
				}
				if (result->exitflag == 7) {
					break;
				}
			}
			*NFEVAL = (*NFEVAL) + 1;
			continue;
		}
		if (ISEL == 2) {
			//LABEL30:;
			retval = GRDFUN(NDV, NOBJ, NCON, XOPT, OBJ, G, DF, DG);
			if (retval != 0) {
				result->exitflag = retval;
				break;
			}
			*NGEVAL = (*NGEVAL) + 1;
			continue;
		}
		if (ISEL == 0) {
			//LABEL50:;
			retval = 0;
			if (*IPRNT != 0) {
				printf("NUMBER OF FUNC-CALLS: %d\n", *NFEVAL);
				printf("NUMBER OF GRAD-CALLS: %d\n", *NGEVAL);
			}
			break;
		}
	}
	retval = ksinfo_(infoiout, inforout);
	if (retval != 0) {
		KSTERMINATE();
		return 300;
	}
	result->fevals = (int)*NFEVAL;
	result->iter = (int)infoiout[3];
	result->stepsize = (double)inforout[5];
	tolopt = HUGE_VAL;
	for (ii = 0; ii < ndv; ++ii) {
		x[ii] = XOPT[ii];
		result->xopt[ii] = (double)x[ii];
		normDF = 0.0;
		for (jj = 0; jj < nobj; ++jj) {
			normDF += abs(DF[ii*nobj + jj]*DF[ii*nobj + jj]);
		}
		if (normDF < tolopt) {
			tolopt = normDF;
		}
	}
	for (ii = 0; ii < nobj; ++ii) {
		result->fopt[ii] = (double)OBJ[ii];
	}
	tolcon = 0.0;
	for (ii = 0; ii < ncon; ++ii) {
		result->conopt[ii] = G[ii];
		if (G[ii] > tolcon) {
			tolcon = (double)G[ii];
		}
	}
	result->tolcon = max(0.0, tolcon);
	result->tolopt = tolopt;
	result->time = ((double)(clock()-starttime))/CLOCKS_PER_SEC;
	if (result->exitflag == 0) {
		if (infoiout[3] >= options.maxiter) {
			// maximum number of iterations exceeded
			result->exitflag = 3;
		}
		else {
			if (infoiout[4] >= 3 || infoiout[5] >= 3) {
				if (ncon <= 0 || (ncon > 0 && tolcon <= 0.0)) {
					// optimal solution that satisfies the constraints
					result->exitflag = 0;
				}
				else {
					// solution, that does not satisfy the constraints, but no further step to minimum is possible
					result->exitflag = 4;
				}
			}
			else {
				// terminated because of unknown reason
				result->exitflag = -2;
			}
		}
	}

	if (HAS_ITER_FUN) {
		retval = iter_fun_arguments(*NDV, *NOBJ, *NCON, XOPT, LB, UB, OBJ, G, DF, DG, ((const int)*NFEVAL), ((const int)infoiout[3]), (const double)inforout[5], &iter_fun_argument);
		if (retval != 0) {
			KSTERMINATE();
			return retval;
		}
		retval = ITERFUN(NDV, NOBJ, NCON, XOPT, OBJ, G, DF, DG, iter_fun_argument, KSOPT_ALGORITHM_STATE_DONE, &stop_optimization);
		if (retval != 0) {
			KSTERMINATE();
			return retval;
		}
		if (stop_optimization) {
			result->exitflag = -7;
		}
	}

	//LABEL60:;
	KSTERMINATE();
	return 0;
}

static int OBJFUN(const integer* ndv, const integer* nobj, const integer* ncon, const doublereal* x, doublereal* obj, doublereal* g) {
	int retval = 0;
	if (OBJ_FUN == NULL) {
		return 1;
	}
	retval = OBJ_FUN((const int)*ndv, (const int)*nobj, (const int)*ncon, (const double*)x, (double*)obj, (double*)g);
	return retval;
}

static int GRDFUN(const integer* ndv, const integer* nobj, const integer* ncon, const doublereal* x, doublereal* obj, doublereal* g, doublereal* df, doublereal* dg) {
	int retval = 0;
	if (GRAD_FUN == NULL) {
		return 1;
	}
	retval = GRAD_FUN((const int)*ndv, (const int)*nobj, (const int)*ncon, (const double*)x, (double*)obj, (double*)g, (double*)df, (double*)dg);
	return retval;
}

static int ITERFUN(const integer* ndv, const integer* nobj, const integer* ncon, const doublereal* x, const doublereal* obj, const doublereal* g, const doublereal* df, const doublereal* dg, const KSOPTIter iter, const KSOPT_ALGORITHM_STATE state, unsigned char* stop) {
	int retval = 0;
	if (HAS_ITER_FUN) {
		if (ITER_FUN == NULL) {
			return 1;
		}
		retval = ITER_FUN((const int)*ndv, (const int)*nobj, (const int)*ncon, (const double*)x, (const double*)obj, (const double*)g, (const double*)df, (const double*)dg, iter, state, stop);
		return retval;
	}
	else {
		return 0;
	}
}
/*
C
C  OPTIMIZE
C
	ISEL=-1
10    CONTINUE
	CALL KSOPT(ISEL,X,OBJ,G,DF,DG,NOMAX,NGMAX,WORK)
	IF (ISEL.EQ.0) GO TO 50
	IF (ISEL.EQ.1) GO TO 20
	IF (ISEL.EQ.2) GO TO 30
C
C  EVALUATE OBJECTIVE AND CONSTRAINTS
C
20    CONTINUE
	CALL OBJFUN(NDV,NOBJ,NCON,X,OBJ,G)
	NFEVAL=NFEVAL+1
	GO TO 10
C
C  EVALUATE GRADIENTS
C
30    CONTINUE
	CALL GRDFUN(NDV,NOBJ,NCON,X,OBJ,G,DF,DG)
	NGEVAL=NGEVAL+1
	GO TO 10
C
50    CONTINUE
C
C  PRINT FINAL RESULTS
C
	IF (IPRINT.EQ.0) GO TO 60
	WRITE(IOUT,1650) NFEVAL
	WRITE(IOUT,1750) NGEVAL
60    CONTINUE
C
	RETURN
C  ------------------------------------------------------------------
C  FORMATS
C  ------------------------------------------------------------------
1650  FORMAT(//8X,'NUMBER OF FUNC-CALLS:  NFUN =',I5)
1750  FORMAT(/8X,'NUMBER OF GRAD-CALLS:  NGRD =',I5)
C
	END
	*/

static int checkOptions(const KSOPToptionType options, const int ndv) {
	int ii = 0, print = 0;
	/*// maximum number of iterations
	int_T maxiter;
	// RHOMIN -- minimum multiplier for ks function (default is 5.0)
	real_T rhomin;
	// RHOMAX -- maximum multiplier for ks function (default is 100.0)
	real_T rhomax;
	// RHODEL -- increment for rho (default is computed internally)
	real_T rhodel;
	int_T print;
	// NSCALE-- flag selecting design variable scaling
	//		=0 -- no scaling
	//		<0 -- user supplied scaling in vector scale
	//		>0 -- automatic scaling by ksopt every NSCALE iterations
	int_T scale;
	// indicator, if gradient information is supplied
	boolean_T gradient;
	// RDFUN -- relative function change for termination (default is 0.01)
	real_T tolfunrel;
	// ADFUN -- absolute function change for termination (default is 0.0)
	real_T tolfunabs;
	// FDELT -- step size for computing finite differences derivatives (default is 0.01)
	real_T findiffstepsize;
	// FDMIN -- minimum difference for computing finite differences (default is 0.0001)
	real_T findiffstepsizemin;*/

	/*if (options == NULL) {
		mexErrMsgIdAndTxt("KSOPT:input", "Options must not be empty.");
		return 1;
	}*/
	if (options.maxfunevals <= 0) {
		return 1100;
	}
	if (options.maxiter <= 0) {
		return 1200;
	}
	/*	hundreds digit - one-dimensional search print */
	/*                                         0 = no print */
	/*                                         1 = alpha and k-s function */
	/*                                         2 = proposed d.v. vector */

	/*                        tens     digit - gradient print */
	/*                                         0 = no print */
	/*                                         1 = df and dg */

	/*                        ones     digit - d.v. and constraint print */
	/*                                         0 = no print */
	/*                                         1 = initial and final iterations */
	/*                                         2 = all iterations */
	/*                                         3 = slope and search direction */
	/*                                         4 = hessian matrix */
	if (options.print < 0) {
		return 1201;
	}
	print = options.print;
	ii = print % 10;
	if (ii != 0 && ii != 1 && ii != 2 && ii != 3 && ii != 4) {
		return 1202;
	}
	print = print/10;
	ii = print % 10;
	if (ii != 0 && ii != 1) {
		return 1203;
	}
	print = print/10;
	ii = print % 10;
	if (ii != 0 && ii != 1 && ii != 2) {
		return 1204;
	}
	print = print/10;
	if (print != 0) {
		return 1205;
	}
	if (options.gradient != 0 && options.gradient != 1) {
		return 1300;
	}
	if (options.tolfunabs < 0.0) {
		return 1400;
	}
	if (options.tolfunrel <= 0.0) {
		return 1500;
	}
	if (options.findiffstepsize <= 0.0) {
		return 1600;
	}
	if (options.findiffstepsizemin <= 0.0) {
		return 1700;
	}
	if (options.scale < 0.0) {
		if (options.scalefactorsize != ndv) {
			return 1800;
		}
		if (options.scalefactor == NULL) {
			return 1801;
		}
	}
	if (options.maxtime < 0.0) {
		return 1900;
	}
	if (options.rhomin <= 0.0) {
		return 2000;
	}
	if (options.rhomax <= 0.0) {
		return 2100;
	}
	if (options.rhodel < 0.0) {
		return 2200;
	}
	return 0;
}

static integer iter_fun_arguments(const integer ndv, const integer nobj, const integer ncon, const doublereal* x, const doublereal* lb, const doublereal* ub, const doublereal* obj, const doublereal* g, const doublereal* df, const doublereal* dg, const integer funccount, const integer itercount, const doublereal stepsize, KSOPTIter* iter_argument) {
	doublereal *degeneracy_measure_vector = NULL;
	double tolcon = 0.0, tolopt = 0.0, normDF = 0.0, maxfval = 0.0, meas = 0.0, absgrad = 0.0;
	int ii = 0, jj = 0;
	degeneracy_measure_vector = (doublereal*) calloc(ndv, sizeof(doublereal));
	if (degeneracy_measure_vector == NULL) {
		return 90;
	}
	maxfval = -HUGE_VAL;
	for (ii = 0; ii < nobj; ++ii) {
		if (obj[ii] > maxfval) {
			maxfval = (double)obj[ii];
		}
	}
	iter_argument->maxfval = maxfval;
	/*
	Degeneracy
	The value of the field degenerate, which measures the degeneracy of the current optimization point x, is defined as follows.
	First, define a vector r, of the same size as x, for which r(i) is the minimum distance from x(i) to the ith entries of the lower and upper bounds, lb and ub.
	That is,
		r = min(abs(ub-x, x-lb))
	Then the value of degenerate is the minimum entry of the vector r + abs(grad), where grad is the gradient of the objective function.
	The value of degenerate is 0 if there is an index i for which both of the following are true:
		grad(i) = 0
		x(i) equals the ith entry of either the lower or upper bound.
	*/
	for (ii = 0; ii < ndv; ++ii) {
		absgrad = 0.0;
		for (jj = 0; jj < ncon; ++jj) {
			absgrad += abs(dg[ii*ncon + jj]);
		}
		degeneracy_measure_vector[ii] = min(abs(ub[ii] - x[ii]), abs(x[ii] - lb[ii])) + absgrad;
	}
	meas = HUGE_VAL;
	tolopt = HUGE_VAL;
	for (ii = 0; ii < ndv; ++ii) {
		if (degeneracy_measure_vector[ii] < meas) {
			meas = degeneracy_measure_vector[ii];
		}
		normDF = 0.0;
		for (jj = 0; jj < nobj; ++jj) {
			normDF += abs(df[ii*nobj + jj]*df[ii*nobj + jj]);
		}
		if (normDF < tolopt) {
			tolopt = normDF;
		}
	}
	iter_argument->firstorderopt = tolopt;
	iter_argument->degenerate = meas;
	tolcon = 0.0;
	for (ii = 0; ii < ncon; ++ii) {
		if (g[ii] > tolcon) {
			tolcon = (double)g[ii];
		}
	}
	iter_argument->constrviolation = max(0.0, tolcon);
	iter_argument->funccount = (int) funccount;
	iter_argument->iteration = (int) itercount;
	iter_argument->stepsize = (double) stepsize;
	if (degeneracy_measure_vector != NULL) {
		free(degeneracy_measure_vector);
		degeneracy_measure_vector = NULL;
	}
	return 0;
}