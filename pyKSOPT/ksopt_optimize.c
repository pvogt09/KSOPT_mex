/* KSOPT_OPTIMIZE - A MATLAB MEX Interface for KSOPT
 * Institute for Automatic Control
 * Control and Mechatronics Lab
 * Technical University Darmstadt
 * Patrick Vogt
 *
 * Based on:
 * NLOPTMEX - A MATLAB MEX Interface to NLOPT
 * Released Under the BSD 3-Clause License:
 * https://www.inverseproblem.co.nz/OPTI/index.php/DL/License
 *
 * Copyright (C) Jonathan Currie 2013
 * www.inverseproblem.co.nz
 */

/* Based in parts on nlopt_optimize-mex.c supplied with NLOPT */
#define DEBUG_FLAG 0

#include <mex.h>
//#include <math.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef __cplusplus
#include <exception>
#endif
//#pragma comment(lib, "C:\\Program Files\\MATLAB\\R2015B\\extern\\lib\\win64\\microsoft\\libut.lib")
#if defined DEBUG_FLAG
	#include <crtdbg.h>
#endif

#include "ksopt.h"
#include "ksopt_mex.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define PACKAGE_VERSION "2.4"
#define FLEN 256 /* max length of user function name */
#define MAXRHS 2 /* max nrhs for user function */
//Iteration callback structure
/*typedef struct {
	char f[FLEN];
	mxArray *plhs[1];
	mxArray *prhs[3];
	bool enabled;
} iter_fun_data;*/

//User Structures
/*typedef struct {
	char f[FLEN], g[FLEN];
	mxArray *plhs[1];
	mxArray *prhs[MAXRHS];
	mxArray *prhs_g[MAXRHS];
	int xrhs, nrhs, xrhs_g, nrhs_g;
	int nfeval, ngeval, printLevel;
	clock_t start;
	iter_fun_data *iterF;
	KSOPT_functionHandleInformation *info;
} obj_fun_data;*/

typedef struct {
	char f[FLEN];// name of function to call with mexCallMatlab, can be "feval" or the function name as char array
	mxArray *plhs[2];// pointer to output arguments of objective function
	mxArray *prhs[MAXRHS];// pointer to input arguments of objective function
	int xrhs;// position of optimization variable in prhs
	int nrhs;// number of arguments of objective function
	KSOPT_functionHandleInformation *info;// structure with information about objective function
} KSOPT_objective_function;

typedef struct {
	char f[FLEN];// name of function to call with mexCallMatlab, can be "feval" or the function name as char array
	mxArray *plhs[4];// pointer to output arguments of constraint function
	mxArray *prhs[MAXRHS];// pointer to input arguments of constraint function
	int xrhs;// position of optimization variable in prhs
	int nrhs;// number of arguments of constraint function
	KSOPT_functionHandleInformation *info;// structure with information about constraint function
	size_t number_inequality_linear;// number of linear inequality constraints
	size_t number_equality_linear;// number of linear equality constraints
} KSOPT_constraint_function;

typedef struct {
	// indcator if iteration function is present
	boolean_T hasiterfun;
	// pointer to cell array of iteration functions
	mxArray *iter_functions;
	// pointer to current optimization value
	mxArray *x;
	// pointer to structure with optimization information
	mxArray *optimaloptimValues;
	// pointer to algorithm state name
	mxArray *state;
} KSOPT_iter_function;

//Function Prototypes
static void printSolverInfo(void);
/*CHECKINPUTS check input arguments supplied in matlab
 *	Input:
 *		prhs:							array with mxArrays supplied by user in matlab
 *		nrhs:							number of supplied arguments in matlab
 *	Output:
 *		options:						structure with options for optimization with KSOPT
 *		objectiveinformation:			structure with inforamtion about objective function
 *		constraintinformation:			structure with information about constraint function
 *		number_objectives:				number of objective functions
 *		number_constraints_ineq:		number of inequality constraints
 *		number_constraints_eq:			number of equality constraints
 *		number_constraints_linear_ineq:	number of linear inequality constraints
 *		number_constraints_linear_eq:	number of linear equality constraints
 *		iterinfoplot:					structure with information about iteration function
 *		iterinfooutputoptions:			structure with information about output function
 */
static void checkInputs(const mxArray *prhs[], const int nrhs, KSOPToptionType *options, KSOPT_functionHandleInformation *objectiveinformation, KSOPT_functionHandleInformation *constraintinformation, size_t *number_objectives, size_t *number_constraints_ineq, size_t *number_constraints_eq, size_t *number_constraints_linear_ineq, size_t *number_constraints_linear_eq, KSOPT_iterfunctionHandleInformation *iterinfoplot, KSOPT_iterfunctionHandleInformation *iterinfooutput);
/*CHECKIDENTX check if two vectors are the same to handle case when objective and constraints are called with the same input argument for gradient information
 *	Input:
 *		x:	first vector to check
 *		X:	second vector to check
 *		n:	number of elements
 *	Output:
 *		true if vectors are the same else false
 */
static bool checkIdentX(const double *x, const double *X, const int n);
/*CLEARVARIABLES free memory allocated in structures with information about problem structure
 *	Input:
 *		options:						structure with options
 *		iterinformationplot:			structure with information about plotfunction
 *		iterinformationoutput:			structure with information about outputfunction
 *		number_objectives:				number of objectives
 *		number_constraints_ineq:		number of inequality constraints
 *		number_constraints_eq:			number of equality constraints
 *		number_constraints_linear_ineq:	number of linear inequality constraints
 *		number_constraints_linear_eq:	number of linear equality constraints
 */
static void clearVariables(KSOPToptionType *options, KSOPT_iterfunctionHandleInformation *iterinformationplot, KSOPT_iterfunctionHandleInformation *iterinformationoutput, const size_t number_objectives, const size_t number_constraints_ineq, const size_t number_constraints_eq, const size_t number_constraints_linear_ineq, const size_t number_constraints_linear_eq);
/*GRADIENT_FUNCTION_WRAPPER wrapper for call to gradient function from within KSOPT
 *	Input:
 *		ndv:							number of optimization variables
 *		nobj:							number of objectives
 *		ncon:							number of constraints
 *		x:								current optimization value
 *	Output:
 *		obj:							current objective function value
 *		g:								current constraint function value
 *		exitstatus:						zero in case of success else nonzero
 */
static int objective_function_wrapper(const int ndv, const int nobj, const int ncon, const double* x, double* obj, double* g);
/*GRADIENT_FUNCTION_WRAPPER wrapper for call to gradient function from within KSOPT
 *	Input:
 *		ndv:							number of optimization variables
 *		nobj:							number of objectives
 *		ncon:							number of constraints
 *		x:								current optimization value
 *	Output:
 *		obj:							current objective function value
 *		g:								current constraint function value
 *		df:								gradient of current objective function value
 *		dg:								gradient of current constraint function value
 *		exitstatus:						zero in case of success else nonzero
 */
static int gradient_function_wrapper(const int ndv, const int nobj, const int ncon, const double* x, double* obj, double* g, double* df, double* dg);
/*ITERATION_FUNCTION_WRAPPER wrapper for call to iteration function from within KSOPT
 *	Input:
 *		ndv:							number of optimization variables
 *		nobj:							number of objectives
 *		ncon:							number of constraints
 *		x:								current optimization value
 *		obj:							current objective function value
 *		g:								current constraint function value
 *		df:								gradient of current objective function value
 *		dg:								gradient of current constraint function value
 *		iteration_info:					structure with information about iteration function
 *		state:							current solution state of algorithm
 *	Output:
 *		stop:							indicator if optimization should be stopped
 *		exitstatus:						zero in case of success else nonzero
 */
static int iteration_function_wrapper(const int ndv, const int nobj, const int ncon, const double* x, const double* obj, const double* g, const double* df, const double* dg, const KSOPTIter iteration_info, const KSOPT_ALGORITHM_STATE state, unsigned char* stop);

// Function Prototypes for Linker
#ifdef __cplusplus
	extern "C" double floor(double);
	extern "C" double ceil(double);
#else
	extern double floor(double);
	extern double ceil(double);
#endif

//Macros
#define CHECK(cond, msg) if (!(cond)) { mexErrMsgIdAndTxt("KSOPT:input", msg); }
#define KSOPT_ERR(cond, msg) if (!(cond)) { clearVariables(&options); mexErrMsgIdAndTxt("KSOPT:input", msg); };
// initialize fname in objective and constraint structures with \0
#define INIT_FNAME(type) \
	{\
		unsigned int fnameii = 0;\
		for (fnameii = 0; fnameii < FLEN; ++fnameii) {\
			type ## _function_info.f[fnameii] = '\0';\
		}\
	}

//Argument Enums (in expected order of arguments)
enum {argFUN, argX0, argA, argB, argAEQ, argBEQ, argLB, argUB, argNLCON, argOPTIONS};
//PRHS Defines
// pointer to objective function
#define pargFUN		prhs[argFUN]
// pointer to initial value
#define pargX0		prhs[argX0]
// pointer to matrix of linear inequality constraints
#define pargA		prhs[argA]
// pointer to linear inequality upper bound
#define pargB		prhs[argB]
// pointer to matrix of linear equality constraints
#define pargAEQ		prhs[argAEQ]
// pointer to linear equality upper bound
#define pargBEQ		prhs[argBEQ]
// pointer to lower bound
#define pargLB		prhs[argLB]
// pointer to upper bound
#define pargUB		prhs[argUB]
// pointer to nonlinear contraint function
#define pargNLCON	prhs[argNLCON]
// pointer to options
#define pargOPTIONS	prhs[argOPTIONS]

//Output Argument Enums (in expected order of arguments)
enum {argXOPT, argFUNOPT, argEXITFLAG, argOUTPUT};
//PRHS Defines
// pointer to optimal solution
#define pargXOPT		plhs[argXOPT]
// pointer to optimal function value
#define pargFUNOPT		plhs[argFUNOPT]
// pointer to exitflag
#define pargEXITFLAG	plhs[argEXITFLAG]
// pointer to output structure
#define pargOUTPUT		plhs[argOUTPUT]

//Argument Enums (in expected order of arguments)
static enum {fieldITER, fieldFUNEVALS, fieldTOLCON, fieldSTEPSIZE, fieldALGORITHM, fieldFIRSTORDEROPT, fieldTIME};
#define nfieldnamesOUTPUT 7
static const char *fieldnamesOUTPUT[] = {
	"iterations",
	"funcCount",
	"constrviolation",
	"stepsize",
	"algorithm",
	"firstorderopt",
	"time"
};
//Argument Enums (in expected order of arguments)
static enum {
	//fieldITERattainfactor,
	//fieldITERcgiterations,
	fieldITERconstrviolation,
	fieldITERdegenerate,
	//fieldITERdirectionalderivative,
	fieldITERfirstorderopt,
	fieldITERfunccount,
	fieldITERfval,
	fieldITERgradient,
	fieldITERiteration,
	//fieldITERlambda,
	//fieldITERlssteplength,
	fieldITERmaxfval,
	//fieldITERpositivedefinite,
	//fieldITERprocedure,
	//fieldITERratio,
	//fieldITERresidual,
	//fieldITERresnorm,
	//fieldITERsearchdirection,
	//fieldITERstepaccept,
	fieldITERstepsize,
	//fieldITERtrustregionradius
};
#define nfieldnamesITER 9
static const char *fieldnamesITER[] = {
	//"attainfactor",
	//"cgiterations",
	"constrviolation",
	"degenerate",
	//"directionalderivative",
	"firstorderopt",
	"funccount",
	"fval",
	"gradient",
	"iteration",
	//"lambda",
	//"lssteplength",
	"maxfval",
	//"positivedefinite",
	//"procedure",
	//"ratio",
	//"residual",
	//"resnorm",
	//"searchdirection",
	//"stepaccept",
	"stepsize",
	//"trustregionradius"
};
static const char *iterfunSTATES[] = {
	"init",
	"iterrupt",
	"iter",
	"done"
};

//Ctrl-C Detection
#ifdef __cplusplus
	extern "C" bool utIsInterruptPending(void);
	extern "C" void utSetInterruptPending(bool);
#else
	extern bool utIsInterruptPending(void);
	extern void utSetInterruptPending(bool);
#endif

//Global X, C, J for avoiding multiple constraint and jacobian evaluations
// global indicator for first evaluation of objective function wrapper
static boolean_T FIRSTEVALUATION = true;
// global cache for current iterate
static double *X = NULL;
// global storage for objective function and objective gradient value
static double *OBJ = NULL, *GRAD_OBJ = NULL;
// global storage for constraint function and constraint gradient value
static double *CONSTR = NULL, *GRAD_CONSTR = NULL;
// global storage for linear constraint matrices
static double *A_ineq = NULL, *b_ineq = NULL, *A_eq = NULL, *b_eq = NULL;
// global storage with information about objective function
static KSOPT_objective_function objective_function_info;
// global storage with information about constraint function
static KSOPT_constraint_function constraint_function_info;
// variables for exceptions during objective or constraint function evaluation
static mxArray *objective_exception = NULL;
static mxArray *constraint_exception = NULL;
static mxArray *iter_exception = NULL;
static mxArray *objective_exception_internal = NULL;
static mxArray *constraint_exception_internal = NULL;
static mxArray *iter_exception_internal = NULL;
// global storage for iteration function
static KSOPT_iter_function iter_function;

//Main Function
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	/*[x, fval, exitflag, output] = ksopt_optimize(fun, x0, A, b, A_eq, b_eq, lb, ub, nonlcon, options)
		Input:
			fun:		function handle or char array with function name of objective function
			x0:			initial value for optimization
			A:			matrix of inequalities
			b:			right hand side of inequalities
			Aeq:		matrix of equalities
			beq:		right hand side of equalities
			lb:			lower bounds
			ub:			upper bounds
			nonlcon:	function handle or char array with function name of constraint function
			options:	structure with options (same fields as optimoptions)
		Output:
			x:			optimal value
			fval:		optimal function value
			exitflag:	indicator of success
			output:		structure with information about optimization
	*/
