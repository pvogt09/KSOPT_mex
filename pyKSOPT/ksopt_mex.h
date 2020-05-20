// types of functions that are used
#ifdef __cplusplus
    extern "C" {
#endif
typedef enum {
	// objective function type
	KSOPT_OBJECTIVE,
	// constraint function type
	KSOPT_CONSTRAINT,
	// iteration function type
	KSOPT_ITER
} KSOPT_functionType;

typedef enum {
	//exitflag for successful termination
	KSOPT_SUCCESS = 0,
	// exitflag for maximum number of iteartions
	KSOPT_MAXITER = 3,
	// exitflag for maximum number of function evaluations
	KSOPT_MAXFUNEVALS = 5,
	// exitflag for infeasible solution
	KSOPT_INFEASIBLE = 4,
	// exitflag for maximum time
	KSOPT_MAXTIME = 6,
	// exitflag for objective limit
	KSOPT_OBJECTIVELIMIT = 7,
	// exitflag for user interrupt
	KSOPT_USERINTERRUPT = -1,
	// exitflag for unknown internal error in ksmain
	KSOPT_INTERNAL_ERROR = -2,
	// exitflag for internal error in objective function evaluation
	KSOPT_INTERNAL_OBJECTIVEERROR = -3,
	// exitflag for internal error in constraint function evaluation
	KSOPT_INTERNAL_CONSTRAINTERROR = -4,
	// exitflag for matlab error in objective function evaluation
	KSOPT_MATLAB_OBJECTIVEERROR = -5,
	// exitflag for matlab error in constraint function evaluation
	KSOPT_MATLAB_CONSTRAINTERROR = -6,
	// exitflag for interrupt by plot or output function
	KSOPT_PLOTINTERRUPT = -7,
	// exitflag for internal error in iteration function evaluation
	KSOPT_INTERNAL_ITERERROR = -8,
	// exitflag for matlab error in iteration function evaluation
	KSOPT_MATLAB_ITERERROR = -9,
} KSOPT_exitflag;

// information about Matlab function handles
typedef struct {
	// type of function
	KSOPT_functionType type;
	// number of function input arguments
	int number_inputs;
	// number of function output arguments
	int number_outputs;
	// indicator if function is able to return gradient information
	boolean_T hasgradient;
	// indicator if function has been evaluated
	boolean_T wasevaluated;
	// numer of objectives the function returns
	size_t number_objectives;
	// number of inequality constraints the function returns
	size_t number_constraints_ineq;
	// number of equality constraints the function returns
	size_t number_constraints_eq;
} KSOPT_functionHandleInformation;

// information about Matlab function handles
typedef struct {
	// type of function (should always be KSOPT_ITER)
	KSOPT_functionType type;
	// number of functions
	int number_functions;
	// number of function input arguments
	int *number_inputs;
	// number of function output arguments
	int *number_outputs;
} KSOPT_iterfunctionHandleInformation;

/*
 *KSOPT_CHECKHANDLE check function handle for validity to use with KSOPT and throw matlab error if invalid
 *	Input:
 *		functionhandle:	mxArray of a function handle to check for validity
 *		x:				optimization value to test function at
 *		type:			type of function the function handle should be used for (objective function, constraint function, iteration function)
 *	Output:
 *		info:			structure with information about function handle and its return values (dimensions, gradient information, etc.)
 *		exitflag:		zero if checks passed else nonzero
 */
extern int ksopt_checkhandle(const mxArray *functionhandle, const mxArray *x, const KSOPT_functionType type, KSOPT_functionHandleInformation *info);
/*
 *KSOPT_SPARSE2FULL convert sparse matrix in matlab format to full matrix
 *	Input:
 *		sparse:		mxArray with sparse matrix to convert
 *		m:			number of rows of matrix
 *		n:			number of columns of matrix
 *		throwerror:	indicator if matlab errors should be thrown with mexErrMsgIdAndTxt or only a nonzero exit value should be returned
 *	Output:
 *		full:		double matrix with content of supplied sparse matrix
 *		exitflag:	nonzero number in case of error, zero in case of success
 */
extern int ksopt_sparse2full(const mxArray *sparse, double *full, const size_t m, const size_t n, const boolean_T throwerror);
/*
 *KSOPT_GET_NARGIN get number of input arguments of a matlab function handle
 *	Input:
 *		functionhandle:	function handle to get number of output arguments for
 *		type:			type of function the function handle should be used for (objective function, constraint function, iteration function)
 *	Output:
 *		nargout:		number of input arguments of the function handle
 *		exitflag:		zero in case of success else nonzero
 */
extern int ksopt_get_nargin(const mxArray *functionhandle, const KSOPT_functionType type, int *nargin);
/*
 *KSOPT_GET_NARGOUT get number of output arguments of a matlab function handle
 *	Input:
 *		functionhandle:	function handle to get number of output arguments for
 *		type:			type of function the function handle should be used for (objective function, constraint function, iteration function)
 *	Output:
 *		nargout:		number of output arguments of the function handle
 *		exitflag:		zero in case of success else nonzero
 */
extern int ksopt_get_nargout(const mxArray *functionhandle, const KSOPT_functionType type, int *nargout);
/*
 *PRINT_FORMATTED_ERROR allocate memory for formatted string with variable number of replacements by printf
 *	Input:
 *		buffersize:		length of formatted text (without terminating \0)
 *		format:			format string
 *		varargin:		replacements for placeholders
 *	Output:
 *		buffer:			string containing formatted text
 */
extern char* print_formatted_error(size_t *buffersize, const char *format, ...);
#ifdef __cplusplus
    }
#endif
