#include "f2c.h"

#define HUGE_VAL 1E30
#ifdef __cplusplus
    extern "C" {
#endif
extern int closeunit_(integer *unitnum);
extern int grdfun_(integer *ndv, integer *nobj, integer *ncon, doublereal *x, doublereal *obj, doublereal *g, doublereal *df, doublereal *dg);
extern int ks_(doublereal *f, doublereal *g, integer *ng, doublereal *rho);
extern int ksales_(doublereal *a1, doublereal *a2, doublereal *a3, doublereal *f1, doublereal *f2, doublereal *f3, doublereal *alim, doublereal *alpha);
extern int ksando_(doublereal *obj, doublereal *g, doublereal *fscale, doublereal *offset, integer *ncon, integer *nobj);
extern int kscomg_(doublereal *work);
/* comlen kscomm_ 344 */
extern int kscomp_(doublereal *work);
/* comlen kscomm_ 344 */
extern int kscube_(doublereal *a1, doublereal *a2, doublereal *a3, doublereal *a4, doublereal *f1, doublereal *f2, doublereal *f3, doublereal *f4, doublereal *astar, doublereal *fstar);
extern int ksd_(doublereal *df, doublereal *g, doublereal *dgdx, integer *ng, doublereal *rho);
extern int ksdfp_(doublereal *x, doublereal *side, doublereal *act, doublereal *dfun, integer *ndv, doublereal *s, doublereal *slope, doublereal *y, doublereal *p, doublereal *h__, doublereal *hess, integer *isdflg);
/*:ref: kshmul_ 14 4 7 7 7 4 */
/*:ref: ksvprd_ 14 4 7 7 7 4 */
extern int ksdfun_(doublereal *dfun, doublereal *obj, doublereal *fscale, doublereal *offset, doublereal *df, doublereal *g, doublereal *dg, doublereal *rho, integer *ndv, integer *ncon, integer *nobj, doublereal *temp1, doublereal *temp2, integer *nodim, integer *ncdim);
/*:ref: ksd_ 14 5 7 7 7 4 7 */
extern int ksfun_(doublereal *fun, doublereal *obj, doublereal *g, doublereal *rho, doublereal *fscale, doublereal *offset, integer *ncon, integer *nobj, doublereal *temp);
/*:ref: ks_ 14 4 7 7 4 7 */
extern int ksgrad_(integer *inext, doublereal *x, doublereal *x0, doublereal *xlb, doublereal *xub, doublereal *g, doublereal *g0, doublereal *obj, doublereal *obj0, doublereal *df, doublereal *dg, doublereal *scale, doublereal *delx, integer *ndv, integer *ncon, integer *nobj, integer *nside, doublereal *fdelt, doublereal *fdmin, integer *nodim, integer *ncdim);
extern int kshess_(doublereal *hess, doublereal *obj, doublereal *fscale, doublereal *offset, doublereal *df, doublereal *g, doublereal *dg, doublereal *rho, integer *ndv, integer *ncon, integer *nobj, doublereal *temp1, doublereal *temp2, integer *nodim, integer *ncdim);
extern int kshmul_(doublereal *a, doublereal *b, doublereal *x, integer *nrow);
extern int ksinfo_(integer *iout, doublereal *rout);
/* comlen kscomm_ 344 */
extern int ksinit_(doublereal *x, doublereal *xlb, doublereal *xub, doublereal *scale, doublereal *work, integer *mdv, integer *mcon, integer *mobj, integer *mside, integer *mscale, integer *jprnt, integer *jtmax, integer *jgrad, integer *jsdrst, doublereal *rdfun, doublereal *adfun, doublereal *fdelt, doublereal *fdmin, doublereal *rhomn, doublereal *rhomx, doublereal *rhodl, integer *munit, integer *ireq);
/* comlen kscomm_ 344 */
/*:ref: kscomp_ 14 1 7 */
extern int ksmain_(integer *ndv, integer *nobj, integer *ncon, doublereal *x, doublereal *xlb, doublereal *xub, doublereal *obj, doublereal *g, doublereal *work, integer *nwork, integer *itmax, doublereal *rdfun, doublereal *rhomin, doublereal *rhomax, integer *iout, integer *iprint, char *iname, integer *nfeval, integer *ngeval, S_fp objfun, S_fp grdfun, ftnlen iname_len);
/*:ref: ksinit_ 14 23 7 7 7 7 7 4 4 4 4 4 4 4 4 4 7 7 7 7 7 7 7 4 4 */
/*:ref: ksopt_ 14 9 4 7 7 7 7 7 4 4 7 */
extern int ksoned_(integer *inext, integer *jtry, doublereal *x, doublereal *x0, doublereal *xlb, doublereal *xub, doublereal *fun, doublereal *fun0, doublereal *s, doublereal *slope, doublereal *alpha, doublereal *alpmax, integer *ndv, doublereal *a1, doublereal *a2, doublereal *a3, doublereal *a4, doublereal *f1, doublereal *f2, doublereal *f3, doublereal *f4, doublereal *alim, doublereal *atest, doublereal *ftest, integer *nside, integer *limit, integer *nunit, integer *iprnt3, doublereal *scale, doublereal *temp, integer *isdflg);
/*:ref: ksales_ 14 8 7 7 7 7 7 7 7 7 */
/*:ref: ksquad_ 14 8 7 7 7 7 7 7 7 7 */
/*:ref: kscube_ 14 10 7 7 7 7 7 7 7 7 7 7 */
/*:ref: ksqmin_ 14 10 7 7 7 7 7 7 7 7 7 7 */
/*:ref: ksunsc_ 14 4 7 7 7 4 */
extern int ksopt_(integer *isel, doublereal *x, doublereal *obj, doublereal *g, doublereal *df, doublereal *dg, integer *nomax, integer *ngmax, doublereal *work);
/* comlen kscomm_ 344 */
/*:ref: kscomg_ 14 1 7 */
/*:ref: ksando_ 14 6 7 7 7 7 4 4 */
/*:ref: ksfun_ 14 9 7 7 7 7 7 7 4 4 7 */
/*:ref: ksgrad_ 14 21 4 7 7 7 7 7 7 7 7 7 7 7 7 4 4 4 4 7 7 4 4 */
/*:ref: ksdfun_ 14 15 7 7 7 7 7 7 7 7 4 4 4 7 7 4 4 */
/*:ref: kshess_ 14 15 7 7 7 7 7 7 7 7 4 4 4 7 7 4 4 */
/*:ref: ksside_ 14 7 7 7 7 7 7 4 4 */
/*:ref: ksdfp_ 14 12 7 7 7 7 4 7 7 7 7 7 7 4 */
/*:ref: ksprnt_ 14 14 4 4 4 7 7 7 7 7 7 7 4 4 7 7 */
/*:ref: ksoned_ 14 31 4 4 7 7 7 7 7 7 7 7 7 7 4 7 7 7 7 7 7 7 7 7 7 7 4 4 4 4 7 7 4 */
/*:ref: ksscal_ 14 8 7 7 7 7 7 4 4 4 */
/*:ref: ksxlim_ 14 5 7 7 7 4 4 */
/*:ref: ksunsc_ 14 4 7 7 7 4 */
/*:ref: kscomp_ 14 1 7 */
extern int ksprnt_(integer *ipflag, integer *iprnt1, integer *iprnt2, doublereal *x, doublereal *obj, doublereal *g, doublereal *df, doublereal *dg, doublereal *side, doublereal *scale, integer *nodim, integer *ncdim, doublereal *temp, doublereal *work);
/* comlen kscomm_ 344 */
/*:ref: ksunsc_ 14 4 7 7 7 4 */
extern int ksqmin_(doublereal *a1, doublereal *a2, doublereal *a3, doublereal *a4, doublereal *alpha, doublereal *f1, doublereal *f2, doublereal *f3, doublereal *f4, doublereal *fun);
extern int ksquad_(doublereal *a1, doublereal *a2, doublereal *a3, doublereal *f1, doublereal *f2, doublereal *f3, doublereal *astar, doublereal *fstar);
extern int ksscal_(doublereal *x, doublereal *x0, doublereal *xlb, doublereal *xub, doublereal *scale, integer *ndv, integer *nside, integer *nscale);
extern int ksside_(doublereal *x, doublereal *xlb, doublereal *xub, doublereal *side, doublereal *dfun, integer *ndv, integer *nside);
extern int ksunsc_(doublereal *x, doublereal *sx, doublereal *scale, integer *ndv);
extern int ksvprd_(doublereal *x, doublereal *y, doublereal *prod, integer *nrow);
extern int ksxlim_(doublereal *x, doublereal *xlb, doublereal *xub, integer *ndv, integer *nside);
extern int objfun_(integer *ndv, integer *nobj, integer *ncon, doublereal *x, doublereal *obj, doublereal *g);
//extern int openunit_(integer *unitnum, char *filename, char *filestatus, char *fileaction, integer *ierror, ftnlen filename_len, ftnlen filestatus_len, ftnlen fileaction_len);
//extern int pyflush_(integer *unitnum);
/*:ref: flush_ 14 1 4 */

// iter function definitions
typedef struct {
	// Maximum constraint violation.
	double constrviolation;
	// Measure of degeneracy. A point is degenerate if the partial derivative with respect to one of the variables is 0 at the point. A bound constraint is active for that variable at the point.
	double degenerate;
	// Directional derivative in the search direction.
	//double directionalderivative;
	// First-order optimality (depends on algorithm). Final value equals optimization function output output.firstorderopt.
	double firstorderopt;
	// Cumulative number of function evaluations. Final value equals optimization function output output.funcCount.
	int funccount;
	// Function value at current point. Final value equals optimization function output fval.
	//double fval;
	// Current gradient of objective function — either analytic gradient if you provide it or finite-differencing approximation. Final value equals optimization function output grad.
	//double gradient;
	// Iteration number - starts at 0. Final value equals optimization function output output.iterations.
	int iteration;
	// Actual step length divided by initially predicted step length
	//double lssteplength;
	// Maximum function value
	double maxfval;
	// Search direction.
	//double searchdirection;
	// Current step size (displacement in x). Final value equals optimization function output output.stepsize.
	double stepsize;
} KSOPTIter;

typedef enum {
	KSOPT_ALGORITHM_STATE_INIT,// The algorithm is in the initial state before the first iteration.
	KSOPT_ALGORITHM_STATE_INTERRUPT,// The algorithm is in some computationally expensive part of the iteration. In this state, the output function can interrupt the current iteration of the optimization. At this time, the values of x and optimValues are the same as at the last call to the output function in which state=='iter'.
	KSOPT_ALGORITHM_STATE_ITER,// 	The algorithm is at the end of an iteration.
	KSOPT_ALGORITHM_STATE_DONE// The algorithm is in the final state after the last iteration.
} KSOPT_ALGORITHM_STATE;

typedef int (objective_function(const int, const int, const int, const double*, double*, double*));
typedef int (gradient_function(const int, const int, const int, const double*, double*, double*, double*, double*));
typedef int (iteration_function(const int, const int, const int, const double*, const double*, const double*, const double*, const double*, const KSOPTIter, const KSOPT_ALGORITHM_STATE, unsigned char*));

typedef struct {
	// maximum number of iterations
    int maxiter;
	// maximum number of function evaluations
    int maxfunevals;
	// RHOMIN -- minimum multiplier for ks function (default is 5.0)
    double rhomin;
	// RHOMAX -- maximum multiplier for ks function (default is 100.0)
    double rhomax;
	// RHODEL -- increment for rho (default is computed internally)
	double rhodel;
	int print;
	// NSCALE-- flag selecting design variable scaling
	//		=0 -- no scaling 
	//		<0 -- user supplied scaling in vector scale
	//		>0 -- automatic scaling by ksopt every NSCALE iterations
	int scale;
	// scaling factor to use when scale == 0
	double* scalefactor;
	// size of the scalefactor option
	int scalefactorsize;
	// indicator, if gradient information is supplied
	unsigned short int gradient;
	// RDFUN -- relative function change for termination (default is 0.0001)
	double tolfunrel;
	// ADFUN -- absolute function change for termination (default is 0.0, which means that this setting will not be used)
	double tolfunabs;
	// FDELT -- step size for computing finite differences derivatives (default is 0.01)
	double findiffstepsize;
	// FDMIN -- minimum difference for computing finite differences (default is 0.0001)
	double findiffstepsizemin;
	// maximum execution time
	double maxtime;
	// limit for objective function
	double objectivelimit;
	unsigned char hasobjectivelimit;
	// funtion to call during every iteration
	iteration_function *iterfun;
} KSOPToptionType; /* option structure */

typedef struct {
	// number of optimization variables
	int ndv;
	// optimal optimization variables
	double* xopt;
	// number of objective functions
	int nobj;
	// optimal objective functions
	double* fopt;
	// number of constraint functions
	int ncon;
	// optimal constraint functions
	double* conopt;
	// time needed for solving
	double time;
	// number of function evaluations
	int fevals;
	// number of iterations
	int iter;
	// stepsize of last step
	double stepsize;
	// optimality tolerance at optimal point
	double tolopt;
	// constraint tolerance at optimal point
	double tolcon;
	// exitflag
	double exitflag;
} KSOPTResult;

extern int ksmain(const integer ndv, const integer nobj, const integer ncon, doublereal* x, const doublereal* xlb, const doublereal* xub, const KSOPToptionType options, integer* nfeval, integer* ngeval, objective_function objfun, gradient_function gradfun, KSOPTResult* result);

extern int ksgeterrormessage(const int id, char* errormessage, const int buffersize);
#define KSOPT_ERROR_MESSAGE_MAXSIZE 98
extern const int KSOPT_ErrorID[];
extern const char *KSOPT_Error_Message[];

#if !defined(KSOPT_OPTIONTYPE_DEFAULT)
	#define KSOPT_OPTIONTYPE_DEFAULT \
		KSOPToptionType defaultOptions;\
		defaultOptions.findiffstepsize = 0.01;\
		defaultOptions.findiffstepsizemin = 0.0001;\
		defaultOptions.gradient = false;\
		defaultOptions.hasobjectivelimit = false;\
		defaultOptions.iterfun = NULL;\
		defaultOptions.maxfunevals = 1500;\
		defaultOptions.maxiter = 100;\
		defaultOptions.maxtime = 0.0;\
		defaultOptions.objectivelimit = -HUGE_VAL;\
		defaultOptions.print = 0;\
		defaultOptions.rhodel = 0.0;\
		defaultOptions.rhomax = 100.0;\
		defaultOptions.rhomin = 5.0;\
		defaultOptions.scale = 1;/* 1 means automatic scaling*/\
		defaultOptions.scalefactor = NULL;\
		defaultOptions.scalefactorsize = 0;\
		defaultOptions.tolfunabs = 0.0;/* 0.0 means not used*/\
		defaultOptions.tolfunrel = 0.0001;\

#endif
#ifdef __cplusplus
    }
#endif