#ifdef __cplusplus
	try {
#endif
	//Input Args
	double *lb = NULL, *ub = NULL;

	//Return Args
	double *x0 = NULL, *fval = NULL, *exitflag = NULL;
	int ksexitflag = 0;
	mxArray *funevalfield = NULL, *iterfield = NULL, *tolconfield = NULL, *firstorderoptfield = NULL, *stepsizefield = NULL, *timefield = NULL;
	double *funevalfieldvalue = NULL, *iterfieldvalue = NULL, *tolconfieldvalue = NULL, *firstorderoptfieldvalue = NULL, *stepsizefieldvalue = NULL, *timefieldvalue = NULL;

	// KSOPT return values and input variables
	int nfeval = 0, ngeval = 0;
	unsigned int number_variables_us = 0, number_objectives_us = 0, number_constraints_us = 0;
	KSOPToptionType options;
	KSOPTResult result;
	int printLevel = 0;

	//Internal Vars
	size_t ndec = 0, number_variables = 0, number_objectives = 0, number_constraints = 0, number_constraints_ineq = 0, number_constraints_eq = 0, number_constraints_linear_ineq = 0, number_constraints_linear_eq = 0;
	double *alb = NULL, *aub = NULL;
	doublereal *x_opt_KSOPT = NULL;
	KSOPT_functionHandleInformation objectiveinformation;
	KSOPT_functionHandleInformation constraintinformation;
	KSOPT_iterfunctionHandleInformation iterinformationplot;
	KSOPT_iterfunctionHandleInformation iterinformationouput;

	// temporary variables
	unsigned int ii = 0;
	int retval = 0;
	mxArray *error_inputs[2];
	boolean_T needsgradient = false;
	void *memoryptr = NULL;
	char *errormessage = NULL;
	boolean_T isX0outofbounds = false;
	KSOPT_OPTIONTYPE_DEFAULT
	#if defined DEBUG_FLAG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
	#endif
	/*//User Function Structure
	obj_fun_data fun;
	//Iteration Callback Structure
	iter_fun_data iterF;*/

	//Check for ver display
	if (nrhs < 1) {
		if (nlhs < 1)
			printSolverInfo();
		else {
			plhs[0] = mxCreateString("KSOPT MEX Interface " TOSTRING(PACKAGE_VERSION));
		}
		return;
	}
	if (nlhs > 4) {
		mexErrMsgIdAndTxt("KSOPT:input", "Too many output arguments.");
		return;
	}
	if (sizeof(double) != sizeof(doublereal)) {
		mexErrMsgIdAndTxt("KSOPT:input", "Mex function must be compiled on a platform where double == doublereal.");
		return;
	}

	// default options
	options = defaultOptions;

	objectiveinformation.number_objectives = 0;
	objectiveinformation.hasgradient = false;
	objectiveinformation.number_constraints_ineq = 0;
	objectiveinformation.number_constraints_eq = 0;
	objectiveinformation.number_inputs = 0;
	objectiveinformation.number_outputs = 0;
	objectiveinformation.type = KSOPT_OBJECTIVE;
	objectiveinformation.wasevaluated = false;

	constraintinformation.number_objectives = 0;
	constraintinformation.hasgradient = false;
	constraintinformation.number_constraints_ineq = 0;
	constraintinformation.number_constraints_eq = 0;
	constraintinformation.number_inputs = 0;
	constraintinformation.number_outputs = 0;
	constraintinformation.type = KSOPT_CONSTRAINT;
	constraintinformation.wasevaluated = false;

	iterinformationplot.type = KSOPT_ITER;
	iterinformationplot.number_functions = 0;
	iterinformationplot.number_inputs = NULL;
	iterinformationplot.number_outputs = NULL;

	iterinformationouput.type = KSOPT_ITER;
	iterinformationouput.number_functions = 0;
	iterinformationouput.number_inputs = NULL;
	iterinformationouput.number_outputs = NULL;

	constraint_function_info.nrhs = 0;
	constraint_function_info.prhs[0] = NULL;
	constraint_function_info.prhs[1] = NULL;
	constraint_function_info.xrhs = 0;
	constraint_function_info.info = NULL;
	constraint_function_info.plhs[0] = NULL;
	constraint_function_info.plhs[1] = NULL;
	constraint_function_info.plhs[2] = NULL;
	constraint_function_info.plhs[3] = NULL;
	constraint_function_info.number_equality_linear = 0;
	constraint_function_info.number_inequality_linear = 0;
	INIT_FNAME(constraint);

	objective_function_info.nrhs = 0;
	objective_function_info.prhs[0] = NULL;
	objective_function_info.prhs[1] = NULL;
	objective_function_info.xrhs = 0;
	objective_function_info.info = NULL;
	objective_function_info.plhs[0] = NULL;
	objective_function_info.plhs[0] = NULL;
	INIT_FNAME(objective);

	error_inputs[0] = NULL;
	error_inputs[1] = NULL;

	//Check Inputs
	checkInputs(prhs, nrhs, &options, &objectiveinformation, &constraintinformation, &number_objectives, &number_constraints_ineq, &number_constraints_eq, &number_constraints_linear_ineq, &number_constraints_linear_eq, &iterinformationplot, &iterinformationouput);
	number_objectives = objectiveinformation.number_objectives;
	number_objectives_us = (unsigned int) max(0, number_objectives);
	number_constraints_ineq = constraintinformation.number_constraints_ineq;
	number_constraints_eq = constraintinformation.number_constraints_eq;
	nfeval += (int)objectiveinformation.wasevaluated;
	ngeval += (int)constraintinformation.wasevaluated;
	printLevel = options.print;

	//Get Sizes
	number_variables = mxGetM(pargX0);
	number_variables_us = (unsigned int)max(0, number_variables);
	ndec = mxGetNumberOfElements(pargX0);
	//Get Objective Function Handle
	if (mxIsChar(pargFUN)) {
		CHECK(mxGetString(pargFUN, objective_function_info.f, FLEN) == 0, "Error reading Objective function");
		objective_function_info.prhs[0] = (mxArray*)NULL;
		objective_function_info.prhs[1] = (mxArray*)NULL;
		objective_function_info.f[255] = '\0';
		objective_function_info.nrhs = 1;
		objective_function_info.xrhs = 0;
	}
	else {
		objective_function_info.prhs[0] = (mxArray*)mxDuplicateArray(pargFUN);
		objective_function_info.prhs[1] = (mxArray*)NULL;
		strcpy(objective_function_info.f, "feval");
		objective_function_info.f[5] = '\0';
		objective_function_info.nrhs = 2;
		objective_function_info.xrhs = 1;
	}
	objective_function_info.prhs[objective_function_info.xrhs] = mxCreateDoubleMatrix(number_variables, 1, mxREAL); //x0
	objective_function_info.info = &objectiveinformation;
	if (objective_function_info.prhs[objective_function_info.xrhs] == NULL) {
		mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for optimization variable.");
		return;
	}

	if (pargA && !mxIsEmpty(pargA) && pargB && !mxIsEmpty(pargB)) {
		constraint_function_info.number_inequality_linear = mxGetM(pargA);
	}
	else {
		constraint_function_info.number_inequality_linear = 0;
	}
	if (pargAEQ && !mxIsEmpty(pargAEQ) && pargBEQ && !mxIsEmpty(pargBEQ)) {
		constraint_function_info.number_equality_linear = mxGetM(pargAEQ);
	}
	else {
		constraint_function_info.number_equality_linear = 0;
	}
	//Get Constraints Handles (if exists)
	if (pargNLCON && !mxIsEmpty(pargNLCON)) {
		if (mxIsChar(pargNLCON)) {
			CHECK(mxGetString(pargNLCON, constraint_function_info.f, FLEN) == 0, "Error reading Constraint function");
			constraint_function_info.f[255] = '\0';
			constraint_function_info.nrhs = 1;
			constraint_function_info.xrhs = 0;
		}
		else {
			constraint_function_info.prhs[0] = (mxArray*)mxDuplicateArray(pargNLCON);
			strcpy(constraint_function_info.f, "feval");
			constraint_function_info.f[5] = '\0';
			constraint_function_info.nrhs = 2;
			constraint_function_info.xrhs = 1;
		}
		constraint_function_info.prhs[constraint_function_info.xrhs] = mxCreateDoubleMatrix(number_variables, 1, mxREAL); //x0
		if (constraint_function_info.prhs[constraint_function_info.xrhs] == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for optimization variable.");
			return;
		}
		constraint_function_info.info = &constraintinformation;
	}
	else {
		constraint_function_info.nrhs = 0;
		constraint_function_info.prhs[0] = NULL;
		constraint_function_info.prhs[1] = NULL;
		constraint_function_info.xrhs = 0;
		constraint_function_info.info = &constraintinformation;
	}

	//Get Options if specified
	if (nrhs > argOPTIONS && pargOPTIONS != NULL && !mxIsEmpty(pargOPTIONS)) {
		mxArray *options_iterfun[2] = {NULL, NULL};
		boolean_T isstruct = mxIsStruct(pargOPTIONS);
		boolean_T isobject = mxIsClass(pargOPTIONS, "optim.options.SolverOptions");
		mxArray *field = NULL;
		// TODO: add PlotFcn and OutputFcn
		if (isstruct) {
			field = mxGetField(pargOPTIONS, 0, "PlotFcn");
			if (field != NULL && !mxIsEmpty(field)) {
				options_iterfun[0] = (mxArray*) field;
			}
		}
		else if (isobject) {
			field = mxGetProperty(pargOPTIONS, 0, "PlotFcn");
			if (field != NULL && !mxIsEmpty(field)) {
				options_iterfun[0] = (mxArray*) field;
			}
		}
		if (isstruct) {
			field = mxGetField(pargOPTIONS, 0, "OuputFcn");
			if (field != NULL && !mxIsEmpty(field)) {
				options_iterfun[1] = (mxArray*) field;
			}
		}
		else if (isobject) {
			field = mxGetProperty(pargOPTIONS, 0, "OutputFcn");
			if (field != NULL && !mxIsEmpty(field)) {
				options_iterfun[1] = (mxArray*) field;
			}
		}
		if (iterinformationplot.number_functions + iterinformationouput.number_functions > 0 && (options_iterfun[0] != NULL || options_iterfun[1] != NULL)) {
			mwSize celldim[2];
			size_t structdim[2];
			mwIndex cellidx = 0;
			mxArray *cellcontent = NULL;
			celldim[0] = iterinformationplot.number_functions + iterinformationouput.number_functions;
			celldim[1] = 1;
			structdim[0] = 1;
			structdim[1] = 1;
			iter_function.hasiterfun = true;
			iter_function.iter_functions = mxCreateCellArray(2, celldim);
			if (iter_function.iter_functions == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create cell array of iteration functions.");
				return;
			}
			iter_function.optimaloptimValues = mxCreateStructArray(1, structdim, nfieldnamesITER, fieldnamesITER);
			if (iter_function.optimaloptimValues == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create structure of input arguments for iteration functions.");
				return;
			}
			mxSetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERconstrviolation], mxCreateDoubleMatrix(1, 1, mxREAL));
			mxSetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERdegenerate], mxCreateDoubleMatrix(1, 1, mxREAL));
			mxSetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERfirstorderopt], mxCreateDoubleMatrix(1, 1, mxREAL));
			mxSetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERfunccount], mxCreateDoubleMatrix(1, 1, mxREAL));
			mxSetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERfval], mxCreateDoubleMatrix(number_objectives_us, 1, mxREAL));
			mxSetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERgradient], mxCreateDoubleMatrix(number_objectives_us, number_variables_us, mxREAL));
			mxSetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERiteration], mxCreateDoubleMatrix(1, 1, mxREAL));
			mxSetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERmaxfval], mxCreateDoubleMatrix(1, 1, mxREAL));
			mxSetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERstepsize], mxCreateDoubleMatrix(1, 1, mxREAL));

			if (mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERconstrviolation]) == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in iter function input structure.", fieldnamesITER[fieldITERconstrviolation]);
				return;
			}
			if (mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERdegenerate]) == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in iter function input structure.", fieldnamesITER[fieldITERdegenerate]);
				return;
			}
			if (mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERfirstorderopt]) == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in iter function input structure.", fieldnamesITER[fieldITERfirstorderopt]);
				return;
			}
			if (mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERfunccount]) == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in iter function input structure.", fieldnamesITER[fieldITERfunccount]);
				return;
			}
			if (mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERfval]) == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in iter function input structure.", fieldnamesITER[fieldITERfval]);
				return;
			}
			if (mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERgradient]) == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in iter function input structure.", fieldnamesITER[fieldITERgradient]);
				return;
			}
			if (mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERiteration]) == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in iter function input structure.", fieldnamesITER[fieldITERiteration]);
				return;
			}
			if (mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERmaxfval]) == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in iter function input structure.", fieldnamesITER[fieldITERmaxfval]);
				return;
			}
			if (mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERstepsize]) == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in iter function input structure.", fieldnamesITER[fieldITERstepsize]);
				return;
			}
			iter_function.state = mxCreateString("");
			if (iter_function.state == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create state for iteration functions.");
				return;
			}
			iter_function.x = mxCreateDoubleMatrix(number_variables_us, 1, mxREAL);
			if (iter_function.x == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create current iterate for iteration functions.");
				return;
			}
			cellidx = 0;
			if (options_iterfun[0] != NULL && mxIsCell(options_iterfun[0])) {
				for (ii = 0; ii < mxGetNumberOfElements(options_iterfun[0]); ++ii) {
					if (mxGetM(iter_function.iter_functions) < cellidx) {
						mexErrMsgIdAndTxt("KSOPT:input", "Cell array not large enough for number of iteration functions.");
						return;
					}
					cellcontent = mxGetCell(options_iterfun[0], ii);
					if (cellcontent == NULL) {
						mexErrMsgIdAndTxt("KSOPT:input", "PlotFcn contains invalid cell elements.");
						return;
					}
					if (!mxIsFunctionHandle(cellcontent)) {
						mexErrMsgIdAndTxt("KSOPT:input", "PlotFcn must be a cell array of function handles.");
						return;
					}
					mxSetCell(iter_function.iter_functions, cellidx, mxDuplicateArray(cellcontent));
					++cellidx;
				}
			}
			else if (options_iterfun[0] != NULL && mxIsFunctionHandle(options_iterfun[0])) {
				if (mxGetM(iter_function.iter_functions) < cellidx) {
					mexErrMsgIdAndTxt("KSOPT:input", "Cell array not large enough for number of iteration functions.");
					return;
				}
				mxSetCell(iter_function.iter_functions, cellidx, mxDuplicateArray(options_iterfun[0]));
				++cellidx;
			}
			if (options_iterfun[1] != NULL && mxIsCell(options_iterfun[1])) {
				for (ii = 0; ii < mxGetNumberOfElements(options_iterfun[1]); ++ii) {
					if (mxGetM(iter_function.iter_functions) < cellidx) {
						mexErrMsgIdAndTxt("KSOPT:input", "Cell array not large enough for number of iteration functions.");
						return;
					}
					cellcontent = mxGetCell(options_iterfun[1], ii);
					if (cellcontent == NULL) {
						mexErrMsgIdAndTxt("KSOPT:input", "OutputFcn contains invalid cell elements.");
						return;
					}
					if (!mxIsFunctionHandle(cellcontent)) {
						mexErrMsgIdAndTxt("KSOPT:input", "OutputFcn must be a cell array of function handles.");
						return;
					}
					mxSetCell(iter_function.iter_functions, cellidx, mxDuplicateArray(cellcontent));
					++cellidx;
				}
			}
			else if (options_iterfun[1] != NULL && mxIsFunctionHandle(options_iterfun[1])) {
				if (mxGetM(iter_function.iter_functions) < cellidx) {
					mexErrMsgIdAndTxt("KSOPT:input", "Cell array not large enough for number of iteration functions.");
					return;
				}
				mxSetCell(iter_function.iter_functions, cellidx, mxDuplicateArray(options_iterfun[1]));
				++cellidx;
			}
			if (iter_function.hasiterfun) {
				options.iterfun = iteration_function_wrapper;
			}
		}
		else {
			iter_function.hasiterfun = false;
			iter_function.iter_functions = NULL;
			iter_function.optimaloptimValues = NULL;
			iter_function.state = NULL;
			iter_function.x = NULL;
		}
	}
	else {
		iter_function.hasiterfun = false;
		iter_function.iter_functions = NULL;
		iter_function.optimaloptimValues = NULL;
		iter_function.state = NULL;
		iter_function.x = NULL;
	}
	needsgradient = objectiveinformation.hasgradient;
	needsgradient = needsgradient || (constraintinformation.number_constraints_ineq + constraintinformation.number_constraints_eq <= 0 || constraintinformation.hasgradient);
	needsgradient = needsgradient || (number_constraints_linear_ineq + number_constraints_linear_eq <= 0);
	needsgradient = needsgradient && options.gradient;

	//Create Outputs
	pargXOPT = mxCreateDoubleMatrix(number_variables_us, 1, mxREAL);
	if (pargXOPT == NULL) {
		mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for optimal variable.");
		return;
	}
	if (nlhs > argFUNOPT) {
		pargFUNOPT = mxCreateDoubleMatrix(number_objectives_us, 1, mxREAL);
		if (pargFUNOPT == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for optimal objective function value.");
			return;
		}
		fval = mxGetPr(pargFUNOPT);
	}
	else {
		pargFUNOPT = NULL;
		fval = NULL;
	}
	if (nlhs > argEXITFLAG) {
		pargEXITFLAG = mxCreateDoubleMatrix(1, 1, mxREAL);
		if (pargEXITFLAG == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for exitflag.");
			return;
		}
		exitflag = mxGetPr(pargEXITFLAG);
	}
	else {
		pargEXITFLAG = NULL;
		exitflag = NULL;
	}
	if (nlhs > argOUTPUT) {
		pargOUTPUT = mxCreateStructMatrix(1, 1, nfieldnamesOUTPUT, fieldnamesOUTPUT);
		if (pargOUTPUT == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for information structure.");
			return;
		}
		mxSetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldITER], mxCreateDoubleMatrix(1, 1, mxREAL));
		mxSetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldFUNEVALS], mxCreateDoubleMatrix(1, 1, mxREAL));
		mxSetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldALGORITHM], mxCreateString("KSOPT"));
		mxSetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldFIRSTORDEROPT], mxCreateDoubleMatrix(1, 1, mxREAL));
		mxSetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldSTEPSIZE], mxCreateDoubleMatrix(1, 1, mxREAL));
		mxSetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldTOLCON], mxCreateDoubleMatrix(1, 1, mxREAL));
		mxSetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldTIME], mxCreateDoubleMatrix(1, 1, mxREAL));
		funevalfield = mxGetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldFUNEVALS]);
		if (funevalfield == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in information structure.", fieldnamesOUTPUT[fieldFUNEVALS]);
			return;
		}
		iterfield = mxGetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldITER]);
		if (iterfield == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in information structure.", fieldnamesOUTPUT[fieldITER]);
			return;
		}
		tolconfield = mxGetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldTOLCON]);
		if (tolconfield == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in information structure.", fieldnamesOUTPUT[fieldTOLCON]);
			return;
		}
		firstorderoptfield = mxGetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldFIRSTORDEROPT]);
		if (firstorderoptfield == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in information structure.", fieldnamesOUTPUT[fieldFIRSTORDEROPT]);
			return;
		}
		stepsizefield = mxGetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldSTEPSIZE]);
		if (stepsizefield == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in information structure.", fieldnamesOUTPUT[fieldSTEPSIZE]);
			return;
		}
		timefield = mxGetField(pargOUTPUT, 0, fieldnamesOUTPUT[fieldTIME]);
		if (timefield == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for field '%s' in information structure.", fieldnamesOUTPUT[fieldTIME]);
			return;
		}
		funevalfieldvalue = mxGetPr(funevalfield);
		iterfieldvalue = mxGetPr(iterfield);
		tolconfieldvalue = mxGetPr(tolconfield);
		firstorderoptfieldvalue = mxGetPr(firstorderoptfield);
		stepsizefieldvalue = mxGetPr(stepsizefield);
		timefieldvalue = mxGetPr(timefield);
	}
	else {
		funevalfieldvalue = NULL;
		iterfieldvalue = NULL;
		tolconfieldvalue = NULL;
		firstorderoptfieldvalue = NULL;
		stepsizefieldvalue = NULL;
		timefieldvalue = NULL;
	}

	//Set Bounds
	alb = (double*) mxCalloc(number_variables, sizeof(double));
	if (alb == NULL) {
		mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to copy lower bounds.");
		return;
	}
	aub = (double*) mxCalloc(number_variables, sizeof(double));
	if (aub == NULL) {
		mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to copy upper bounds.");
		return;
	}
	if (nrhs > argLB && pargLB != NULL && !mxIsEmpty(pargLB)) {
		if (mxIsSparse(pargLB)) {
			int success = -1;
			double* lb_KSOPT_fromsparse = (double*) mxCalloc(number_variables_us, sizeof(double));
			if (lb_KSOPT_fromsparse == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to copy lower bounds.");
				return;
			}
			success = ksopt_sparse2full(pargLB, lb_KSOPT_fromsparse, number_variables_us, 1, true);
			if (success != 0) {
				mexErrMsgIdAndTxt("KSOPT:input", "Could not convert sparse lower bounds to full.");
				return;
			}
			for (ii = 0; ii < number_variables_us; ++ii) {
				if (mxIsInf(lb_KSOPT_fromsparse[ii])) {
					alb[ii] = -HUGE_VAL;
				}
				else {
					alb[ii] = lb_KSOPT_fromsparse[ii];
				}
			}
			if (lb_KSOPT_fromsparse != NULL) {
				mxFree(lb_KSOPT_fromsparse);
				lb_KSOPT_fromsparse = NULL;
			}
		}
		else {
			lb = mxGetPr(pargLB);
			for (ii = 0; ii < number_variables_us; ++ii) {
				if (mxIsInf(lb[ii])) {
					alb[ii] = -HUGE_VAL;
				}
				else {
					alb[ii] = lb[ii];
				}
			}
		}
	}
	else {
		for (ii = 0; ii < number_variables_us; ++ii) {
			alb[ii] = -HUGE_VAL;
		}
	}

	if (nrhs > argUB && pargUB && !mxIsEmpty(pargUB)) {
		if (mxIsSparse(pargUB)) {
			int success = -1;
			double* ub_KSOPT_fromsparse = (double*) mxCalloc(number_variables_us, sizeof(double));
			if (ub_KSOPT_fromsparse == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to copy upper bounds.");
				return;
			}
			success = ksopt_sparse2full(pargUB, ub_KSOPT_fromsparse, number_variables_us, 1, true);
			if (success != 0) {
				mexErrMsgIdAndTxt("KSOPT:input", "Could not convert sparse upper bounds to full.");
				return;
			}
			for (ii = 0; ii < number_variables_us; ++ii) {
				if (mxIsInf(ub_KSOPT_fromsparse[ii])) {
					aub[ii] = HUGE_VAL;
				}
				else {
					aub[ii] = ub_KSOPT_fromsparse[ii];
				}
			}
			if (ub_KSOPT_fromsparse != NULL) {
				mxFree(ub_KSOPT_fromsparse);
				ub_KSOPT_fromsparse = NULL;
			}
		}
		else {
			ub = mxGetPr(pargUB);
			for (ii = 0; ii < number_variables_us; ++ii) {
				if (mxIsInf(ub[ii])) {
					aub[ii] = HUGE_VAL;
				}
				else {
					aub[ii] = ub[ii];
				}
			}
		}
	}
	else {
		for (ii = 0; ii < number_variables_us; ++ii) {
			aub[ii] = HUGE_VAL;
		}
	}

	number_constraints = constraint_function_info.info->number_constraints_ineq + 2*constraint_function_info.info->number_constraints_eq + constraint_function_info.number_inequality_linear + 2*constraint_function_info.number_equality_linear;
	number_constraints_us = (unsigned int) max(0, number_constraints);

	//Copy initial guess to x
	x0 = mxGetPr(pargX0);
	x_opt_KSOPT = (doublereal*) mxCalloc(number_variables_us, sizeof(doublereal));
	if (x_opt_KSOPT == NULL) {
		mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to copy optimization variable.");
		return;
	}
	if (mxIsSparse(pargX0)) {
		int success = -1;
		double* x_opt_KSOPT_fromsparse = (double*) mxCalloc(number_variables_us, sizeof(double));
		if (x_opt_KSOPT == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to copy optimization variable.");
			return;
		}
		success = ksopt_sparse2full(pargX0, x_opt_KSOPT_fromsparse, number_variables_us, 1, true);
		if (success != 0) {
			mexErrMsgIdAndTxt("KSOPT:input", "Could not convert sparse initial value to full.");
			return;
		}
		for (ii = 0; ii < number_variables_us; ++ii) {
			if (x_opt_KSOPT_fromsparse[ii] < alb[ii]) {
				x_opt_KSOPT[ii] = (doublereal) alb[ii];
				isX0outofbounds = true;
			}
			else if (x_opt_KSOPT_fromsparse[ii] > aub[ii]) {
				x_opt_KSOPT[ii] = (doublereal) aub[ii];
				isX0outofbounds = true;
			}
			else {
				x_opt_KSOPT[ii] = (doublereal) x_opt_KSOPT_fromsparse[ii];
			}
		}
		if (x_opt_KSOPT_fromsparse != NULL) {
			mxFree(x_opt_KSOPT_fromsparse);
			x_opt_KSOPT_fromsparse = NULL;
		}
	}
	else {
		for (ii = 0; ii < number_variables_us; ++ii) {
			if (x0[ii] < alb[ii]) {
				x_opt_KSOPT[ii] = (doublereal) alb[ii];
				isX0outofbounds = true;
			}
			else if (x0[ii] > aub[ii]) {
				x_opt_KSOPT[ii] = (doublereal) aub[ii];
				isX0outofbounds = true;
			}
			else {
				x_opt_KSOPT[ii] = (doublereal) x0[ii];
			}
		}
	}
	if (isX0outofbounds && printLevel != 0) {
		mexPrintf("Initial point x0 is not between bounds lb and ub and is shifted to strictly satisfy the bounds.");
	}

	//Create Global Memory for storing previous evaluations
	X = (double*) mxCalloc(number_variables_us, sizeof(double));
	if (X == NULL) {
		mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to cache for optimization variable.");
		return;
	}
	OBJ = (double*) mxCalloc(number_objectives_us, sizeof(double));
	if (OBJ == NULL) {
		mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to cache objective function value.");
		return;
	}
	if (number_constraints_us > 0) {
		CONSTR = (double*) mxCalloc(number_constraints_us, sizeof(double));
		if (CONSTR == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to cache constraint function value.");
			return;
		}
	}
	else {
		CONSTR = NULL;
	}
	if (needsgradient) {
		GRAD_OBJ = (double*) mxCalloc(number_objectives_us*number_variables_us, sizeof(double));
		if (GRAD_OBJ == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to cache for objective function gradient.");
			return;
		}
		if (number_constraints_us > 0) {
			GRAD_CONSTR = (double*) mxCalloc(number_constraints_us*number_variables_us, sizeof(double));
			if (GRAD_CONSTR == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to cache for constraint function gradient.");
				return;
			}
		}
		else {
			GRAD_CONSTR = NULL;
		}
	}
	else {
		GRAD_OBJ = NULL;
		GRAD_CONSTR = NULL;
	}

	//Print Header
	if (printLevel != 0) {
		mexPrintf("\n------------------------------------------------------------------\n");
		mexPrintf(" This is KSOPT v%s\n", PACKAGE_VERSION);
		mexPrintf(" Author: Gregory A. Wrenn\n\n");
		mexPrintf(" MEX Interface: Patrick Vogt\n\n");
		mexPrintf(" Problem Properties:\n");
		mexPrintf(" # Decision Variables:     %4d\n", number_variables);
		mexPrintf(" # Objectives:             %4d\n", number_objectives);
		mexPrintf(" # Inequality Constraints: %4d\n", constraintinformation.number_constraints_ineq + number_constraints_linear_ineq);
		mexPrintf(" # Equality Constraints:   %4d\n", constraintinformation.number_constraints_eq + number_constraints_linear_eq);

		mexPrintf("------------------------------------------------------------------\n");
		mexEvalString("drawnow;");
	}
	result.ndv = (int) number_variables_us;
	result.nobj = (int) number_objectives_us;
	result.ncon = (int) number_constraints_us;
	result.xopt = (double*) mxCalloc(number_variables_us, sizeof(double));
	result.exitflag = 0;
	result.stepsize = 0.0;
	result.tolcon = 0.0;
	result.tolopt = 0.0;
	result.fevals = 0;
	result.time = 0.0;
	if (result.xopt == NULL) {
		mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for optimal x.");
		return;
	}
	result.fopt = (double*) mxCalloc(number_objectives_us, sizeof(double));
	if (result.fopt == NULL) {
		mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for optimal objective function value.");
		return;
	}
	if (number_constraints_us > 0) {
		result.conopt = (double*) mxCalloc(number_constraints_us, sizeof(double));
		if (result.conopt == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to create output for optimal constraint function value.");
			return;
		}
	}
	else {
		result.conopt = NULL;
	}

	//Solve Problem
	ksexitflag = ksmain((const integer)number_variables_us, (const integer)number_objectives_us, (const integer)number_constraints_us, x_opt_KSOPT, (const doublereal*)alb, (const doublereal*)aub, options, (integer*)&nfeval, (integer*)&ngeval, objective_function_wrapper, gradient_function_wrapper, &result);
	if (ksexitflag != 0 && ksexitflag > 10) {
		errormessage = (char*) mxCalloc(KSOPT_ERROR_MESSAGE_MAXSIZE + 10, sizeof(char));
		if (errormessage == NULL) {
			clearVariables(&options, &iterinformationplot, &iterinformationouput, number_objectives, number_constraints_ineq, number_constraints_eq, number_constraints_linear_ineq, number_constraints_linear_eq);
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to copy error message.");
			return;
		}
		retval = ksgeterrormessage((const int)ksexitflag, errormessage, KSOPT_ERROR_MESSAGE_MAXSIZE + 10);
		if (retval != 0) {
			clearVariables(&options, &iterinformationplot, &iterinformationouput, number_objectives, number_constraints_ineq, number_constraints_eq, number_constraints_linear_ineq, number_constraints_linear_eq);
			mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to copy error message.");
			return;
		}
		clearVariables(&options, &iterinformationplot, &iterinformationouput, number_objectives, number_constraints_ineq, number_constraints_eq, number_constraints_linear_ineq, number_constraints_linear_eq);
		mexErrMsgIdAndTxt("KSOPT:input", (const char*)errormessage);
		return;
	}
	//Set Outputs
	if (nlhs > argXOPT && pargXOPT != NULL) {
		x0 = mxGetPr(pargXOPT);
		if (x0 != NULL && result.xopt != NULL) {
			for (ii = 0; ii < number_variables_us; ++ii) {
				x0[ii] = (double)result.xopt[ii];
			}
		}
	}
	if (nlhs > argFUNOPT && fval != NULL) {
		if (result.fopt != NULL) {
			memoryptr = memcpy(fval, result.fopt, number_objectives_us*sizeof(double));
			if (memoryptr == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to copy optimal function value.");
				return;
			}
		}
	}
	if (nlhs > argEXITFLAG && exitflag != NULL) {
		if (ksexitflag == 0) {
			*exitflag = result.exitflag;
		}
		else {
			*exitflag = (double) ksexitflag;
		}
	}
	if (nlhs > argOUTPUT) {
		if (funevalfieldvalue != NULL) {
			*funevalfieldvalue = (double) result.fevals;
		}
		if (iterfieldvalue != NULL) {
			*iterfieldvalue = (double)result.iter;
		}
		if (tolconfieldvalue != NULL) {
			*tolconfieldvalue = (double)result.tolcon;
		}
		if (firstorderoptfieldvalue != NULL) {
			*firstorderoptfieldvalue = (double)result.tolopt;
		}
		if (stepsizefieldvalue != NULL) {
			*stepsizefieldvalue = (double)result.stepsize;
		}
		if (timefieldvalue != NULL) {
			*timefieldvalue = (double)result.time;
		}
	}

	//Print Header
	if (printLevel != 0) {
		//Termination Detected
		switch ((int)result.exitflag) {
			//Success
			case KSOPT_SUCCESS:
				if (number_constraints_us > 0) {
					mexPrintf("\n *** Found a local minimum that satisfies the constraints. ***\n");
				}
				else {
					mexPrintf("\n *** Found a local minimum. ***\n");
				}
				break;
			case KSOPT_MAXITER:
				mexPrintf("\n *** Maximum number of iterations reached. ***\n");
				break;
			case KSOPT_INFEASIBLE:
				mexPrintf("\n *** No feasible solution could be found. ***\n");
				break;
			case KSOPT_MAXFUNEVALS:
				mexPrintf("\n *** Maximum number of function evaluations reached. ***\n");
				break;
			case KSOPT_MAXTIME:
				mexPrintf("\n *** Maximum time reached. ***\n");
				break;
			case KSOPT_OBJECTIVELIMIT:
				mexPrintf("\n *** Objective function at current iteration went below ObjectiveLimit. ***\n");
				break;
			case KSOPT_USERINTERRUPT:
				mexPrintf("\n *** ERROR: User cancelled optimization. ***\n");
				break;
			// Errors
			case KSOPT_INTERNAL_ERROR:
				mexPrintf("\n *** ERROR: An unknown error occured in ksopt. ***\n");
				break;
			case KSOPT_INTERNAL_OBJECTIVEERROR:
				mexPrintf("\n *** ERROR: Internal error in objective function. ***\n");
				break;
			case KSOPT_INTERNAL_CONSTRAINTERROR:
				mexPrintf("\n *** ERROR: Internal error in constraint function. ***\n");
				break;
			case KSOPT_INTERNAL_ITERERROR:
				mexPrintf("\n *** ERROR: Internal error in iteration function. ***\n");
				break;
			case KSOPT_MATLAB_OBJECTIVEERROR:
				mexPrintf("\n *** ERROR: Error in objective function. ***\n");
				break;
			case KSOPT_MATLAB_CONSTRAINTERROR:
				mexPrintf("\n *** ERROR: Error in constraint function. ***\n");
				break;
			case KSOPT_MATLAB_ITERERROR:
				mexPrintf("\n *** ERROR: Error in iteration function. ***\n");
				break;
			case KSOPT_PLOTINTERRUPT:
				mexPrintf("\n *** ERROR: Optimization was stopped by an output or plot function. ***\n");
				break;
			//Other Error
			default:
				mexPrintf("\n *** ERROR: Internal error code %d and %d. ***\n", result.exitflag, ksexitflag);
				break;
		}

		if (ksexitflag == 0) {
			double maxfval = -HUGE_VAL;
			if (result.fopt != NULL) {
				maxfval = result.fopt[0];
				for (ii = 1; ii < (unsigned int) result.nobj; ++ii) {
					if (result.fopt[ii] > maxfval) {
						maxfval = result.fopt[ii];
					}
				}
			}
			else {
				maxfval = mxGetNaN();
			}
			mexPrintf("\n Final fval: %12.5g\n In %3.0f function evaluations\n", maxfval, (double)result.fevals);
		}

		mexPrintf("------------------------------------------------------------------\n\n");
	}

	clearVariables(&options, &iterinformationplot, &iterinformationouput, number_objectives, number_constraints_ineq, number_constraints_eq, number_constraints_linear_ineq, number_constraints_linear_eq);

	//Clean Up
	if (x_opt_KSOPT != NULL) {
		mxFree(x_opt_KSOPT);
		x_opt_KSOPT = NULL;
	}
	if (alb != NULL) {
		mxFree(alb);
		alb = NULL;
	}
	if (aub != NULL) {
		mxFree(aub);
		aub = NULL;
	}
	if (X != NULL) {
		mxFree(X);
		X = NULL;
	}
	if (OBJ != NULL) {
		mxFree(OBJ);
		OBJ = NULL;
	}
	if (CONSTR != NULL) {
		mxFree(CONSTR);
		CONSTR = NULL;
	}
	if (GRAD_OBJ != NULL) {
		mxFree(GRAD_OBJ);
		GRAD_OBJ = NULL;
	}
	if (GRAD_CONSTR != NULL) {
		mxFree(GRAD_CONSTR);
		GRAD_CONSTR = NULL;
	}
	if (A_ineq != NULL) {
		mxFree(A_ineq);
		A_ineq = NULL;
	}
	if (b_ineq != NULL) {
		mxFree(b_ineq);
		b_ineq = NULL;
	}
	if (A_eq != NULL) {
		mxFree(A_eq);
		A_eq = NULL;
	}
	if (b_eq != NULL) {
		mxFree(b_eq);
		b_eq = NULL;
	}
	if (iter_function.hasiterfun) {
		if (iter_function.state != NULL) {
			mxDestroyArray(iter_function.state);
			iter_function.state = NULL;
		}
		if (iter_function.x != NULL) {
			mxDestroyArray(iter_function.x);
			iter_function.x = NULL;
		}
		if (iter_function.iter_functions != NULL) {
			mxDestroyArray(iter_function.iter_functions);
			iter_function.iter_functions = NULL;
		}
		if (iter_function.optimaloptimValues != NULL) {
			mxDestroyArray(iter_function.optimaloptimValues);
			iter_function.optimaloptimValues = NULL;
		}
	}
	if (objective_exception != NULL) {
		if (mxIsClass(objective_exception, "MException")) {
			error_inputs[0] = objective_exception;
			error_inputs[1] = NULL;
			mexCallMATLAB(0, (mxArray **)NULL, 1, error_inputs, "throw");
			return;
		}
		else if (mxIsChar(objective_exception)) {
			error_inputs[0] = mxCreateString("KSOPT:internal");
			error_inputs[1] = objective_exception;
			mexCallMATLAB(0, (mxArray **)NULL, 2, error_inputs, "error");
			return;
		}
		else {
			mexErrMsgIdAndTxt("KSOPT:input", "Unknown error of type '%s' in objective function", mxGetClassName(objective_exception));
			return;
		}
	}
	if (constraint_exception != NULL) {
		if (mxIsClass(constraint_exception, "MException")) {
			error_inputs[0] = constraint_exception;
			error_inputs[1] = NULL;
			mexCallMATLAB(0, (mxArray **)NULL, 1, error_inputs, "throw");
			return;
		}
		else if (mxIsChar(constraint_exception)) {
			error_inputs[0] = mxCreateString("KSOPT:internal");
			error_inputs[1] = constraint_exception;
			mexCallMATLAB(0, (mxArray **)NULL, 1, error_inputs, "error");
			return;
		}
		else {
			mexErrMsgIdAndTxt("KSOPT:input", "Unknown error of type '%s' in objective function", mxGetClassName(constraint_exception));
			return;
		}
	}
	if (iter_exception != NULL) {
		if (mxIsClass(iter_exception, "MException")) {
			error_inputs[0] = iter_exception;
			error_inputs[1] = NULL;
			mexCallMATLAB(0, (mxArray **)NULL, 1, error_inputs, "throw");
			return;
		}
		else if (mxIsChar(iter_exception)) {
			error_inputs[0] = mxCreateString("KSOPT:internal");
			error_inputs[1] = iter_exception;
			mexCallMATLAB(0, (mxArray **)NULL, 1, error_inputs, "error");
			return;
		}
		else {
			mexErrMsgIdAndTxt("KSOPT:input", "Unknown error of type '%s' in iteration function", mxGetClassName(iter_exception));
			return;
		}
	}
	if (objective_exception_internal != NULL) {
		error_inputs[0] = mxCreateString("KSOPT:internal");
		error_inputs[1] = objective_exception_internal;
		mexCallMATLAB(0, (mxArray **)NULL, 2, error_inputs, "error");
		return;
	}
	if (constraint_exception_internal != NULL) {
		error_inputs[0] = mxCreateString("KSOPT:internal");
		error_inputs[1] = constraint_exception_internal;
		mexCallMATLAB(0, (mxArray **)NULL, 2, error_inputs, "error");
		return;
	}
	if (iter_exception_internal != NULL) {
		error_inputs[0] = mxCreateString("KSOPT:internal");
		error_inputs[1] = iter_exception_internal;
		mexCallMATLAB(0, (mxArray **)NULL, 2, error_inputs, "error");
		return;
	}
#ifdef __cplusplus
}
catch (std::exception ex) {
	mexErrMsgIdAndTxt("KSOPT:input", "An internal error occured while calling KSOPT.");
}
catch (...) {
	mexErrMsgIdAndTxt("KSOPT:input", "An unknown internal error occured while calling KSOPT.");
}
#endif
}


#define FREE_CALLMATLABPLHSOBJECTIVE(size)\
	for (kk = 0; kk < size; ++kk) {\
		if (objective_function_info.plhs != NULL && objective_function_info.plhs[kk] != NULL) {\
			mxDestroyArray(objective_function_info.plhs[kk]);\
			objective_function_info.plhs[kk] = NULL;\
		}\
	}
#define FREE_CALLMATLABPLHSCONSTRAINT(size)\
	for (kk = 0; kk < size; ++kk) {\
		if (constraint_function_info.plhs != NULL && constraint_function_info.plhs[kk] != NULL) {\
			mxDestroyArray(constraint_function_info.plhs[kk]);\
			constraint_function_info.plhs[kk] = NULL;\
		}\
	}
static int objective_function_wrapper(const int ndv, const int nobj, const int ncon, const double* x, double* obj, double* g) {
	/*GRADIENT_FUNCTION_WRAPPER wrapper for call to gradient function from within KSOPT
	*	Input:
	*		ndv:							number of optimization variables
	*		nobj:							number of objectives
	*		ncon:							number of constraints
	*		x:								current optimization value
	*	Output:
	*		obj:							current objective function value
	*		g:								current constraint function value
	*		exitstatus:						zero in case of success else nonzero
	*/
	double accum = 0.0;
	double *objective = NULL;
	double *constraint = NULL;
	double *constraint_gradient_ineq = NULL;
	double *constraint_gradient_eq = NULL;
	mxArray *exception = NULL;
	boolean_T needsgradient = false;
	void *memoryptr = NULL;
	unsigned int ii = 0, jj = 0, kk = 0, constraint_position = 0;
	unsigned int number_variables = 0, number_objectives = 0, number_constraints = 0;
	double *xptr = NULL;
	char *error_msg_buffer = NULL;
	size_t error_msg_buffersize = 0;
	//Check for Ctrl-C
	if (utIsInterruptPending()) {
		utSetInterruptPending(false); /* clear Ctrl-C status */
		mexPrintf("\nCtrl-C Detected. Exiting KSOPT...\n\n");
		return KSOPT_USERINTERRUPT;
	}
	if (ndv <= 0) {
		objective_exception_internal = mxCreateString("Number of optimization variables must be positive.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	if (nobj <= 0) {
		objective_exception_internal = mxCreateString("Number of Objectives must be positive.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	if (nobj != objective_function_info.info->number_objectives) {
		objective_exception_internal = mxCreateString("Number of optimization variables changed during optimization.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	if (ncon < 0) {
		constraint_exception_internal = mxCreateString("Number of constraints must be positive.");
		return KSOPT_INTERNAL_CONSTRAINTERROR;
	}
	if (ncon != constraint_function_info.info->number_constraints_ineq + 2*constraint_function_info.info->number_constraints_eq + constraint_function_info.number_inequality_linear + 2*constraint_function_info.number_equality_linear) {
		constraint_exception_internal = mxCreateString("Number of constraints changed during optimization.");
		return KSOPT_INTERNAL_CONSTRAINTERROR;
	}
	if (X == NULL || OBJ == NULL) {
		objective_exception_internal = mxCreateString("Memory for cache variables was not allocated.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	if (objective_function_info.xrhs < 0 || objective_function_info.xrhs >= MAXRHS || objective_function_info.prhs[objective_function_info.xrhs] == NULL) {
		objective_exception_internal = mxCreateString("Objective function interface is corrupted.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	number_variables = (unsigned int)max(0, ndv);
	number_objectives = (unsigned int)max(0, nobj);
	number_constraints = (unsigned int)max(0, ncon);

	needsgradient = GRAD_OBJ != NULL && objective_function_info.info->hasgradient;
	needsgradient = needsgradient && (ncon <= 0 || (GRAD_CONSTR != NULL && ((constraint_function_info.info->number_constraints_eq <= 0 && constraint_function_info.info->number_constraints_ineq <= 0) || (constraint_function_info.info->hasgradient))));
	// get objective function value
	if (objective_function_info.info->number_outputs <= 0) {
		objective_exception_internal = mxCreateString("Number of output arguments of objective function must not be negative.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	//Set x
	objective_function_info.plhs[0] = NULL;
	objective_function_info.plhs[1] = NULL;
	xptr = mxGetPr(objective_function_info.prhs[objective_function_info.xrhs]);
	if (xptr == NULL) {
		objective_exception_internal = mxCreateString("Out of Memory.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	memoryptr = memcpy(xptr, x, number_variables*sizeof(double));
	if (memoryptr == NULL) {
		objective_exception_internal = mxCreateString("Out of Memory.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	memoryptr = memcpy(X, x, number_variables*sizeof(double));
	if (memoryptr == NULL) {
		objective_exception_internal = mxCreateString("Out of Memory.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	//Call MATLAB
	if (needsgradient) {
		exception = mexCallMATLABWithTrap(2, objective_function_info.plhs, objective_function_info.nrhs, objective_function_info.prhs, objective_function_info.f);
	}
	else {
		exception = mexCallMATLABWithTrap(1, objective_function_info.plhs, objective_function_info.nrhs, objective_function_info.prhs, objective_function_info.f);
	}
	if (exception != NULL) {
		objective_exception = exception;
		return KSOPT_MATLAB_OBJECTIVEERROR;
	}
	if (objective_function_info.plhs[0] == NULL) {
		objective_exception_internal = mxCreateString("Invalid return value for objective function call.");
		return KSOPT_MATLAB_OBJECTIVEERROR;
	}
	if (mxIsClass(objective_function_info.plhs[0], "unknown")) {
		objective_exception_internal = mxCreateString("Unknown return value for objective function call.");
		return KSOPT_MATLAB_OBJECTIVEERROR;
	}
	if (mxGetM(objective_function_info.plhs[0]) != number_objectives || mxGetN(objective_function_info.plhs[0]) != 1) {
		error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Objective function must return a row vector with %d elements.", number_objectives);
		if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
			objective_exception_internal = mxCreateString("Out of Memory while checking objective gradient.");
			return KSOPT_MATLAB_OBJECTIVEERROR;
		}
		objective_exception_internal = mxCreateString(error_msg_buffer);
		mxFree(error_msg_buffer);
		error_msg_buffer = NULL;
		return KSOPT_MATLAB_OBJECTIVEERROR;
	}
	//Get Objective
	if (mxIsSparse(objective_function_info.plhs[0])) {
		int success = -1;
		xptr = (double*) mxCalloc(number_objectives, sizeof(double));
		success = ksopt_sparse2full(objective_function_info.plhs[0], xptr, number_objectives, 1, false);
		if (success != 0) {
			if (objective_function_info.plhs[1] != NULL) {
				mxDestroyArray(objective_function_info.plhs[1]);
				objective_function_info.plhs[1] = NULL;
			}
			objective_exception_internal = mxCreateString("Could not convert sparse objective value to full.");
			return KSOPT_MATLAB_OBJECTIVEERROR;
		}
		memoryptr = memcpy(obj, xptr, number_objectives*sizeof(double));
		// Clean up Ptr
		if (objective_function_info.plhs[0] != NULL) {
			mxDestroyArray(objective_function_info.plhs[0]);
			objective_function_info.plhs[0] = NULL;
		}
		if (memoryptr == NULL) {
			if (objective_function_info.plhs[1] != NULL) {
				mxDestroyArray(objective_function_info.plhs[1]);
				objective_function_info.plhs[1] = NULL;
			}
			objective_exception_internal = mxCreateString("Out of Memory.");
			return KSOPT_INTERNAL_OBJECTIVEERROR;
		}
		if (xptr != NULL) {
			mxFree(xptr);
		}
		xptr = NULL;
	}
	else {
		xptr = mxGetPr(objective_function_info.plhs[0]);
		if (xptr == NULL) {
			if (objective_function_info.plhs[1] != NULL) {
				mxDestroyArray(objective_function_info.plhs[1]);
				objective_function_info.plhs[1] = NULL;
			}
			objective_exception_internal = mxCreateString("Out of Memory.");
			return KSOPT_MATLAB_OBJECTIVEERROR;
		}
		memoryptr = memcpy(obj, xptr, number_objectives*sizeof(double));
		// Clean up Ptr
		if (objective_function_info.plhs[0] != NULL) {
			mxDestroyArray(objective_function_info.plhs[0]);
			objective_function_info.plhs[0] = NULL;
		}
		if (memoryptr == NULL) {
			if (objective_function_info.plhs[1] != NULL) {
				mxDestroyArray(objective_function_info.plhs[1]);
				objective_function_info.plhs[1] = NULL;
			}
			objective_exception_internal = mxCreateString("Out of Memory.");
			return KSOPT_INTERNAL_OBJECTIVEERROR;
		}
		xptr = NULL;
	}
	memoryptr = memcpy(OBJ, obj, number_objectives*sizeof(double));
	if (memoryptr == NULL) {
		objective_exception_internal = mxCreateString("Out of Memory.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	if (needsgradient) {
		boolean_T freexptr = false;
		if (needsgradient && GRAD_OBJ == NULL) {
			objective_exception_internal = mxCreateString("Memory for cache variables was not allocated.");
			return KSOPT_INTERNAL_OBJECTIVEERROR;
		}
		if (objective_function_info.plhs[1] == NULL) {
			objective_exception_internal = mxCreateString("Invalid return value for objective function call.");
			return KSOPT_MATLAB_OBJECTIVEERROR;
		}
		if (mxIsClass(objective_function_info.plhs[1], "unknown")) {
			objective_exception_internal = mxCreateString("Unknown return value for objective function call.");
			return KSOPT_MATLAB_OBJECTIVEERROR;
		}
		if (mxGetN(objective_function_info.plhs[1]) != number_objectives || mxGetM(objective_function_info.plhs[1]) != number_variables) {
			error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Objective function gradient must return a matrix with %d rows and %d columns.", number_objectives, number_variables);
			if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
				objective_exception_internal = mxCreateString("Out of Memory while checking objective gradient.");
				return KSOPT_MATLAB_OBJECTIVEERROR;
			}
			objective_exception_internal = mxCreateString(error_msg_buffer);
			mxFree(error_msg_buffer);
			error_msg_buffer = NULL;
			return KSOPT_MATLAB_OBJECTIVEERROR;
		}
		//Get Objective Gradient
		if (mxIsSparse(objective_function_info.plhs[1])) {
			int success = -1;
			xptr = (double*) mxCalloc(number_objectives*number_variables, sizeof(double));
			freexptr = true;
			success = ksopt_sparse2full(objective_function_info.plhs[1], xptr, number_objectives, number_variables, false);
			if (success != 0) {
				if (objective_function_info.plhs[1] != NULL) {
					mxDestroyArray(objective_function_info.plhs[1]);
					objective_function_info.plhs[1] = NULL;
				}
				objective_exception_internal = mxCreateString("Could not convert sparse objective value to full.");
				return KSOPT_MATLAB_OBJECTIVEERROR;
			}
		}
		else {
			xptr = mxGetPr(objective_function_info.plhs[1]);
			if (xptr == NULL) {
				if (objective_function_info.plhs[1] != NULL) {
					mxDestroyArray(objective_function_info.plhs[1]);
					objective_function_info.plhs[1] = NULL;
				}
				objective_exception_internal = mxCreateString("Out of Memory.");
				return KSOPT_MATLAB_OBJECTIVEERROR;
			}
		}
		for (ii = 0; ii < number_variables; ++ii) {
			for (jj = 0; jj < number_objectives; ++jj) {
				GRAD_OBJ[ii*number_objectives + jj] = xptr[ii + number_variables*jj];
			}
		}
		if (freexptr && xptr != NULL) {
			mxFree(xptr);
		}
		xptr = NULL;
		// Clean up Ptr
		if (objective_function_info.plhs[1] != NULL) {
			mxDestroyArray(objective_function_info.plhs[1]);
			objective_function_info.plhs[1] = NULL;
		}
	}
	if (number_constraints > 0) {
		if (X == NULL || CONSTR == NULL) {
			constraint_exception_internal = mxCreateString("Memory for cache variables was not allocated.");
			return KSOPT_INTERNAL_CONSTRAINTERROR;
		}
		if (needsgradient && GRAD_CONSTR == NULL) {
			constraint_exception_internal = mxCreateString("Memory for cache variables was not allocated.");
			return KSOPT_INTERNAL_CONSTRAINTERROR;
		}
		// get constraint values
		constraint_position = 0;
		if (constraint_function_info.f != NULL && strcmp(constraint_function_info.f, "") != 0 && (constraint_function_info.info->number_constraints_eq > 0 || constraint_function_info.info->number_constraints_ineq > 0)) {
			boolean_T freeconstraintptr = false, freeconstrainteqptr = false, freeconstraintgradientptr = false, freeconstrainteqgradientptr = false;
			if (constraint_function_info.info->number_outputs <= 0) {
				return KSOPT_INTERNAL_CONSTRAINTERROR;
			}
			if (constraint_function_info.xrhs < 0 || constraint_function_info.xrhs >= MAXRHS || constraint_function_info.prhs[constraint_function_info.xrhs] == NULL) {
				constraint_exception_internal = mxCreateString("Constraint function interface is corrupted.");
				return KSOPT_INTERNAL_CONSTRAINTERROR;
			}
			//Set x
			constraint_function_info.plhs[0] = NULL;
			constraint_function_info.plhs[1] = NULL;
			constraint_function_info.plhs[2] = NULL;
			constraint_function_info.plhs[3] = NULL;
			xptr = mxGetPr(constraint_function_info.prhs[constraint_function_info.xrhs]);
			if (xptr == NULL) {
				constraint_exception_internal = mxCreateString("Out of Memory.");
				return KSOPT_INTERNAL_CONSTRAINTERROR;
			}
			memoryptr = memcpy(xptr, x, number_variables*sizeof(double));
			if (memoryptr == NULL) {
				constraint_exception_internal = mxCreateString("Out of Memory.");
				return KSOPT_INTERNAL_CONSTRAINTERROR;
			}
			//Call MATLAB
			if (needsgradient) {
				exception = mexCallMATLABWithTrap(min(4, constraint_function_info.info->number_outputs), constraint_function_info.plhs, constraint_function_info.nrhs, constraint_function_info.prhs, constraint_function_info.f);
			}
			else {
				exception = mexCallMATLABWithTrap(min(2, constraint_function_info.info->number_outputs), constraint_function_info.plhs, constraint_function_info.nrhs, constraint_function_info.prhs, constraint_function_info.f);
			}
			if (exception != NULL) {
				constraint_exception = exception;
				return KSOPT_MATLAB_CONSTRAINTERROR;
			}
			if (constraint_function_info.plhs[0] == NULL || mxIsClass(constraint_function_info.plhs[0], "unknown")) {
				FREE_CALLMATLABPLHSCONSTRAINT(4)
				constraint_exception_internal = mxCreateString("Unknown return value for constraint function call.");
				return KSOPT_MATLAB_CONSTRAINTERROR;
			}
			if (constraint_function_info.plhs[1] == NULL || mxIsClass(constraint_function_info.plhs[1], "unknown")) {
				FREE_CALLMATLABPLHSCONSTRAINT(4)
				constraint_exception_internal = mxCreateString("Unknown return value for constraint function call.");
				return KSOPT_MATLAB_CONSTRAINTERROR;
			}
			if (needsgradient) {
				if (constraint_function_info.plhs[2] == NULL || mxIsClass(constraint_function_info.plhs[2], "unknown")) {
					constraint_exception_internal = mxCreateString("Unknown return value for constraint function call.");
					FREE_CALLMATLABPLHSCONSTRAINT(4)
					return KSOPT_MATLAB_CONSTRAINTERROR;
				}
				if (constraint_function_info.plhs[3] == NULL || mxIsClass(constraint_function_info.plhs[3], "unknown")) {
					constraint_exception_internal = mxCreateString("Unknown return value for constraint function call.");
					FREE_CALLMATLABPLHSCONSTRAINT(4)
					return KSOPT_MATLAB_CONSTRAINTERROR;
				}
				// TODO: is checked twice and can be deleted here
				if (constraint_function_info.info->number_constraints_ineq > 0) {
					if (mxGetN(constraint_function_info.plhs[2]) != constraint_function_info.info->number_constraints_ineq || mxGetM(constraint_function_info.plhs[2]) != number_variables) {
						error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return a matrix with %d rows and %d columns for inequality constraint gradient.", constraint_function_info.info->number_constraints_ineq, number_variables);
						if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
							constraint_exception_internal = mxCreateString("Out of Memory while checking inequality constraint gradient.");
							FREE_CALLMATLABPLHSCONSTRAINT(4)
							return KSOPT_MATLAB_CONSTRAINTERROR;
						}
						constraint_exception_internal = mxCreateString(error_msg_buffer);
						mxFree(error_msg_buffer);
						error_msg_buffer = NULL;
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
				}
				if (constraint_function_info.info->number_constraints_ineq <= 0 && !mxIsEmpty(constraint_function_info.plhs[2])) {
					error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return an empty matrix for inequality constraint gradient, not a %dX%d matrix.", mxGetM(constraint_function_info.plhs[2]), mxGetN(constraint_function_info.plhs[2]));
					if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
						constraint_exception_internal = mxCreateString("Out of Memory while checking inequality constraint gradient.");
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
					constraint_exception_internal = mxCreateString(error_msg_buffer);
					mxFree(error_msg_buffer);
					error_msg_buffer = NULL;
					FREE_CALLMATLABPLHSCONSTRAINT(4)
					return KSOPT_MATLAB_CONSTRAINTERROR;
				}
				if (constraint_function_info.info->number_constraints_eq > 0) {
					if (mxGetN(constraint_function_info.plhs[3]) != constraint_function_info.info->number_constraints_eq || mxGetM(constraint_function_info.plhs[3]) != number_variables) {
						error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return a matrix with %d rows and %d columns for equality constraint gradient.", constraint_function_info.info->number_constraints_ineq, number_variables);
						if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
							constraint_exception_internal = mxCreateString("Out of Memory while checking equality constraint gradient.");
							FREE_CALLMATLABPLHSCONSTRAINT(4)
							return KSOPT_MATLAB_CONSTRAINTERROR;
						}
						constraint_exception_internal = mxCreateString(error_msg_buffer);
						mxFree(error_msg_buffer);
						error_msg_buffer = NULL;
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
				}
				if (constraint_function_info.info->number_constraints_eq <= 0 && !mxIsEmpty(constraint_function_info.plhs[3])) {
					error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return an empty matrix for equality constraint gradient, not a %dX%d matrix.", mxGetM(constraint_function_info.plhs[3]), mxGetN(constraint_function_info.plhs[3]));
					if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
						constraint_exception_internal = mxCreateString("Out of Memory while checking equality constraint gradient.");
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
					constraint_exception_internal = mxCreateString(error_msg_buffer);
					mxFree(error_msg_buffer);
					error_msg_buffer = NULL;
					FREE_CALLMATLABPLHSCONSTRAINT(4)
					return KSOPT_MATLAB_CONSTRAINTERROR;
				}
			}
			if (!mxIsEmpty(constraint_function_info.plhs[0])) {
				if (mxIsSparse(constraint_function_info.plhs[0])) {
					constraint = (double*) mxCalloc(constraint_function_info.info->number_constraints_ineq, sizeof(double));
					freeconstraintptr = true;
				}
				else {
					constraint = mxGetPr(constraint_function_info.plhs[0]);
				}
			}
			else {
				constraint = NULL;
			}
			if (!mxIsEmpty(constraint_function_info.plhs[1])) {
				if (mxIsSparse(constraint_function_info.plhs[1])) {
					objective = (double*) mxCalloc(constraint_function_info.info->number_constraints_eq, sizeof(double));
					freeconstrainteqptr = true;
				}
				else {
					objective = mxGetPr(constraint_function_info.plhs[1]);
				}
			}
			else {
				objective = NULL;
			}
			if (needsgradient) {
				if (!mxIsEmpty(constraint_function_info.plhs[2])) {
					if (mxIsSparse(constraint_function_info.plhs[2])) {
						constraint_gradient_ineq = (double*) mxCalloc(constraint_function_info.info->number_constraints_ineq*number_variables, sizeof(double));
						freeconstraintgradientptr = true;
					}
					else {
						constraint_gradient_ineq = mxGetPr(constraint_function_info.plhs[2]);
					}
				}
				else {
					constraint_gradient_ineq = NULL;
				}
				if (!mxIsEmpty(constraint_function_info.plhs[3])) {
					if (mxIsSparse(constraint_function_info.plhs[3])) {
						constraint_gradient_eq = (double*) mxCalloc(constraint_function_info.info->number_constraints_eq*number_variables, sizeof(double));
						freeconstrainteqgradientptr = true;
					}
					else {
						constraint_gradient_eq = mxGetPr(constraint_function_info.plhs[3]);
					}
				}
				else {
					constraint_gradient_eq = NULL;
				}
			}
			else {
				constraint_gradient_ineq = NULL;
				constraint_gradient_eq = NULL;
			}
			if (constraint_function_info.info->number_constraints_ineq > 0) {
				int success = -1;
				if (mxGetM(constraint_function_info.plhs[0]) != constraint_function_info.info->number_constraints_ineq || mxGetN(constraint_function_info.plhs[0]) != 1) {
					error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return a row vector with %d elements for inequality constraints.", constraint_function_info.info->number_constraints_ineq);
					if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
						constraint_exception_internal = mxCreateString("Out of Memory while checking inequality constraints.");
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
					constraint_exception_internal = mxCreateString(error_msg_buffer);
					mxFree(error_msg_buffer);
					error_msg_buffer = NULL;
					FREE_CALLMATLABPLHSCONSTRAINT(4)
					return KSOPT_MATLAB_CONSTRAINTERROR;
				}
				if (mxIsSparse(constraint_function_info.plhs[0])) {
					success = ksopt_sparse2full(constraint_function_info.plhs[0], constraint, constraint_function_info.info->number_constraints_ineq, 1, false);
					if (success != 0) {
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						constraint_exception_internal = mxCreateString("Sparse inequality constraint matrix is invalid.");
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
				}
				if (needsgradient) {
					if (mxGetN(constraint_function_info.plhs[2]) != constraint_function_info.info->number_constraints_ineq || mxGetM(constraint_function_info.plhs[2]) != number_variables) {
						error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return a matrix with %d rows and %d columns for inequality constraint gradient.", constraint_function_info.info->number_constraints_ineq, number_variables);
						if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
							constraint_exception_internal = mxCreateString("Out of Memory while checking inequality constraint gradient.");
							FREE_CALLMATLABPLHSCONSTRAINT(4)
							return KSOPT_MATLAB_CONSTRAINTERROR;
						}
						constraint_exception_internal = mxCreateString(error_msg_buffer);
						mxFree(error_msg_buffer);
						error_msg_buffer = NULL;
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
					if (mxIsSparse(constraint_function_info.plhs[2])) {
						success = ksopt_sparse2full(constraint_function_info.plhs[2], constraint_gradient_ineq, constraint_function_info.info->number_constraints_ineq, number_variables, false);
						if (success != 0) {
							FREE_CALLMATLABPLHSCONSTRAINT(4)
							constraint_exception_internal = mxCreateString("Sparse inequality constraint gradient matrix is invalid.");
							return KSOPT_MATLAB_CONSTRAINTERROR;
						}
					}
				}
				// copy nonlinear inequality constraints
				for (ii = 0; ii < constraint_function_info.info->number_constraints_ineq; ++ii) {
					if (constraint_position >= number_constraints) {
						constraint_exception_internal = mxCreateString("Not enough memory to copy inequality constraints.");
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_INTERNAL_CONSTRAINTERROR;
					}
					g[constraint_position] = constraint[ii];
					if (needsgradient && constraint_gradient_ineq != NULL) {
						for (jj = 0; jj < number_variables; ++jj) {
							GRAD_CONSTR[constraint_position + jj*number_constraints] = constraint_gradient_ineq[ii*number_variables + jj];
						}
					}
					constraint_position++;
				}
			}
			else {
				if (!mxIsEmpty(constraint_function_info.plhs[0])) {
					error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return an empty matrix for inequality constraints, not a %dX%d matrix.", mxGetM(constraint_function_info.plhs[0]), mxGetN(constraint_function_info.plhs[0]));
					if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
						constraint_exception_internal = mxCreateString("Out of Memory while checking inequality constraint gradient.");
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
					constraint_exception_internal = mxCreateString(error_msg_buffer);
					mxFree(error_msg_buffer);
					error_msg_buffer = NULL;
					FREE_CALLMATLABPLHSCONSTRAINT(4)
					return KSOPT_MATLAB_CONSTRAINTERROR;
				}
				if (needsgradient && !mxIsEmpty(constraint_function_info.plhs[2])) {
					error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return an empty matrix for inequality constraint gradient, not a %dX%d matrix.", mxGetM(constraint_function_info.plhs[2]), mxGetN(constraint_function_info.plhs[2]));
					if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
						constraint_exception_internal = mxCreateString("Out of Memory while checking inequality constraint gradient.");
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
					constraint_exception_internal = mxCreateString(error_msg_buffer);
					mxFree(error_msg_buffer);
					error_msg_buffer = NULL;
					FREE_CALLMATLABPLHSCONSTRAINT(4)
					return KSOPT_MATLAB_CONSTRAINTERROR;
				}
			}
			if (constraint_function_info.info->number_constraints_eq > 0) {
				int success = -1;
				if (mxGetM(constraint_function_info.plhs[1]) != constraint_function_info.info->number_constraints_eq || mxGetN(constraint_function_info.plhs[1]) != 1) {
					error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return a row vector with %d elements for equality constraints.", constraint_function_info.info->number_constraints_eq);
					if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
						constraint_exception_internal = mxCreateString("Out of Memory while checking equality constraint gradient.");
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
					constraint_exception_internal = mxCreateString(error_msg_buffer);
					mxFree(error_msg_buffer);
					error_msg_buffer = NULL;
					FREE_CALLMATLABPLHSCONSTRAINT(4)
					return KSOPT_MATLAB_CONSTRAINTERROR;
				}
				if (mxIsSparse(constraint_function_info.plhs[1])) {
					success = ksopt_sparse2full(constraint_function_info.plhs[1], objective, constraint_function_info.info->number_constraints_eq, 1, false);
					if (success != 0) {
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						constraint_exception_internal = mxCreateString("Sparse equality constraint matrix is invalid.");
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
				}
				if (needsgradient) {
					if (mxGetN(constraint_function_info.plhs[3]) != constraint_function_info.info->number_constraints_eq || mxGetM(constraint_function_info.plhs[3]) != number_variables) {
						error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return a matrix with %d rows and %d columns for equality constraint gradient.", constraint_function_info.info->number_constraints_eq, number_variables);
						if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
							constraint_exception_internal = mxCreateString("Out of Memory while checking equality constraint gradient.");
							FREE_CALLMATLABPLHSCONSTRAINT(4)
							return KSOPT_MATLAB_CONSTRAINTERROR;
						}
						constraint_exception_internal = mxCreateString(error_msg_buffer);
						mxFree(error_msg_buffer);
						error_msg_buffer = NULL;
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
					if (mxIsSparse(constraint_function_info.plhs[3])) {
						success = ksopt_sparse2full(constraint_function_info.plhs[3], constraint_gradient_eq, constraint_function_info.info->number_constraints_eq, number_variables, false);
						if (success != 0) {
							FREE_CALLMATLABPLHSCONSTRAINT(4)
							constraint_exception_internal = mxCreateString("Sparse equality constraint gradient matrix is invalid.");
							return KSOPT_MATLAB_CONSTRAINTERROR;
						}
					}
				}
				// copy nonlinear equality constraints
				for (ii = 0; ii < constraint_function_info.info->number_constraints_eq; ++ii) {
					if (constraint_position >= number_constraints || constraint_position + (number_variables - 1)*number_constraints >= number_variables*number_constraints) {
						constraint_exception_internal = mxCreateString("Not enough memory to copy equality constraints.");
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_INTERNAL_CONSTRAINTERROR;
					}
					g[constraint_position] = objective[ii];
					if (needsgradient && constraint_gradient_eq != NULL) {
						for (jj = 0; jj < number_variables; ++jj) {
							GRAD_CONSTR[constraint_position + jj*number_constraints] = constraint_gradient_eq[ii*number_variables + jj];
						}
					}
					constraint_position++;
				}
				for (ii = 0; ii < constraint_function_info.info->number_constraints_eq; ++ii) {
					if (constraint_position >= number_constraints || constraint_position + (number_variables - 1)*number_constraints >= number_variables*number_constraints) {
						constraint_exception_internal = mxCreateString("Not enough memory to copy equality constraints.");
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_INTERNAL_CONSTRAINTERROR;
					}
					g[constraint_position] = -objective[ii];
					if (needsgradient && constraint_gradient_eq != NULL) {
						for (jj = 0; jj < number_variables; ++jj) {
							GRAD_CONSTR[constraint_position + jj*number_constraints] = -constraint_gradient_eq[ii*number_variables + jj];
						}
					}
					constraint_position++;
				}
			}
			else {
				if (!mxIsEmpty(constraint_function_info.plhs[1])) {
					error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return an empty matrix for equality constraints, not a %dX%d matrix", mxGetM(constraint_function_info.plhs[1]), mxGetN(constraint_function_info.plhs[1]));
					if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
						constraint_exception_internal = mxCreateString("Out of Memory while checking equality constraints.");
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
					constraint_exception_internal = mxCreateString(error_msg_buffer);
					mxFree(error_msg_buffer);
					error_msg_buffer = NULL;
					FREE_CALLMATLABPLHSCONSTRAINT(4)
					return KSOPT_MATLAB_CONSTRAINTERROR;
				}
				if (needsgradient && !mxIsEmpty(constraint_function_info.plhs[3])) {
					error_msg_buffer = print_formatted_error(&error_msg_buffersize, "Constraint function must return an empty matrix for equality constraint gradient, not a %dX%d matrix", mxGetM(constraint_function_info.plhs[3]), mxGetN(constraint_function_info.plhs[3]));
					if (error_msg_buffer == NULL || error_msg_buffersize <= 0) {
						constraint_exception_internal = mxCreateString("Out of Memory while checking equality constraint gradient.");
						FREE_CALLMATLABPLHSCONSTRAINT(4)
						return KSOPT_MATLAB_CONSTRAINTERROR;
					}
					constraint_exception_internal = mxCreateString(error_msg_buffer);
					mxFree(error_msg_buffer);
					error_msg_buffer = NULL;
					FREE_CALLMATLABPLHSCONSTRAINT(4)
					return KSOPT_MATLAB_CONSTRAINTERROR;
				}
			}
			// Clean up Ptr
			FREE_CALLMATLABPLHSCONSTRAINT(4)
			if (freeconstraintptr && constraint != NULL) {
				mxFree(constraint);
				constraint = NULL;
			}
			if (freeconstrainteqptr && objective != NULL) {
				mxFree(objective);
				objective = NULL;
			}
			if (freeconstraintgradientptr && constraint_gradient_ineq != NULL) {
				mxFree(constraint_gradient_ineq);
				constraint_gradient_ineq = NULL;
			}
			if (freeconstrainteqgradientptr && constraint_gradient_eq != NULL) {
				mxFree(constraint_gradient_eq);
				constraint_gradient_eq = NULL;
			}
		}
		// copy linear inequality constraints
		if (constraint_function_info.number_inequality_linear > 0) {
			if (A_ineq == NULL || b_ineq == NULL) {
				constraint_exception_internal = mxCreateString("Global variables for linear inequality constraints not created.");
				return KSOPT_INTERNAL_CONSTRAINTERROR;
			}
			for (ii = 0; ii < constraint_function_info.number_inequality_linear; ++ii) {
				accum = 0.0;
				if (constraint_position >= number_constraints || constraint_position + (number_variables - 1)*number_constraints >= number_variables*number_constraints) {
					constraint_exception_internal = mxCreateString("Not enough memory to copy linear inequality constraints.");
					return KSOPT_INTERNAL_CONSTRAINTERROR;
				}
				for (jj = 0; jj < number_variables; ++jj) {
					accum = accum + A_ineq[ii + jj*constraint_function_info.number_inequality_linear]*x[jj];
					if (needsgradient) {
						GRAD_CONSTR[constraint_position + jj*number_constraints] = A_ineq[ii + jj*constraint_function_info.number_inequality_linear];
					}
				}
				g[constraint_position] = accum - b_ineq[ii];
				constraint_position++;
			}
		}
		// copy linear equality constraints
		if (constraint_function_info.number_equality_linear > 0) {
			if (A_eq == NULL || b_eq == NULL) {
				constraint_exception_internal = mxCreateString("Global variables for linear equality constraints not created.");
				return KSOPT_INTERNAL_CONSTRAINTERROR;
			}
			for (ii = 0; ii < constraint_function_info.number_equality_linear; ++ii) {
				accum = 0.0;
				if (constraint_position >= number_constraints || constraint_position + (number_variables - 1)*number_constraints >= number_variables*number_constraints) {
					constraint_exception_internal = mxCreateString("Not enough memory to copy linear equality constraints.");
					return KSOPT_INTERNAL_CONSTRAINTERROR;
				}
				for (jj = 0; jj < number_variables; ++jj) {
					accum = accum + A_eq[ii + jj*constraint_function_info.number_equality_linear]*x[jj];
					if (needsgradient) {
						GRAD_CONSTR[constraint_position + jj*number_constraints] = A_eq[ii + jj*constraint_function_info.number_equality_linear];
					}
				}
				g[constraint_position] = accum - b_eq[ii];
				constraint_position++;
			}
			for (ii = 0; ii < constraint_function_info.number_equality_linear; ++ii) {
				accum = 0.0;
				// TODO: only calculate once
				if (constraint_position >= number_constraints || constraint_position + (number_variables - 1)*number_constraints >= number_variables*number_constraints) {
					constraint_exception_internal = mxCreateString("Not enough memory to copy linear equality constraints.");
					return KSOPT_INTERNAL_CONSTRAINTERROR;
				}
				for (jj = 0; jj < number_variables; ++jj) {
					accum = accum + A_eq[ii + jj*constraint_function_info.number_equality_linear]*x[jj];
					if (needsgradient) {
						GRAD_CONSTR[constraint_position + jj*number_constraints] = -A_eq[ii + jj*constraint_function_info.number_equality_linear];
					}
				}
				g[constraint_position] = -accum + b_eq[ii];
				constraint_position++;
			}
		}
		memoryptr = memcpy(CONSTR, g, number_constraints*sizeof(double));
		if (memoryptr == NULL) {
			objective_exception_internal = mxCreateString("Out of Memory.");
			return KSOPT_INTERNAL_CONSTRAINTERROR;
		}
	}
	FIRSTEVALUATION = false;
	return 0;
}

static int gradient_function_wrapper(const int ndv, const int nobj, const int ncon, const double* x, double* obj, double* g, double* df, double* dg) {
	/*GRADIENT_FUNCTION_WRAPPER wrapper for call to gradient function from within KSOPT
	*	Input:
	*		ndv:							number of optimization variables
	*		nobj:							number of objectives
	*		ncon:							number of constraints
	*		x:								current optimization value
	*	Output:
	*		obj:							current objective function value
	*		g:								current constraint function value
	*		df:								gradient of current objective function value
	*		dg:								gradient of current constraint function value
	*		exitstatus:						zero in case of success else nonzero
	*/
	boolean_T needsgradient = false;
	void *memoryptr = NULL;
	unsigned int number_variables, number_objectives, number_constraints;
	//Check for Ctrl-C
	if (utIsInterruptPending()) {
		utSetInterruptPending(false); /* clear Ctrl-C status */
		mexPrintf("\nCtrl-C Detected. Exiting KSOPT...\n\n");
		return KSOPT_USERINTERRUPT;
	}
	if (ndv <= 0) {
		objective_exception_internal = mxCreateString("Number of optimization variables must be positive.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	if (nobj <= 0) {
		objective_exception_internal = mxCreateString("Number of Objectives must be positive.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	if (nobj != objective_function_info.info->number_objectives) {
		objective_exception_internal = mxCreateString("Number of optimization variables changed during optimization.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	if (ncon < 0) {
		constraint_exception_internal = mxCreateString("Number of constraints must be positive.");
		return KSOPT_INTERNAL_CONSTRAINTERROR;
	}
	if (ncon != constraint_function_info.info->number_constraints_ineq + 2*constraint_function_info.info->number_constraints_eq + constraint_function_info.number_inequality_linear + 2*constraint_function_info.number_equality_linear) {
		objective_exception_internal = mxCreateString("Number of constraints changed during optimization.");
		return KSOPT_INTERNAL_CONSTRAINTERROR;
	}
	if (objective_function_info.info == NULL) {
		objective_exception_internal = mxCreateString("Internal optimization problem structure is corrupted.");
		return KSOPT_INTERNAL_OBJECTIVEERROR;
	}
	if (constraint_function_info.info == NULL) {
		constraint_exception_internal = mxCreateString("Internal optimization problem structure is corrupted.");
		return KSOPT_INTERNAL_CONSTRAINTERROR;
	}
	number_variables = (unsigned int)max(0, ndv);
	number_objectives = (unsigned int)max(0, nobj);
	number_constraints = (unsigned int)max(0, ncon);

	needsgradient = GRAD_OBJ != NULL && objective_function_info.info->hasgradient;
	needsgradient = needsgradient && (ncon <= 0 || (GRAD_CONSTR != NULL && ((constraint_function_info.info->number_constraints_eq <= 0 && constraint_function_info.info->number_constraints_ineq <= 0) || (constraint_function_info.info->hasgradient))));
	if (!FIRSTEVALUATION && checkIdentX(x, X, number_variables)) {
		FIRSTEVALUATION = false;
		memoryptr = memcpy(obj, OBJ, number_objectives*sizeof(double));
		if (memoryptr == NULL) {
			objective_exception_internal = mxCreateString("Out of Memory.");
			return KSOPT_INTERNAL_OBJECTIVEERROR;
		}
		if (number_constraints > 0) {
			memoryptr = memcpy(g, CONSTR, number_constraints*sizeof(double));
			if (memoryptr == NULL) {
				constraint_exception_internal = mxCreateString("Out of Memory.");
				return KSOPT_INTERNAL_CONSTRAINTERROR;
			}
		}
		if (needsgradient) {
			if (GRAD_OBJ == NULL) {
				objective_exception_internal = mxCreateString("Objective gradient was not initialized.");
				return KSOPT_INTERNAL_OBJECTIVEERROR;
			}
			memoryptr = memcpy(df, GRAD_OBJ, number_objectives*number_variables*sizeof(double));
			if (memoryptr == NULL) {
				objective_exception_internal = mxCreateString("Out of Memory.");
				return KSOPT_INTERNAL_OBJECTIVEERROR;
			}
			if (number_constraints > 0) {
				if (GRAD_CONSTR == NULL) {
					constraint_exception_internal = mxCreateString("Constraint gradient was not initialized.");
					return KSOPT_INTERNAL_CONSTRAINTERROR;
				}
				memoryptr = memcpy(dg, GRAD_CONSTR, number_constraints*number_variables*sizeof(double));
				if (memoryptr == NULL) {
					constraint_exception_internal = mxCreateString("Out of Memory.");
					return KSOPT_INTERNAL_CONSTRAINTERROR;
				}
			}
		}
	}
	else {
		if (objective_function_wrapper(number_variables, number_objectives, number_constraints, x, obj, g) == 0) {
			memoryptr = memcpy(obj, OBJ, number_objectives*sizeof(double));
			if (memoryptr == NULL) {
				objective_exception_internal = mxCreateString("Out of Memory.");
				return KSOPT_INTERNAL_OBJECTIVEERROR;
			}
			if (number_constraints > 0) {
				memoryptr = memcpy(g, CONSTR, number_constraints*sizeof(double));
				if (memoryptr == NULL) {
					constraint_exception_internal = mxCreateString("Out of Memory.");
					return KSOPT_INTERNAL_CONSTRAINTERROR;
				}
			}
			if (needsgradient) {
				if (GRAD_OBJ == NULL) {
					objective_exception_internal = mxCreateString("Objective gradient was not initialized.");
					return KSOPT_INTERNAL_OBJECTIVEERROR;
				}
				memoryptr = memcpy(df, GRAD_OBJ, number_objectives*number_variables*sizeof(double));
				if (memoryptr == NULL) {
					objective_exception_internal = mxCreateString("Out of Memory.");
					return KSOPT_INTERNAL_OBJECTIVEERROR;
				}
				if (number_constraints > 0) {
					if (GRAD_CONSTR == NULL) {
						constraint_exception_internal = mxCreateString("Constraint gradient was not initialized.");
						return KSOPT_INTERNAL_CONSTRAINTERROR;
					}
					memoryptr = memcpy(dg, GRAD_CONSTR, number_constraints*number_variables*sizeof(double));
					if (memoryptr == NULL) {
						constraint_exception_internal = mxCreateString("Out of Memory.");
						return KSOPT_INTERNAL_CONSTRAINTERROR;
					}
				}
			}
		}
		else {
			if (objective_exception_internal == NULL && constraint_exception_internal == NULL) {
				objective_exception_internal = mxCreateString("An unknown error occured while evaluating the objective function.");
			}
			return KSOPT_INTERNAL_OBJECTIVEERROR;
		}
	}
	return 0;
}

static int iteration_function_wrapper(const int ndv, const int nobj, const int ncon, const double* x, const double* obj, const double* g, const double* df, const double* dg, const KSOPTIter iteration_info, const KSOPT_ALGORITHM_STATE state, unsigned char* stop) {
	/*ITERATION_FUNCTION_WRAPPER wrapper for call to iteration function from within KSOPT
	*	Input:
	*		ndv:							number of optimization variables
	*		nobj:							number of objectives
	*		ncon:							number of constraints
	*		x:								current optimization value
	*		obj:							current objective function value
	*		g:								current constraint function value
	*		df:								gradient of current objective function value
	*		dg:								gradient of current constraint function value
	*		iteration_info:					structure with information about iteration function
	*		state:							current solution state of algorithm
	*	Output:
	*		stop:							indicator if optimization should be stopped
	*		exitstatus:						zero in case of success else nonzero
	*/
	mxArray *iter_fun_plhs[1] = {NULL};
	mxArray *iter_fun_prhs[4] = {NULL, NULL, NULL, NULL};
	mxArray *exception = NULL;
	boolean_T stopoptimization = false;
	mxLogical *logicalreturnvalue = NULL;
	mxArray *structfield = NULL;
	void *memoryptr = NULL;
	unsigned int ii = 0;
	unsigned int number_variables = 0, number_objectives = 0, number_constraints = 0;
	double *xptr = NULL;
	//Check for Ctrl-C
	if (utIsInterruptPending()) {
		utSetInterruptPending(false); /* clear Ctrl-C status */
		mexPrintf("\nCtrl-C Detected. Exiting KSOPT...\n\n");
		return KSOPT_USERINTERRUPT;
	}
	if (!iter_function.hasiterfun) {
		*stop = false;
		return 0;
	}
	if (ndv <= 0) {
		iter_exception_internal = mxCreateString("Number of optimization variables must be positive.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	if (nobj <= 0) {
		iter_exception_internal = mxCreateString("Number of Objectives must be positive.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	if (nobj != objective_function_info.info->number_objectives) {
		iter_exception_internal = mxCreateString("Number of optimization variables changed during optimization.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	if (ncon < 0) {
		iter_exception_internal = mxCreateString("Number of constraints must be positive.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	if (ncon != constraint_function_info.info->number_constraints_ineq + 2*constraint_function_info.info->number_constraints_eq + constraint_function_info.number_inequality_linear + 2*constraint_function_info.number_equality_linear) {
		iter_exception_internal = mxCreateString("Number of constraints changed during optimization.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	if (x == NULL || obj == NULL) {
		iter_exception_internal = mxCreateString("Memory for cache variables was not allocated.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	number_variables = (unsigned int)max(0, ndv);
	number_objectives = (unsigned int)max(0, nobj);
	number_constraints = (unsigned int)max(0, ncon);

	if (iter_function.x == NULL || mxGetM(iter_function.x) != number_variables) {
		iter_exception_internal = mxCreateString("Memory for cache variables was not allocated.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	if (iter_function.optimaloptimValues == NULL) {
		iter_exception_internal = mxCreateString("Memory for cache variables was not allocated.");
		return KSOPT_INTERNAL_ITERERROR;
	}

	//Set x
	iter_fun_plhs[0] = NULL;
	xptr = mxGetPr(iter_function.x);
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Out of Memory.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	memoryptr = memcpy(xptr, x, number_variables*sizeof(double));
	if (memoryptr == NULL) {
		iter_exception_internal = mxCreateString("Out of Memory.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	// Set state
	if (iter_function.state != NULL) {
		mxDestroyArray(iter_function.state);
		iter_function.state = NULL;
	}
	switch (state) {
		case KSOPT_ALGORITHM_STATE_INIT: {
			iter_function.state = mxCreateString(iterfunSTATES[KSOPT_ALGORITHM_STATE_INIT]);
			break;
		}
		case KSOPT_ALGORITHM_STATE_INTERRUPT: {
			iter_function.state = mxCreateString(iterfunSTATES[KSOPT_ALGORITHM_STATE_INTERRUPT]);
			break;
		}
		case KSOPT_ALGORITHM_STATE_ITER: {
			iter_function.state = mxCreateString(iterfunSTATES[KSOPT_ALGORITHM_STATE_ITER]);
			break;
		}
		case KSOPT_ALGORITHM_STATE_DONE: {
			iter_function.state = mxCreateString(iterfunSTATES[KSOPT_ALGORITHM_STATE_DONE]);
			break;
		}
		default: {
			iter_exception_internal = mxCreateString("Undefined iteration function state.");
			return KSOPT_INTERNAL_ITERERROR;
		}
	}
	if (iter_function.state == NULL) {
		iter_exception_internal = mxCreateString("Not enough memory to allocate iteration function state.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	// Set optimization statistics in structure argument
	// set function value
	structfield = mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERfval]);
	if (structfield == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	if (mxGetM(structfield) != number_objectives) {
		iter_exception_internal = mxCreateString("Memory for cache variables was not allocated.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	xptr = mxGetPr(structfield);
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	xptr = (double*) memcpy(xptr, obj, number_objectives*sizeof(double));
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Out of Memory.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	// set gradient value
	structfield = mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERgradient]);
	if (structfield == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	if (mxGetM(structfield) != number_objectives || mxGetN(structfield) != number_variables) {
		iter_exception_internal = mxCreateString("Memory for cache variables was not allocated.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	xptr = mxGetPr(structfield);
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	xptr = (double*) memcpy(xptr, df, number_objectives*number_variables*sizeof(double));
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Out of Memory.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	// set constraint violation
	structfield = mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERconstrviolation]);
	if (structfield == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	xptr = mxGetPr(structfield);
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	*xptr = iteration_info.constrviolation;
	// set degeneracy
	structfield = mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERdegenerate]);
	if (structfield == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	xptr = mxGetPr(structfield);
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	*xptr = iteration_info.degenerate;
	// set first order optimality
	structfield = mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERfirstorderopt]);
	if (structfield == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	xptr = mxGetPr(structfield);
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	*xptr = iteration_info.firstorderopt;
	// set function evaluation count
	structfield = mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERfunccount]);
	if (structfield == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	xptr = mxGetPr(structfield);
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	*xptr = (double) iteration_info.funccount;
	// set iteration count
	structfield = mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERiteration]);
	if (structfield == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	xptr = mxGetPr(structfield);
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	*xptr = (double) iteration_info.iteration;
	// set maximum function value
	structfield = mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERmaxfval]);
	if (structfield == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	xptr = mxGetPr(structfield);
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	*xptr = iteration_info.maxfval;
	// set stepsize
	structfield = mxGetField(iter_function.optimaloptimValues, 0, fieldnamesITER[fieldITERstepsize]);
	if (structfield == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	xptr = mxGetPr(structfield);
	if (xptr == NULL) {
		iter_exception_internal = mxCreateString("Iteration function argument is invalid.");
		return KSOPT_INTERNAL_ITERERROR;
	}
	*xptr = iteration_info.stepsize;

	// call iteration functions
	for (ii = 0; ii < mxGetM(iter_function.iter_functions); ++ii) {
		iter_fun_prhs[0] = mxGetCell(iter_function.iter_functions, ii);
		iter_fun_prhs[1] = iter_function.x;
		iter_fun_prhs[2] = iter_function.optimaloptimValues;
		iter_fun_prhs[3] = iter_function.state;
		//Call MATLAB
		exception = mexCallMATLABWithTrap(1, iter_fun_plhs, 4, iter_fun_prhs, "feval");
		if (exception != NULL) {
			iter_exception = exception;
			return KSOPT_MATLAB_ITERERROR;
		}
		if (iter_fun_plhs[0] == NULL) {
			iter_exception_internal = mxCreateString("Invalid return value for iteration function call.");
			return KSOPT_MATLAB_ITERERROR;
		}
		if (mxIsClass(iter_fun_plhs[0], "unknown")) {
			iter_exception_internal = mxCreateString("Unknown return value for iteration function call.");
			return KSOPT_MATLAB_ITERERROR;
		}
		if (!mxIsScalar(iter_fun_plhs[0])) {
			iter_exception_internal = mxCreateString("Iteration function must return a scalar value.");
			return KSOPT_MATLAB_ITERERROR;
		}
		if (!mxIsLogical(iter_fun_plhs[0])) {
			iter_exception_internal = mxCreateString("Iteration function must return a logical value.");
			return KSOPT_MATLAB_ITERERROR;
		}
		logicalreturnvalue = mxGetLogicals(iter_fun_plhs[0]);
		if (logicalreturnvalue == NULL) {
			iter_exception_internal = mxCreateString("Iteration function return value is invalid.");
			return KSOPT_MATLAB_ITERERROR;
		}
		stopoptimization = stopoptimization || logicalreturnvalue[0];
	}
	*stop = stopoptimization;
	return 0;
}

static void checkInputs(const mxArray *prhs[], const int nrhs, KSOPToptionType *options, KSOPT_functionHandleInformation *objectiveinformation, KSOPT_functionHandleInformation *constraintinformation, size_t *number_objectives, size_t *number_constraints_ineq, size_t *number_constraints_eq, size_t *number_constraints_linear_ineq, size_t *number_constraints_linear_eq, KSOPT_iterfunctionHandleInformation *iterinfoplot, KSOPT_iterfunctionHandleInformation *iterinfooutput) {
	/*CHECKINPUTS check input arguments supplied in matlab
	*	Input:
	*		prhs:							array with mxArrays supplied by user in matlab
	*		nrhs:							number of supplied arguments in matlab
	*	Output:
	*		options:						structure with options for optimization with KSOPT
	*		objectiveinformation:			structure with inforamtion about objective function
	*		constraintinformation:			structure with information about constraint function
	*		number_objectives:				number of objective functions
	*		number_constraints_ineq:		number of inequality constraints
	*		number_constraints_eq:			number of equality constraints
	*		number_constraints_linear_ineq:	number of linear inequality constraints
	*		number_constraints_linear_eq:	number of linear equality constraints
	*		iterinfoplot:					structure with information about iteration function
	*		iterinfooutputoptions:			structure with information about out�put function
	*/
	int success = 1;
	double* temp = NULL;
	int ii, number_fields;
	mwSize number_dimensions1 = 0, number_dimensions2 = 0;
	const mwSize* dimensions1 = NULL, *dimensions2 = NULL;
	mwIndex *numericSparseJC = NULL;
	size_t number_variables = 0;
	size_t number_ineq_lin = 0, number_eq_lin = 0;
	mxArray *field = NULL, *otherfield = NULL;
	double doubleoption = 0.0;
	int intoption = 0;
	char *charoption = NULL;
	void *voidptr = NULL;
	boolean_T booleanoption = false;
	boolean_T hasgradient = false;
	boolean_T objectivegradient = false, constraintgradient = false;
	KSOPT_functionHandleInformation objectiveinfo;
	KSOPT_functionHandleInformation constraintinfo;
	KSOPT_OPTIONTYPE_DEFAULT

	*number_objectives = 0;
	*number_constraints_ineq = 0;
	*number_constraints_eq = 0;
	*number_constraints_linear_ineq = 0;
	*number_constraints_linear_eq = 0;

	objectiveinfo.type = KSOPT_OBJECTIVE;
	objectiveinfo.hasgradient = false;
	objectiveinfo.wasevaluated = false;
	objectiveinfo.number_inputs = 0;
	objectiveinfo.number_outputs = 0;
	objectiveinfo.number_objectives = 0;
	objectiveinfo.number_constraints_ineq = 0;
	objectiveinfo.number_constraints_eq = 0;

	constraintinfo.type = KSOPT_CONSTRAINT;
	constraintinfo.hasgradient = false;
	constraintinfo.wasevaluated = false;
	constraintinfo.number_inputs = 0;
	constraintinfo.number_outputs = 0;
	constraintinfo.number_objectives = 0;
	constraintinfo.number_constraints_ineq = 0;
	constraintinfo.number_constraints_eq = 0;

	objectiveinformation->number_objectives = 0;
	objectiveinformation->hasgradient = false;
	objectiveinformation->number_constraints_ineq = 0;
	objectiveinformation->number_constraints_eq = 0;
	objectiveinformation->number_inputs = 0;
	objectiveinformation->number_outputs = 0;
	objectiveinformation->type = KSOPT_OBJECTIVE;
	objectiveinformation->wasevaluated = false;

	constraintinformation->number_objectives = 0;
	constraintinformation->hasgradient = false;
	constraintinformation->number_constraints_ineq = 0;
	constraintinformation->number_constraints_eq = 0;
	constraintinformation->number_inputs = 0;
	constraintinformation->number_outputs = 0;
	constraintinformation->type = KSOPT_CONSTRAINT;
	constraintinformation->wasevaluated = false;

	if (nrhs < argA) {
		mexErrMsgIdAndTxt("KSOPT:input", "You must supply at least 2 arguments to ksopt!\nksopt(fun, x0)\n");
		return;
	}

	// check objective function
	if (pargFUN == NULL || mxIsEmpty(pargFUN) || (!mxIsFunctionHandle(pargFUN) && !mxIsChar(pargFUN))) {
		mexErrMsgIdAndTxt("KSOPT:input", "Objective function must be a function handle or a name of a function.");
		return;
	}

	// check initial value
	if (pargX0 == NULL || (!mxIsDouble(pargX0) && !mxIsSparse(pargX0)) || mxIsComplex(pargX0) || mxIsEmpty(pargX0)) {
		//TODO: allow int, float, etc.
		mexErrMsgIdAndTxt("KSOPT:input", "Initial value must be a real double.");
		return;
	}
	number_dimensions1 = mxGetNumberOfDimensions(pargX0);
	dimensions1 = mxGetDimensions(pargX0);
	if (number_dimensions1 > 2) {
		// TODO: allow parallel execution for different initial values
		mexErrMsgIdAndTxt("KSOPT:input", "Initial value must be vector.");
		return;
	}
	if (dimensions1[0] <= 0 || dimensions1[1] != 1) {
		mexErrMsgIdAndTxt("KSOPT:input", "Initial value must be a column vector.");
		return;
	}
	if (mxIsSparse(pargX0)) {
		double *numericSparseDouble = NULL;
		numericSparseDouble = mxGetPr(pargX0);
		if (numericSparseDouble == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Numeric data for initial value is invalid.");
			return;
		}
		numericSparseJC = mxGetJc(pargX0);
		if (numericSparseJC == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Numeric data for initial value is invalid.");
			return;
		}
		for (ii = 0; ii < numericSparseJC[mxGetN(pargX0)]; ++ii) {
			if (!mxIsFinite(numericSparseDouble[ii]) || mxIsNaN(numericSparseDouble[ii])) {
				mexErrMsgIdAndTxt("KSOPT:input", "Initial value must be finite.");
				return;
			}
		}
		numericSparseDouble = NULL;
		numericSparseJC = NULL;
	}
	else {
		temp = mxGetPr(pargX0);
		for (ii = 0; ii < mxGetNumberOfElements(pargX0); ++ii) {
			if (!mxIsFinite(temp[ii]) || mxIsNaN(temp[ii])) {
				mexErrMsgIdAndTxt("KSOPT:input", "Initial value must be finite.");
				return;
			}
		}
		temp = NULL;
	}
	number_variables = dimensions1[0];

	success = ksopt_checkhandle(pargFUN, pargX0, KSOPT_OBJECTIVE, &objectiveinfo);
	if (success != 0) {
		mexErrMsgIdAndTxt("KSOPT:input", "Objective function is not valid.");
		return;
	}
	*number_objectives = objectiveinfo.number_objectives;

	// check A and b
	if (nrhs > argA) {
		if (pargA != NULL && !mxIsEmpty(pargA)) {
			if (nrhs < argB) {
				mexErrMsgIdAndTxt("KSOPT:input", "Missing input argument 'b'.");
				return;
			}
			if (pargB != NULL && !mxIsEmpty(pargB)) {
				if ((pargA == NULL) ^ (pargB == NULL)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality matrix and upper bound must be both empty.");
					return;
				}
				if (mxIsEmpty(pargA) ^ mxIsEmpty(pargB)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality matrix and upper bound must be both empty.");
					return;
				}
				if ((!mxIsDouble(pargA) && !mxIsSparse(pargA)) || mxIsComplex(pargA)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality matrix must be a real double.");
					return;
				}
				if ((!mxIsDouble(pargB) && !mxIsSparse(pargB)) || mxIsComplex(pargB)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality bound must be a real double.");
					return;
				}
				number_dimensions1 = mxGetNumberOfDimensions(pargA);
				dimensions1 = mxGetDimensions(pargA);
				number_dimensions2 = mxGetNumberOfDimensions(pargB);
				dimensions2 = mxGetDimensions(pargB);
				if (number_dimensions1 > 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality matrix must be a matrix.");
					return;
				}
				if (number_dimensions2 > 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality bound must be a vector.");
					return;
				}
				number_ineq_lin = dimensions1[0];
				if (dimensions1[1] != number_variables) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality matrix must have %d rows.", number_variables);
					return;
				}
				if (dimensions2[0] != number_ineq_lin || dimensions2[1] != 1) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality matrix and bound must have same number of rows.");
					return;
				}
				temp = mxGetPr(pargA);
				if (temp == NULL) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality matrix is invalid.");
					return;
				}
				if (mxIsSparse(pargA)) {
					numericSparseJC = mxGetJc(pargA);
					if (numericSparseJC == NULL) {
						mexErrMsgIdAndTxt("KSOPT:input", "Numeric data for linear inequality matrix is invalid.");
						return;
					}
					for (ii = 0; ii < numericSparseJC[mxGetN(pargA)]; ++ii) {
						if (!mxIsFinite(temp[ii]) || mxIsNaN(temp[ii])) {
							mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality matrix must be finite.");
							return;
						}
					}
					numericSparseJC = NULL;
				}
				else {
					for (ii = 0; ii < mxGetNumberOfElements(pargA); ++ii) {
						if (!mxIsFinite(temp[ii]) || mxIsNaN(temp[ii])) {
							mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality matrix must be finite.");
							return;
						}
					}
				}
				temp = NULL;
				temp = mxGetPr(pargB);
				if (temp == NULL) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality upper bound is invalid.");
					return;
				}
				if (mxIsSparse(pargB)) {
					numericSparseJC = mxGetJc(pargB);
					if (numericSparseJC == NULL) {
						mexErrMsgIdAndTxt("KSOPT:input", "Numeric data for linear inequality bound is invalid.");
						return;
					}
					for (ii = 0; ii < numericSparseJC[mxGetN(pargB)]; ++ii) {
						if (!mxIsFinite(temp[ii]) || mxIsNaN(temp[ii])) {
							mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality bound must be finite.");
							return;
						}
					}
					numericSparseJC = NULL;
				}
				else {
					for (ii = 0; ii < mxGetNumberOfElements(pargB); ++ii) {
						if (!mxIsFinite(temp[ii]) || mxIsNaN(temp[ii])) {
							mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality bound must be finite.");
							return;
						}
					}
				}
				temp = NULL;
				*number_constraints_linear_ineq = number_ineq_lin;
				A_ineq = (double*) mxCalloc(number_ineq_lin*number_variables, sizeof(double));
				if (A_ineq == NULL) {
					mexErrMsgIdAndTxt("KSOPT:input", "Out of Memory.");
					return;
				}
				if (mxIsSparse(pargA)) {
					int success = ksopt_sparse2full(pargA, A_ineq, number_ineq_lin, number_variables, true);
					if (success != 0) {
						mexErrMsgIdAndTxt("KSOPT:input", "Out of Memory.");
						return;
					}
				}
				else {
					temp = mxGetPr(pargA);
					for (ii = 0; ii < number_ineq_lin*number_variables; ++ii) {
						A_ineq[ii] = (double) temp[ii];
					}
					temp = NULL;
				}
				b_ineq = (double*) mxCalloc(number_ineq_lin, sizeof(double));
				if (b_ineq == NULL) {
					mexErrMsgIdAndTxt("KSOPT:input", "Out of Memory.");
					return;
				}
				if (mxIsSparse(pargB)) {
					int success = ksopt_sparse2full(pargB, b_ineq, number_ineq_lin, 1, true);
					if (success != 0) {
						mexErrMsgIdAndTxt("KSOPT:input", "Out of Memory.");
						return;
					}
				}
				else {
					temp = mxGetPr(pargB);
					for (ii = 0; ii < number_ineq_lin; ++ii) {
						b_ineq[ii] = (double) temp[ii];
					}
					temp = NULL;
				}
			}
			else {
				mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality bound must not be empty.");
				return;
			}
		}
		else {
			if (nrhs > argB) {
				if (pargB != NULL && !mxIsEmpty(pargB)) {
					if ((pargA == NULL) ^ (pargB == NULL)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality matrix and upper bound must be both empty.");
						return;
					}
					if (mxIsEmpty(pargA) ^ mxIsEmpty(pargB)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Linear inequality matrix and upper bound must be both empty.");
						return;
					}
				}
			}
		}
	}

	// check Aeq and beq
	if (nrhs > argAEQ) {
		if (pargAEQ != NULL && !mxIsEmpty(pargAEQ)) {
			if (nrhs < argBEQ) {
				mexErrMsgIdAndTxt("KSOPT:input", "Missing input argument 'beq'.");
				return;
			}
			if (pargBEQ != NULL && !mxIsEmpty(pargBEQ)) {
				if ((pargAEQ == NULL) ^ (pargBEQ == NULL)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix and upper bound must be both empty.");
					return;
				}
				if (mxIsEmpty(pargAEQ) ^ mxIsEmpty(pargBEQ)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix and upper bound must be both empty.");
					return;
				}
				if ((!mxIsDouble(pargAEQ) && !mxIsSparse(pargAEQ)) || mxIsComplex(pargAEQ)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix must be a real double.");
					return;
				}
				if ((!mxIsDouble(pargBEQ) && (mxIsSparse(pargBEQ))) || mxIsComplex(pargBEQ)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear equality bound must be a real double.");
					return;
				}
				number_dimensions1 = mxGetNumberOfDimensions(pargAEQ);
				dimensions1 = mxGetDimensions(pargAEQ);
				number_dimensions2 = mxGetNumberOfDimensions(pargBEQ);
				dimensions2 = mxGetDimensions(pargBEQ);
				if (number_dimensions1 > 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix must be a matrix.");
					return;
				}
				if (number_dimensions2 > 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear equality bound must be a vector.");
					return;
				}
				number_eq_lin = dimensions1[0];
				if (dimensions1[1] != number_variables) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix must have %d rows.", number_variables);
					return;
				}
				if (dimensions2[0] != number_eq_lin || dimensions2[1] != 1) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix and bound must have same number of rows.");
					return;
				}
				temp = mxGetPr(pargAEQ);
				if (temp == NULL) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix is invalid.");
					return;
				}
				if (mxIsSparse(pargAEQ)) {
					numericSparseJC = mxGetJc(pargAEQ);
					if (numericSparseJC == NULL) {
						mexErrMsgIdAndTxt("KSOPT:input", "Numeric data for linear equality matrix is invalid.");
						return;
					}
					for (ii = 0; ii < numericSparseJC[mxGetN(pargAEQ)]; ++ii) {
						if (!mxIsFinite(temp[ii]) || mxIsNaN(temp[ii])) {
							mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix must be finite.");
							return;
						}
					}
					numericSparseJC = NULL;
				}
				else {
					for (ii = 0; ii < mxGetNumberOfElements(pargAEQ); ++ii) {
						if (!mxIsFinite(temp[ii]) || mxIsNaN(temp[ii])) {
							mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix must be finite.");
							return;
						}
					}
				}
				temp = NULL;
				temp = mxGetPr(pargBEQ);
				if (temp == NULL) {
					mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix upper bound is invalid.");
					return;
				}
				if (mxIsSparse(pargBEQ)) {
					numericSparseJC = mxGetJc(pargBEQ);
					if (numericSparseJC == NULL) {
						mexErrMsgIdAndTxt("KSOPT:input", "Numeric data for linear equality bound is invalid.");
						return;
					}
					for (ii = 0; ii < numericSparseJC[mxGetN(pargBEQ)]; ++ii) {
						if (!mxIsFinite(temp[ii]) || mxIsNaN(temp[ii])) {
							mexErrMsgIdAndTxt("KSOPT:input", "Linear equality bound must be finite.");
							return;
						}
					}
					numericSparseJC = NULL;
				}
				else {
					for (ii = 0; ii < mxGetNumberOfElements(pargBEQ); ++ii) {
						if (!mxIsFinite(temp[ii]) || mxIsNaN(temp[ii])) {
							mexErrMsgIdAndTxt("KSOPT:input", "Linear equality bound must be finite.");
							return;
						}
					}
				}
				temp = NULL;
				*number_constraints_linear_eq = number_eq_lin;
				A_eq = (double*) mxCalloc(number_eq_lin*number_variables, sizeof(double));
				if (A_eq == NULL) {
					mexErrMsgIdAndTxt("KSOPT:input", "Out of Memory.");
					return;
				}
				if (mxIsSparse(pargAEQ)) {
					int success = ksopt_sparse2full(pargAEQ, A_eq, number_eq_lin, number_variables, true);
					if (success != 0) {
						mexErrMsgIdAndTxt("KSOPT:input", "Out of Memory.");
						return;
					}
				}
				else {
					temp = mxGetPr(pargAEQ);
					for (ii = 0; ii < number_eq_lin*number_variables; ++ii) {
						A_eq[ii] = (double) temp[ii];
					}
					temp = NULL;
				}
				b_eq = (double*) mxCalloc(number_eq_lin, sizeof(double));
				if (b_eq == NULL) {
					mexErrMsgIdAndTxt("KSOPT:input", "Out of Memory.");
					return;
				}
				if (mxIsSparse(pargBEQ)) {
					int success = ksopt_sparse2full(pargBEQ, b_eq, number_eq_lin, 1, true);
					if (success != 0) {
						mexErrMsgIdAndTxt("KSOPT:input", "Out of Memory.");
						return;
					}
				}
				else {
					temp = mxGetPr(pargBEQ);
					for (ii = 0; ii < number_eq_lin; ++ii) {
						b_eq[ii] = (double) temp[ii];
					}
					temp = NULL;
				}
			}
			else {
				mexErrMsgIdAndTxt("KSOPT:input", "Linear equality bound must not be empty.");
				return;
			}
		}
		else {
			if (nrhs > argBEQ) {
				if (pargBEQ != NULL && !mxIsEmpty(pargBEQ)) {
					if ((pargAEQ == NULL) ^ (pargBEQ == NULL)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix and upper bound must be both empty.");
						return;
					}
					if (mxIsEmpty(pargAEQ) ^ mxIsEmpty(pargBEQ)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Linear equality matrix and upper bound must be both empty.");
						return;
					}
				}
			}
		}
	}
	if (nrhs > argLB) {
		if (pargLB != NULL && !mxIsEmpty(pargLB)) {
			if (nrhs < argUB) {
				mexErrMsgIdAndTxt("KSOPT:input", "Missing input argument 'ub'.");
				return;
			}
			if (pargUB != NULL && !mxIsEmpty(pargUB)) {
				if ((pargLB == NULL) ^ (pargUB == NULL)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Lower bound and upper bound must be both empty.");
					return;
				}
				if (mxIsEmpty(pargLB) ^ mxIsEmpty(pargUB)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Lower bound and upper bound must be both empty.");
					return;
				}
				if ((!mxIsDouble(pargLB) && !mxIsSparse(pargLB)) || mxIsComplex(pargLB)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Lower bound must be a real double.");
					return;
				}
				if ((!mxIsDouble(pargUB) && !mxIsSparse(pargUB)) || mxIsComplex(pargUB)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Upper bound must be a real double.");
					return;
				}
				number_dimensions1 = mxGetNumberOfDimensions(pargLB);
				dimensions1 = mxGetDimensions(pargLB);
				number_dimensions2 = mxGetNumberOfDimensions(pargUB);
				dimensions2 = mxGetDimensions(pargUB);
				if (number_dimensions1 > 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Lower bound must be a vector.");
					return;
				}
				if (number_dimensions2 > 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Upper bound must be a vector.");
					return;
				}
				if (dimensions1[0] != number_variables) {
					mexErrMsgIdAndTxt("KSOPT:input", "Lower bounds must have %d rows.", number_variables);
					return;
				}
				if (dimensions1[1] != 1) {
					mexErrMsgIdAndTxt("KSOPT:input", "Lower bounds must be a column vector.");
					return;
				}
				if (dimensions2[0] != number_variables) {
					mexErrMsgIdAndTxt("KSOPT:input", "Upper bounds must have %d rows.", number_variables);
					return;
				}
				if (dimensions2[1] != 1) {
					mexErrMsgIdAndTxt("KSOPT:input", "Upper bounds must be a column vector.");
					return;
				}
				temp = mxGetPr(pargLB);
				if (temp == NULL) {
					mexErrMsgIdAndTxt("KSOPT:input", "Lower bounds are invalid.");
					return;
				}
				if (mxIsSparse(pargLB)) {
					numericSparseJC = mxGetJc(pargLB);
					if (numericSparseJC == NULL) {
						mexErrMsgIdAndTxt("KSOPT:input", "Numeric data for lower bounds is invalid.");
						return;
					}
					for (ii = 0; ii < numericSparseJC[mxGetN(pargLB)]; ++ii) {
						if (mxIsNaN(temp[ii]) || (!mxIsFinite(temp[ii]) && temp[ii] > 0.0)) {
							mexErrMsgIdAndTxt("KSOPT:input", "Lower bounds must not be NaN or Infinity.");
							return;
						}
					}
					numericSparseJC = NULL;
				}
				else {
					for (ii = 0; ii < mxGetNumberOfElements(pargLB); ++ii) {
						if (mxIsNaN(temp[ii]) || (!mxIsFinite(temp[ii]) && temp[ii] > 0.0)) {
							mexErrMsgIdAndTxt("KSOPT:input", "Lower bounds must not be NaN or Infinity.");
							return;
						}
					}
				}
				temp = NULL;
				temp = mxGetPr(pargUB);
				if (temp == NULL) {
					mexErrMsgIdAndTxt("KSOPT:input", "Upper bounds are invalid.");
					return;
				}
				if (mxIsSparse(pargUB)) {
					numericSparseJC = mxGetJc(pargUB);
					if (numericSparseJC == NULL) {
						mexErrMsgIdAndTxt("KSOPT:input", "Numeric data for upper bounds is invalid.");
						return;
					}
					for (ii = 0; ii < numericSparseJC[mxGetN(pargUB)]; ++ii) {
						if (mxIsNaN(temp[ii]) || (!mxIsFinite(temp[ii]) && temp[ii] > 0.0)) {
							mexErrMsgIdAndTxt("KSOPT:input", "Lower bounds must not be NaN or Infinity.");
							return;
						}
					}
					numericSparseJC = NULL;
				}
				else {
					for (ii = 0; ii < mxGetNumberOfElements(pargUB); ++ii) {
						if (mxIsNaN(temp[ii]) || (!mxIsFinite(temp[ii]) &&  temp[ii] < 0.0)) {
							mexErrMsgIdAndTxt("KSOPT:input", "Lower bounds must not be NaN or -Infinity.");
							return;
						}
					}
				}
				temp = NULL;
			}
			else {
				mexErrMsgIdAndTxt("KSOPT:input", "Lower bound and upper bound must not be empty.");
				return;
			}
		}
		else {
			if (nrhs > argUB) {
				if (pargUB != NULL && !mxIsEmpty(pargUB)) {
					if ((pargLB == NULL) ^ (pargUB == NULL)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Lower and upper bound must be both empty.");
						return;
					}
					if (mxIsEmpty(pargLB) ^ mxIsEmpty(pargUB)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Lower and upper bound must be both empty.");
						return;
					}
				}
			}
		}
	}

	// check nonlinear constraints
	if (nrhs > argNLCON) {
		if (pargNLCON != NULL && !mxIsEmpty(pargNLCON)) {
			if (!mxIsChar(pargNLCON) && !mxIsFunctionHandle(pargNLCON)) {
				mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must be a function handle or a name of a function.");
				return;
			}
			success = ksopt_checkhandle(pargNLCON, pargX0, KSOPT_CONSTRAINT, &constraintinfo);
			if (success != 0) {
				mexErrMsgIdAndTxt("KSOPT:input", "Constraint function is not valid.");
				return;
			}
			*number_constraints_ineq = constraintinfo.number_constraints_ineq;
			*number_constraints_eq = constraintinfo.number_constraints_eq;
		}
	}

	hasgradient = objectiveinfo.hasgradient && (constraintinfo.number_constraints_ineq + constraintinfo.number_constraints_eq <= 0 || constraintinfo.hasgradient);

	// check options
	if (nrhs > argOPTIONS) {
		boolean_T usedefaultoptions = false;
		boolean_T isstruct = false;
		boolean_T isobject = false;
		if (pargOPTIONS == NULL) {
			mexErrMsgIdAndTxt("KSOPT:input", "Options are invalid.");
			return;
		}
		isstruct = mxIsStruct(pargOPTIONS);
		isobject = mxIsClass(pargOPTIONS, "optim.options.SolverOptions");
		if (mxIsEmpty(pargOPTIONS)) {
			usedefaultoptions = true;
		}
		else {
			if (!mxIsScalar(pargOPTIONS) || (!isstruct && !isobject)) {
				mexErrMsgIdAndTxt("KSOPT:input", "Options must be a scalar structure or object of type 'optim.options.SolverOptions'.");
				return;
			}
		}
		if (isstruct) {
			number_fields = mxGetNumberOfFields(pargOPTIONS);
			if (number_fields == 0) {
				usedefaultoptions = true;
			}
		}
		if (usedefaultoptions) {
			*options = defaultOptions;
		}
		else {
			// MaxIterations
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "MaxIterations");
				if (field == NULL) {
					field = mxGetField(pargOPTIONS, 0 , "MaxIter");
				}
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "MaxIterations");
				if (field == NULL) {
					field = mxGetProperty(pargOPTIONS, 0, "MaxIter");
				}
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsScalar(field) || !mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Number of maximum iterations must be a numeric scalar.");
					return;
				}
				if (mxIsInt8(field) || mxIsUint8(field) || mxIsInt16(field) || mxIsUint16(field) || mxIsInt32(field) || mxIsUint32(field) || mxIsInt64(field) || mxIsUint64(field)) {
					doubleoption = (int) mxGetScalar(field);
				}
				else {
					doubleoption = mxGetScalar(field);
				}
				if (!mxIsFinite(doubleoption) || mxIsNaN(doubleoption)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Number of maximum iterations must be finite.");
					return;
				}
				options->maxiter = (int) doubleoption;
			}
			else {
				options->maxiter = defaultOptions.maxiter;
			}

			//MaxFunctionEvaluations
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "MaxFunctionEvaluations");
				if (field == NULL) {
					field = mxGetField(pargOPTIONS, 0, "MaxFunEvals");
				}
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "MaxFunctionEvaluations");
				if (field == NULL) {
					field = mxGetProperty(pargOPTIONS, 0, "MaxFunEvals");
				}
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsScalar(field) || !mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Number of maximum function evaluations must be a numeric scalar.");
					return;
				}
				if (mxIsInt8(field) || mxIsUint8(field) || mxIsInt16(field) || mxIsUint16(field) || mxIsInt32(field) || mxIsUint32(field) || mxIsInt64(field) || mxIsUint64(field)) {
					doubleoption = (int) mxGetScalar(field);
				}
				else {
					doubleoption = mxGetScalar(field);
				}
				if (!mxIsFinite(doubleoption) || mxIsNaN(doubleoption)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Number of maximum function evaluations must be finite.");
					return;
				}
				options->maxfunevals = (int) doubleoption;
			}
			else {
				options->maxfunevals = defaultOptions.maxfunevals;
			}

			//MaxTime
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "MaxTime");
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "MaxTime");
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsScalar(field) || !mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Number of maximum iterations must be a numeric scalar.");
					return;
				}
				doubleoption = mxGetScalar(field);
				if (!mxIsFinite(doubleoption) || mxIsNaN(doubleoption)) {
					doubleoption = 0.0;
				}
				options->maxtime = doubleoption;
			}
			else {
				options->maxtime = defaultOptions.maxtime;
			}

			//ObjectiveLimit
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "ObjectiveLimit");
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "ObjectiveLimit");
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsScalar(field) || !mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "ObjectiveLimit must be a numeric scalar.");
					return;
				}
				if (mxIsInt8(field) || mxIsUint8(field) || mxIsInt16(field) || mxIsUint16(field) || mxIsInt32(field) || mxIsUint32(field) || mxIsInt64(field) || mxIsUint64(field)) {
					doubleoption = (double) mxGetScalar(field);
				}
				else {
					doubleoption = mxGetScalar(field);
				}
				options->hasobjectivelimit = !mxIsFinite(doubleoption) || mxIsNaN(doubleoption);
				if (options->hasobjectivelimit) {
					options->objectivelimit = doubleoption;
				}
				else {
					options->objectivelimit = -HUGE_VAL;
				}
			}
			else {
				options->hasobjectivelimit = defaultOptions.hasobjectivelimit;
				options->objectivelimit = defaultOptions.objectivelimit;
			}

			//FiniteDifferenceStepSize
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "FiniteDifferenceStepSize");
				if (field == NULL) {
					field = mxGetField(pargOPTIONS, 0, "FinDiffRelStep");
				}
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "FiniteDifferenceStepSize");
				if (field == NULL) {
					field = mxGetProperty(pargOPTIONS, 0, "FinDiffRelStep");
				}
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsScalar(field) || !mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Finite difference step size must be a numeric scalar.");
					return;
				}
				doubleoption = mxGetScalar(field);
				if (!mxIsFinite(doubleoption) || mxIsNaN(doubleoption)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Finite difference step size must be finite.");
					return;
				}
				options->findiffstepsize = doubleoption;
			}
			else {
				options->findiffstepsize = defaultOptions.findiffstepsize;
			}

			//FiniteDifferenceStepSizeMin
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "FiniteDifferenceStepSizeMin");
				if (field == NULL) {
					field = mxGetField(pargOPTIONS, 0, "DiffMinChange");
				}
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "FiniteDifferenceStepSizeMin");
				if (field == NULL) {
					field = mxGetProperty(pargOPTIONS, 0, "DiffMinChange");
				}
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsScalar(field) || !mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Minimum finite difference step size must be a numeric scalar.");
					return;
				}
				doubleoption = mxGetScalar(field);
				if (!mxIsFinite(doubleoption) || mxIsNaN(doubleoption)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Minimum finite difference step size must be finite.");
					return;
				}
				options->findiffstepsizemin = doubleoption;
			}
			else {
				options->findiffstepsizemin = defaultOptions.findiffstepsizemin;
			}

			//OptimalityTolerance
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "OptimalityTolerance");
				if (field == NULL) {
					field = mxGetField(pargOPTIONS, 0, "TolFun");
				}
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "OptimalityTolerance");
				if (field == NULL) {
					field = mxGetProperty(pargOPTIONS, 0, "TolFun");
				}
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsScalar(field) || !mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Optimality tolerance must be a numeric scalar.");
					return;
				}
				doubleoption = mxGetScalar(field);
				if (!mxIsFinite(doubleoption) || mxIsNaN(doubleoption)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Optimality tolerance must be finite.");
					return;
				}
				options->tolfunrel = doubleoption;
			}
			else {
				options->tolfunrel = defaultOptions.tolfunrel;
			}

			//OptimalityToleranceAbs
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "OptimalityToleranceAbs");
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "OptimalityToleranceAbs");
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsScalar(field) || !mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Absolute optimality tolerance must be a numeric scalar.");
					return;
				}
				doubleoption = mxGetScalar(field);
				if (!mxIsFinite(doubleoption) || mxIsNaN(doubleoption)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Absolute optimality tolerance must be finite.");
					return;
				}
				options->tolfunabs = doubleoption;
			}
			else {
				options->tolfunabs = defaultOptions.tolfunabs;
			}

			//RhoIncrement
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "RhoIncrement");
			}
			//else if (isobject) {
			//	field = mxGetProperty(pargOPTIONS, 0, "RhoIncrement");
			//}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsScalar(field) || !mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Increment for rho must be a numeric scalar.");
					return;
				}
				doubleoption = mxGetScalar(field);
				if (!mxIsFinite(doubleoption) || mxIsNaN(doubleoption)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Increment for rho must be finite.");
					return;
				}
				options->rhodel = doubleoption;
			}
			else {
				options->rhodel = defaultOptions.rhodel;
			}

			//RhoMin
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "RhoMin");
			}
			//else if (isobject) {
			//	field = mxGetProperty(pargOPTIONS, 0, "RhoMin");
			//}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsScalar(field) || !mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Minimum rho must be a numeric scalar.");
					return;
				}
				doubleoption = mxGetScalar(field);
				if (!mxIsFinite(doubleoption) || mxIsNaN(doubleoption)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Minimum rho must be finite.");
					return;
				}
				options->rhomin = doubleoption;
			}
			else {
				options->rhomin = defaultOptions.rhomin;
			}

			//RhoMax
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "RhoMax");
			}
			//else if (isobject) {
			//	field = mxGetProperty(pargOPTIONS, 0, "RhoMax");
			//}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsScalar(field) || !mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Maximum rho must be a numeric scalar.");
					return;
				}
				doubleoption = mxGetScalar(field);
				if (!mxIsFinite(doubleoption) || mxIsNaN(doubleoption)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Maximum rho must be finite.");
					return;
				}
				options->rhomax = doubleoption;
			}
			else {
				options->rhomax = defaultOptions.rhomax;
			}

			//Display
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "Display");
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "Display");
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (mxIsNumeric(field)) {
					if (!mxIsScalar(field)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Display must be scalar.");
						return;
					}
					intoption = (int) mxGetScalar(field);
					if (!mxIsFinite(intoption) || mxIsNaN(intoption)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Display must be finite.");
						return;
					}
					if (intoption < 0 || intoption > 3) {
						mexErrMsgIdAndTxt("KSOPT:input", "Display must be between 0 and 3.");
						return;
					}
					options->print = intoption;
				}
				else if (mxIsChar(field)) {
					charoption = mxArrayToString(field);
					if (charoption == NULL) {
						mexErrMsgIdAndTxt("KSOPT:input", "Display must be a valid string.");
						return;
					}
					if (!strcmpi(charoption, "off")) {
						options->print = 0;
					}
					else if (!strcmpi(charoption, "final")) {
						options->print = 1;
					}
					else if (!strcmpi(charoption, "final-detailed")) {
						options->print = 11;
					}
					else if (!strcmpi(charoption, "iter")) {
						options->print = 112;
					}
					else if (!strcmpi(charoption, "iter-detailed")) {
						options->print = 113;
					}
					else if (!strcmpi(charoption, "notify")) {
						options->print = 102;
					}
					else if (!strcmpi(charoption, "notify-detailed")) {
						options->print = 103;
					}
					else {
						mxFree(charoption);
						charoption = NULL;
						mexErrMsgIdAndTxt("KSOPT:input", "Display must be a valid string.");
						return;
					}
					if (charoption != NULL) {
						mxFree(charoption);
						charoption = NULL;
					}
				}
				else {
					mexErrMsgIdAndTxt("KSOPT:input", "Display must be scalar or a string.");
					return;
				}
			}
			else {
				options->print = defaultOptions.print;
			}

			//Scale
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "Scale");
			}
			//else if (isobject) {
			//	field = mxGetProperty(pargOPTIONS, 0, "Scale");
			//}
			else {
				field = NULL;
			}
			if (isstruct) {
				otherfield = mxGetField(pargOPTIONS, 0, "ScaleVector");
			}
			//else if (isobject) {
			//	otherfield = mxGetProperty(pargOPTIONS, 0, "ScaleVector");
			//}
			else {
				otherfield = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (!mxIsNumeric(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Scaling must be numeric.");
					return;
				}
				if (mxIsInt8(field) || mxIsUint8(field) || mxIsInt16(field) || mxIsUint16(field) || mxIsInt32(field) || mxIsUint32(field) || mxIsInt64(field) || mxIsUint64(field)) {
					intoption = (int) mxGetScalar(field);
				}
				else {
					doubleoption = mxGetScalar(field);
					if (floor(doubleoption) != ceil(doubleoption)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Scaling must be a number.");
						return;
					}
					intoption = (int) doubleoption;
				}
				if (!mxIsFinite(intoption) || mxIsNaN(intoption)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Scaling must be finite.");
					return;
				}
				options->scale = intoption;
				if (intoption < 0) {
					if (otherfield != NULL && !mxIsEmpty(otherfield)) {
						if (!mxIsNumeric(otherfield) || mxIsComplex(otherfield)) {
							mexErrMsgIdAndTxt("KSOPT:input", "Scaling vector must be real and numeric.");
							return;
						}
						if (number_variables != mxGetNumberOfElements(otherfield)) {
							mexErrMsgIdAndTxt("KSOPT:input", "Scaling vector must have %d elements.", number_variables);
							return;
						}
						number_dimensions1 = mxGetNumberOfDimensions(otherfield);
						dimensions1 = mxGetDimensions(otherfield);
						if (number_dimensions1 > 2) {
							mexErrMsgIdAndTxt("KSOPT:input", "Scaling vector must be a vector.");
							return;
						}
						if (dimensions1[0] != number_variables) {
							mexErrMsgIdAndTxt("KSOPT:input", "Scaling vector must have %d rows.", number_variables);
							return;
						}
						if (dimensions1[1] != 1) {
							mexErrMsgIdAndTxt("KSOPT:input", "Scaling vector must be a column vector.");
							return;
						}
						temp = mxGetPr(otherfield);
						for (ii = 0; ii < mxGetNumberOfElements(otherfield); ++ii) {
							if (mxIsNaN(temp[ii]) || (!mxIsFinite(temp[ii])) && temp[ii] > 0.0) {
								mexErrMsgIdAndTxt("KSOPT:input", "Scaling vector must not be NaN or Infinity.");
								return;
							}
						}
						temp = NULL;
						options->scalefactorsize = (int) dimensions1[0];
						options->scalefactor = (double*) mxCalloc(options->scalefactorsize, sizeof(double));
						voidptr = memcpy(options->scalefactor, mxGetPr(otherfield), number_variables*sizeof(double));
						if (voidptr == NULL) {
							mexErrMsgIdAndTxt("KSOPT:input", "Not enough memory to copy scaling vector.");
							return;
						}
					}
					else {
						mexErrMsgIdAndTxt("KSOPT:input", "When user defined scaling vector is activated, scaling vector must not be empty.");
						return;
					}
				}
				else {
					if (otherfield != NULL && !mxIsEmpty(otherfield)) {
						mexWarnMsgIdAndTxt("KSOPT:input", "User defined scaling vector is ignored, because of automatic scaling.");
					}
				}
			}
			else {
				// no scaling
				options->scale = 0;
				options->scalefactor = NULL;
				options->scalefactorsize = 0;
			}

			//Gradient
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "SpecifyObjectiveGradient");
				if (field == NULL) {
					field = mxGetField(pargOPTIONS, 0, "GradObj");
				}
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "SpecifyObjectiveGradient");
				if (field == NULL) {
					field = mxGetProperty(pargOPTIONS, 0, "GradObj");
				}
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (mxIsChar(field)) {
					charoption = mxArrayToString(field);
					if (charoption == NULL) {
						mexErrMsgIdAndTxt("KSOPT:input", "Objective gradient must be a valid string and either 'on' or 'off'.");
						return;
					}
					if (!strcmp(charoption, "on")) {
						booleanoption = true;
					}
					else if (!strcmp(charoption, "off")) {
						booleanoption = false;
					}
					else {
						mxFree(charoption);
						charoption = NULL;
						mexErrMsgIdAndTxt("KSOPT:input", "Objective gadient indicator must be a logical scalar.");
						return;
					}
					mxFree(charoption);
					charoption = NULL;
				}
				else {
					if (!mxIsScalar(field) || !mxIsLogical(field) || !mxIsLogicalScalar(field)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Objective gadient indicator must be a logical scalar.");
						return;
					}
					booleanoption = (boolean_T) (mxGetScalar(field) != 0.0);
					if (!mxIsFinite(booleanoption) || mxIsNaN(booleanoption)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Objective gradient indicator must be finite.");
						return;
					}
				}
				if (!objectiveinfo.hasgradient && booleanoption) {
					mexErrMsgIdAndTxt("KSOPT:input", "Objective gradient can not be used because the objective function does not supply gradient information.");
					return;
				}
				objectivegradient = (unsigned short) booleanoption;
			}

			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "SpecifyConstraintGradient");
				if (field == NULL) {
					field = mxGetField(pargOPTIONS, 0, "GradConstr");
				}
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "SpecifyConstraintGradient");
				if (field == NULL) {
					field = mxGetProperty(pargOPTIONS, 0, "GradConstr");
				}
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				if (mxIsChar(field)) {
					charoption = mxArrayToString(field);
					if (charoption == NULL) {
						mexErrMsgIdAndTxt("KSOPT:input", "Objective gradient must be a valid string and either 'on' or 'off'.");
						return;
					}
					if (!strcmp(charoption, "on")) {
						booleanoption = true;
					}
					else if (!strcmp(charoption, "off")) {
						booleanoption = false;
					}
					else {
						mxFree(charoption);
						charoption = NULL;
						mexErrMsgIdAndTxt("KSOPT:input", "Objective gadient indicator must be a logical scalar.");
						return;
					}
					mxFree(charoption);
					charoption = NULL;
				}
				else {
					if (!mxIsScalar(field) || !mxIsLogical(field) || !mxIsLogicalScalar(field)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint gadient indicator must be a logical scalar.");
						return;
					}
					booleanoption = (boolean_T) (mxGetScalar(field) != 0.0);
					if (!mxIsFinite(booleanoption) || mxIsNaN(booleanoption)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint gradient indicator must be finite.");
						return;
					}
					if ((constraintinfo.number_constraints_ineq + constraintinfo.number_constraints_eq > 0 && !constraintinfo.hasgradient) && booleanoption) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint gradient can not be used because the constraint function does not supply gradient information.");
						return;
					}
				}
				if (constraintinfo.number_constraints_ineq + constraintinfo.number_constraints_eq <= 0) {
					constraintgradient = objectivegradient;
				}
				else {
					constraintgradient = (unsigned short) booleanoption;
				}
			}
			/*if (objectivegradient != constraintgradient) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective and constraint gradient must be both supplied or not supplied for KSOPT.");
				return;
			}*/
			options->gradient = hasgradient && objectivegradient && constraintgradient;

			// PlotFcn
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "PlotFcn");
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "PlotFcn");
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				int nargin = 0;
				int nargout = 0;
				int success = 0;
				int *narginarray = NULL;
				int *nargoutarray = NULL;
				if (!mxIsCell(field) && !mxIsFunctionHandle(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Plot function must be a function handle or a cell array of function handles.");
					return;
				}
				iterinfoplot->type = KSOPT_ITER;
				if (mxIsFunctionHandle(field)) {
					if (!mxIsScalar(field)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Plot function must be a scalar function handle.");
						return;
					}
					success = ksopt_get_nargin(field, KSOPT_ITER, &nargin);
					if (success != 0) {
						mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargin for iteration function.");
						return;
					}
					success = ksopt_get_nargout(field, KSOPT_ITER, &nargout);
					if (success != 0) {
						mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargout for iteration function.");
						return;
					}
					if (nargout < 1) {
						mexErrMsgIdAndTxt("KSOPT:input", "PlotFcn must have one output.");
						return;
					}
					if (nargin != 3 && nargin != -4) {
						mexErrMsgIdAndTxt("KSOPT:input", "PlotFcn must have three inputs.");
						return;
					}
					iterinfoplot->number_functions = 1;
					iterinfoplot->number_inputs = (int*) mxCalloc(1, sizeof(double));
					iterinfoplot->number_outputs = (int*) mxCalloc(1, sizeof(double));
					iterinfoplot->number_inputs[0] = nargin;
					iterinfoplot->number_outputs[0] = nargout;
				}
				else {
					mxArray *cellvalue = NULL;
					narginarray = (int*) mxCalloc(mxGetNumberOfElements(field), sizeof(double));
					nargoutarray = (int*) mxCalloc(mxGetNumberOfElements(field), sizeof(double));
					iterinfoplot->number_functions = (int) mxGetNumberOfElements(field);
					for (ii = 0; ii < mxGetNumberOfElements(field); ++ii) {
						cellvalue = mxGetCell(field, ii);
						if (cellvalue == NULL) {
							mexErrMsgIdAndTxt("KSOPT:input", "Plot function cell array is invalid.");
							return;
						}
						if (!mxIsFunctionHandle(cellvalue)) {
							mexErrMsgIdAndTxt("KSOPT:input", "Plot function must be a function handle.");
							return;
						}
						if (!mxIsScalar(cellvalue)) {
							mexErrMsgIdAndTxt("KSOPT:input", "Plot function must be a scalar function handle.");
							return;
						}
						success = ksopt_get_nargin(cellvalue, KSOPT_ITER, &nargin);
						if (success != 0) {
							mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargin for iteration function.");
							return;
						}
						success = ksopt_get_nargout(cellvalue, KSOPT_ITER, &nargout);
						if (success != 0) {
							mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargout for iteration function.");
							return;
						}
						if (nargout < 1) {
							mexErrMsgIdAndTxt("KSOPT:input", "PlotFcn must have one output.");
							return;
						}
						if (nargin != 3 && nargin != -4) {
							mexErrMsgIdAndTxt("KSOPT:input", "PlotFcn must have three inputs.");
							return;
						}
						narginarray[ii] = nargin;
						nargoutarray[ii] = nargout;
					}
					iterinfoplot->number_inputs = narginarray;
					iterinfoplot->number_outputs = nargoutarray;
				}
			}
			else {
				iterinfoplot->type = KSOPT_ITER;
				iterinfoplot->number_functions = 0;
				iterinfoplot->number_inputs = NULL;
				iterinfoplot->number_outputs = NULL;
			}

			//OutputFcn
			if (isstruct) {
				field = mxGetField(pargOPTIONS, 0, "OutputFcn");
			}
			else if (isobject) {
				field = mxGetProperty(pargOPTIONS, 0, "OutputFcn");
			}
			else {
				field = NULL;
			}
			if (field != NULL && !mxIsEmpty(field)) {
				int nargin = 0;
				int nargout = 0;
				int success = 0;
				int *narginarray = NULL;
				int *nargoutarray = NULL;
				if (!mxIsCell(field) && !mxIsFunctionHandle(field)) {
					mexErrMsgIdAndTxt("KSOPT:input", "Plot function must be a function handle or a cell array of function handles.");
					return;
				}
				iterinfooutput->type = KSOPT_ITER;
				if (mxIsFunctionHandle(field)) {
					if (!mxIsScalar(field)) {
						mexErrMsgIdAndTxt("KSOPT:input", "Plot function must be a scalar function handle.");
						return;
					}
					success = ksopt_get_nargin(field, KSOPT_ITER, &nargin);
					if (success != 0) {
						mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargin for PlotFcn.");
						return;
					}
					success = ksopt_get_nargout(field, KSOPT_ITER, &nargout);
					if (success != 0) {
						mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargout for PlotFcn.");
						return;
					}
					if (nargout < 1) {
						mexErrMsgIdAndTxt("KSOPT:input", "OutputFcn must have one output.");
						return;
					}
					if (nargin != 3 && nargin != -4) {
						mexErrMsgIdAndTxt("KSOPT:input", "OutputFcn must have three inputs.");
						return;
					}
					iterinfooutput->number_functions = 1;
					iterinfooutput->number_inputs = (int*) mxCalloc(1, sizeof(double));
					iterinfooutput->number_outputs = (int*) mxCalloc(1, sizeof(double));
					iterinfooutput->number_inputs[0] = nargin;
					iterinfooutput->number_outputs[0] = nargout;
				}
				else {
					mxArray *cellvalue = NULL;
					narginarray = (int*) mxCalloc(mxGetNumberOfElements(field), sizeof(double));
					nargoutarray = (int*) mxCalloc(mxGetNumberOfElements(field), sizeof(double));
					iterinfooutput->number_functions = (int) mxGetNumberOfElements(field);
					for (ii = 0; ii < mxGetNumberOfElements(field); ++ii) {
						cellvalue = mxGetCell(field, ii);
						if (cellvalue == NULL) {
							mexErrMsgIdAndTxt("KSOPT:input", "Output function cell array is invalid.");
							return;
						}
						if (!mxIsFunctionHandle(cellvalue)) {
							mexErrMsgIdAndTxt("KSOPT:input", "Output function must be a function handle.");
							return;
						}
						if (!mxIsScalar(cellvalue)) {
							mexErrMsgIdAndTxt("KSOPT:input", "Output function must be a scalar function handle.");
							return;
						}
						success = ksopt_get_nargin(cellvalue, KSOPT_ITER, &nargin);
						if (success != 0) {
							mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargin for OutputFcn.");
							return;
						}
						success = ksopt_get_nargout(cellvalue, KSOPT_ITER, &nargout);
						if (success != 0) {
							mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargout for OutputFcn.");
							return;
						}
						if (nargout < 1) {
							mexErrMsgIdAndTxt("KSOPT:input", "OutputFcn must have one output.");
							return;
						}
						if (nargin != 3 && nargin != -4) {
							mexErrMsgIdAndTxt("KSOPT:input", "OutputFcn must have three inputs.");
							return;
						}
						narginarray[ii] = nargin;
						nargoutarray[ii] = nargout;
					}
					iterinfooutput->number_inputs = narginarray;
					iterinfooutput->number_outputs = nargoutarray;
				}
			}
			else {
				iterinfooutput->type = KSOPT_ITER;
				iterinfooutput->number_functions = 0;
				iterinfooutput->number_inputs = NULL;
				iterinfooutput->number_outputs = NULL;
			}
		}
	}

	objectiveinformation->number_objectives = objectiveinfo.number_objectives;
	objectiveinformation->hasgradient = objectiveinfo.hasgradient && constraintinfo.hasgradient;
	objectiveinformation->number_constraints_ineq = 0;
	objectiveinformation->number_constraints_eq = 0;
	objectiveinformation->number_inputs = objectiveinfo.number_inputs;
	objectiveinformation->number_outputs = objectiveinfo.number_outputs;
	objectiveinformation->type = KSOPT_OBJECTIVE;
	objectiveinformation->wasevaluated = objectiveinfo.wasevaluated;

	constraintinformation->number_objectives = 0;
	constraintinformation->hasgradient = constraintinfo.hasgradient && objectiveinfo.hasgradient;
	constraintinformation->number_constraints_ineq = constraintinfo.number_constraints_ineq;
	constraintinformation->number_constraints_eq = constraintinfo.number_constraints_eq;
	constraintinformation->number_inputs = constraintinfo.number_inputs;
	constraintinformation->number_outputs = constraintinfo.number_outputs;
	constraintinformation->type = KSOPT_CONSTRAINT;
	constraintinformation->wasevaluated = constraintinfo.wasevaluated;
}

static bool checkIdentX(const double *x, const double *X, const int n) {
	/*CHECKIDENTX check if two vectors are the same to handle case when objective and constraints are called with the same input argument for gradient information
	*	Input:
	*		x:	first vector to check
	*		X:	second vector to check
	*		n:	number of elements
	*	Output:
	*		true if vectors are the same else false
	*/
	int ii = 0;
	if (n <= 0) {
		return false;
	}
	if (x == NULL || X == NULL) {
		return false;
	}
	//Check each for equality, should be no need for tolerance
	for (ii = 0; ii < n; ii++) {
		if (x[ii] != X[ii]) {
			return false;
		}
	}
	return true;
}

static void clearVariables(KSOPToptionType *options, KSOPT_iterfunctionHandleInformation *iterinformationplot, KSOPT_iterfunctionHandleInformation *iterinformationoutput, const size_t number_objectives, const size_t number_constraints_ineq, const size_t number_constraints_eq, const size_t number_constraints_linear_ineq, const size_t number_constraints_linear_eq) {
	/*CLEARVARIABLES free memory allocated in structures with information about problem structure
	*	Input:
	*		options:						structure with options
	*		iterinformationplot:			structure with information about plotfunction
	*		iterinformationoutput:			structure with information about outputfunction
	*		number_objectives:				number of objectives
	*		number_constraints_ineq:		number of inequality constraints
	*		number_constraints_eq:			number of equality constraints
	*		number_constraints_linear_ineq:	number of linear inequality constraints
	*		number_constraints_linear_eq:	number of linear equality constraints
	*/
	if (options != NULL && options->scalefactor != NULL) {
		mxFree(options->scalefactor);
		options->scalefactor = NULL;
	}
	if (objective_function_info.prhs[objective_function_info.xrhs] != NULL) {
		mxDestroyArray(objective_function_info.prhs[objective_function_info.xrhs]);
		objective_function_info.prhs[objective_function_info.xrhs] = NULL;
	}
	if (objective_function_info.prhs[0] != NULL) {
		mxDestroyArray(objective_function_info.prhs[0]);
		objective_function_info.prhs[0] = NULL;
	}
	if (objective_function_info.prhs[1] != NULL) {
		mxDestroyArray(objective_function_info.prhs[1]);
		objective_function_info.prhs[1] = NULL;
	}
	if (objective_function_info.plhs[0] != NULL) {
		mxDestroyArray(objective_function_info.plhs[0]);
		objective_function_info.plhs[0] = NULL;
	}
	if (constraint_function_info.plhs[1] != NULL) {
		mxDestroyArray(objective_function_info.plhs[1]);
		objective_function_info.plhs[1] = NULL;
	}
	INIT_FNAME(objective);
	if (constraint_function_info.prhs[constraint_function_info.xrhs] != NULL) {
		mxDestroyArray(constraint_function_info.prhs[constraint_function_info.xrhs]);
		constraint_function_info.prhs[constraint_function_info.xrhs] = NULL;
	}
	if (constraint_function_info.prhs[0] != NULL) {
		mxDestroyArray(constraint_function_info.prhs[0]);
		constraint_function_info.prhs[0] = NULL;
	}
	if (constraint_function_info.prhs[1] != NULL) {
		mxDestroyArray(constraint_function_info.prhs[1]);
		constraint_function_info.prhs[1] = NULL;
	}
	if (constraint_function_info.plhs[0] != NULL) {
		mxDestroyArray(constraint_function_info.plhs[0]);
		constraint_function_info.plhs[0] = NULL;
	}
	if (constraint_function_info.plhs[1] != NULL) {
		mxDestroyArray(constraint_function_info.plhs[1]);
		constraint_function_info.plhs[1] = NULL;
	}
	if (constraint_function_info.plhs[2] != NULL) {
		mxDestroyArray(constraint_function_info.plhs[2]);
		constraint_function_info.plhs[2] = NULL;
	}
	if (constraint_function_info.plhs[3] != NULL) {
		mxDestroyArray(constraint_function_info.plhs[3]);
		constraint_function_info.plhs[3] = NULL;
	}
	INIT_FNAME(constraint);
	if (iterinformationplot->number_functions > 0) {
		if (iterinformationplot->number_inputs != NULL) {
			mxFree(iterinformationplot->number_inputs);
			iterinformationplot->number_inputs = NULL;
		}
		if (iterinformationplot->number_outputs != NULL) {
			mxFree(iterinformationplot->number_outputs);
			iterinformationplot->number_outputs = NULL;
		}
	}
	if (iterinformationoutput->number_functions > 0) {
		if (iterinformationoutput->number_inputs != NULL) {
			mxFree(iterinformationoutput->number_inputs);
			iterinformationoutput->number_inputs = NULL;
		}
		if (iterinformationoutput->number_outputs != NULL) {
			mxFree(iterinformationoutput->number_outputs);
			iterinformationoutput->number_outputs = NULL;
		}
	}
}

//Print Solver Information
static void printSolverInfo(void) {
	mexPrintf("\n-----------------------------------------------------------\n");
	mexPrintf(" KSOPT: Nonlinear Optimization [v%s]\n", PACKAGE_VERSION);
	mexPrintf("  - Released under the GNU Lesser General Public License: http://www.gnu.org/copyleft/lesser.html\n");
	mexPrintf("  - Source available from: http://github.com/pvogt09/KSOPT_mex\n");
	mexPrintf("\n MEX Interface Patrick Vogt 2020\n");
	mexPrintf("-----------------------------------------------------------\n");
}