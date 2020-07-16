#include <mex.h>
#include <string.h>
#include "ksopt.h"
#include "ksopt_mex.h"

typedef struct {
	int nargout;// number of output arguments of objective function
	int evals;// number of evaluations of objective function
	size_t dimension_objectives;// number of dimensions of objectives
	size_t size_objectivesM;// number of rows of objectives
	size_t size_objectivesN;// number of columns of objectives
	size_t dimension_gradient;// number of dimensions of objective gradient
	size_t size_gradientM;// number of rows of objective gradient
	size_t size_gradientN;// number of columns of objective gradient
} objectiveinfo;

typedef struct {
	int nargout;// number of output arguments of constraint function
	int evals;// number of evaluations of constraint function
	size_t dimension_ineq;// number of dimensions of ineqaulity constraints
	size_t size_ineqM;// number of rows of inequalities
	size_t size_ineqN;// number of columns of inequalities
	size_t dimension_ineqgradient;// number of dimensions of ineqaulity constraint gradient
	size_t size_ineqgradientM;// number of rows of inequality gradient
	size_t size_ineqgradientN;// number of columns of inequality gradient
	size_t dimension_eq;// number of dimensions of eqaulity constraints
	size_t size_eqM;// number of rows of equalities
	size_t size_eqN;// number of columns of equalities
	size_t dimension_eqgradient;// number of dimensions of eqaulity constraint gradient
	size_t size_eqgradientM;// number of rows of equality gradient
	size_t size_eqgradientN;// number of columns of equality gradient
} constraintinfo;

static int eval_objective(const mxArray *functionhandle, const mxArray *x, const int nargout, objectiveinfo *info);
static int eval_constraint(const mxArray *functionhandle, const mxArray *x, const int nargout, constraintinfo *info);

int ksopt_checkhandle(const mxArray *functionhandle, const mxArray *x, const KSOPT_functionType type, KSOPT_functionHandleInformation *info) {
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
	int nargin = 0, nargout = 0;
	int success = 0;
	size_t number_variables = 0;
	objectiveinfo objectivedimensions;
	constraintinfo constraintdimensions;
	boolean_T checkobjective = true, checkgradient = true, checkconstraint = true, checkconstraintgradient = true, hasconstraintgradient_ineq = false, hasconstraintgradient_eq = false;
	info->type = type;
	info->hasgradient = false;
	info->number_inputs = 0;
	info->number_outputs = 0;
	info->wasevaluated = false;
	info->number_objectives = 0;
	info->number_constraints_ineq = 0;
	info->number_constraints_eq = 0;

	objectivedimensions.nargout = 0;
	objectivedimensions.dimension_gradient = 0;
	objectivedimensions.dimension_objectives = 0;
	objectivedimensions.evals = 0;
	objectivedimensions.size_gradientM = 0;
	objectivedimensions.size_gradientN = 0;
	objectivedimensions.size_objectivesM = 0;
	objectivedimensions.size_objectivesN = 0;

	constraintdimensions.nargout = 0;
	constraintdimensions.dimension_eq = 0;
	constraintdimensions.dimension_eqgradient = 0;
	constraintdimensions.dimension_ineq = 0;
	constraintdimensions.dimension_ineqgradient = 0;
	constraintdimensions.evals = 0;
	constraintdimensions.size_eqgradientM = 0;
	constraintdimensions.size_eqgradientN = 0;
	constraintdimensions.size_eqM = 0;
	constraintdimensions.size_eqN = 0;
	constraintdimensions.size_ineqgradientM = 0;
	constraintdimensions.size_ineqgradientN = 0;
	constraintdimensions.size_ineqM = 0;
	constraintdimensions.size_ineqN = 0;

	if (functionhandle == NULL || mxIsEmpty(functionhandle)) {
		mexErrMsgIdAndTxt("KSOPT:input", "Objective function must be a function handle or a name of a function.");
		return 1;
	}
	if (x == NULL || mxIsEmpty(x) || !mxIsNumeric(x)) {
		mexErrMsgIdAndTxt("KSOPT:input", "Optimization variable must be numeric.");
		return 1;
	}
	number_variables = mxGetM(x);
	success = ksopt_get_nargin(functionhandle, type, &nargin);
	if (success != 0) {
		mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargin.");
		return 1;
	}
	if (nargin != 1) {
		mexErrMsgIdAndTxt("KSOPT:input", "Function to evaluate must have one input argument.");
		return 1;
	}
	success = ksopt_get_nargout(functionhandle, type, &nargout);
	if (success != 0) {
		mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargout.");
		return 1;
	}
	if (type == KSOPT_OBJECTIVE) {
		// check objective function for validity
		success = eval_objective(functionhandle, x, nargout, &objectivedimensions);
		if (success != 0) {
			mexErrMsgIdAndTxt("KSOPT:input", "Objective function could not be evaluated.");
			return 1;
		}
		checkobjective = true;
		checkgradient = nargout >= 2 || nargout < 0;
		if (nargout == 2) {
			if (objectivedimensions.nargout != 2) {
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargout.");
				return 1;
			}
		}
		if (checkobjective) {
			if (objectivedimensions.dimension_objectives != 2) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a matrix.");
				return 1;
			}
			if (objectivedimensions.size_objectivesM <= 0) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return at least one element.");
				return 1;
			}
			if (objectivedimensions.size_objectivesN != 1) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a column vector.");
				return 1;
			}
			info->number_objectives = objectivedimensions.size_objectivesM;
		}
		if (nargout >= 2 && objectivedimensions.nargout < 2) {
			mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a gradient.");
			return 1;
		}
		if (nargout < 0 && objectivedimensions.nargout <= 1) {
			checkgradient = false;
		}
		if (checkgradient) {
			if (objectivedimensions.dimension_gradient != 2) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a matrix for gradient.");
				return 1;
			}
			if (objectivedimensions.size_gradientN != objectivedimensions.size_objectivesM) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function gradient must have %d columns.", objectivedimensions.size_objectivesM);
				return 1;
			}
			if (objectivedimensions.size_gradientM != number_variables) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function gradient must have %d rows.", number_variables);
				return 1;
			}
			info->hasgradient = true;
		}
		info->number_inputs = 1;
		info->number_outputs = objectivedimensions.nargout;
		info->wasevaluated = (objectivedimensions.evals > 0 ? true : false);
	}
	else if (type == KSOPT_CONSTRAINT) {
		// check constraint function for validity
		success = eval_constraint(functionhandle, x, nargout, &constraintdimensions);
		if (success != 0) {
			mexErrMsgIdAndTxt("KSOPT:input", "Constraint function could not be evaluated.");
			return 1;
		}
		checkconstraint = true;
		checkgradient = true;
		checkconstraintgradient = nargout >= 4 || nargout < 0;
		if (nargout == 2) {
			if (constraintdimensions.nargout != 2) {
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargout.");
				return 1;
			}
		}
		if (nargout == 4) {
			if (constraintdimensions.nargout != 4) {
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when getting nargout.");
				return 1;
			}
		}
		if (checkconstraint) {
			if (constraintdimensions.size_ineqM > 0) {
				if (constraintdimensions.dimension_ineq != 0 && constraintdimensions.dimension_ineq != 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix for inequality constraints.");
					return 1;
				}
				if (constraintdimensions.size_ineqM < 0) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return at least one element for inequality constraints.");
					return 1;
				}
				else {
					if (constraintdimensions.size_ineqN != 1) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a column vector for inequality constraints.");
						return 1;
					}
					info->number_constraints_ineq = constraintdimensions.size_ineqM;
				}
			}
			if (constraintdimensions.size_eqM > 0) {
				if (constraintdimensions.dimension_eq != 0 && constraintdimensions.dimension_eq != 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix for equality constraints.");
					return 1;
				}
				if (constraintdimensions.size_eqM < 0) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return at least one element for equality constraints.");
					return 1;
				}
				else {
					if ((constraintdimensions.size_eqN != 0 && constraintdimensions.size_eqM != 0) && constraintdimensions.size_eqN != 1) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a column vector for equality constraints.");
						return 1;
					}
					info->number_constraints_eq = constraintdimensions.size_eqM;
				}
			}
		}
		if (nargout >= 4 && constraintdimensions.nargout < 4) {
			mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a gradient.");
			return 1;
		}
		if (nargout < 0 && constraintdimensions.nargout <= 2) {
			checkconstraintgradient = false;
		}
		if (checkconstraintgradient) {
			if (info->number_constraints_ineq > 0) {
				if (constraintdimensions.dimension_ineqgradient != 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix for inequality constraint function gradient.");
					return 1;
				}
				if (constraintdimensions.size_ineqgradientN != constraintdimensions.size_ineqM) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function gradient for inequality constraints must have %d rows.", constraintdimensions.size_ineqM);
					return 1;
				}
				if (constraintdimensions.size_ineqgradientM != number_variables) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function gradient for inequality constraints must have %d columns.", number_variables);
					return 1;
				}
				hasconstraintgradient_ineq = true;
			}
			else {
				hasconstraintgradient_ineq = true;
			}
			if (info->number_constraints_eq > 0) {
				if (constraintdimensions.dimension_eqgradient != 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix for equality constraint function gradient.");
					return 1;
				}
				if (constraintdimensions.size_eqgradientN != constraintdimensions.size_eqM) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function gradient for equality constraints must have %d rows.", constraintdimensions.size_eqM);
					return 1;
				}
				if (constraintdimensions.size_eqgradientM != number_variables) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function gradient for equality constraints must have %d columns.", number_variables);
					return 1;
				}
				hasconstraintgradient_eq = true;
			}
			else {
				hasconstraintgradient_eq = true;
			}
		}
		info->hasgradient = hasconstraintgradient_ineq && hasconstraintgradient_eq;
		info->number_inputs = 1;
		info->number_outputs = constraintdimensions.nargout;
		info->wasevaluated = (constraintdimensions.evals > 0 ? true : false);
	}
	else {
		mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
		return 2;
	}
	return 0;
}

// TODO: use mxDestroyArray and mxFree or only one?
#define FREE_CALLMATLABARGSOUTOBJCTIVE(size)\
	for (ii = 0; ii < size; ++ii) {\
		if (evalobjectiveargsout != NULL && evalobjectiveargsout[ii] != NULL) {\
			mxDestroyArray(evalobjectiveargsout[ii]);\
			evalobjectiveargsout[ii] = NULL;\
		}\
	}
static int eval_objective(const mxArray *functionhandle, const mxArray *x, const int nargout, objectiveinfo *info) {
	/*
	*EVAL_OBJECTIVE evaluate objective function
	*	Input:
	*		functionhandle:	function handle to use as objective function
	*		x:				optimization value to test function handle at
	*		nargout:		number of output arguments of function handle
	*	Output:
	*		info:			structure with information about objective function return values
	*		exitflag:		zero in case of success else nonzero
	*/
	int success = 0, ii = 0;
	size_t number_variables = ((x != NULL) ? mxGetM(x) : 0);
	mxArray *evalobjectiveargsin[2], *evalobjectiveargsout[2];
	mxArray *exception = NULL, *exceptionidentifier = NULL;
	mwSize number_dimensions;
	const mwSize* dimensions = NULL;
	char *identifier = NULL;
	char *functionname = NULL;
	info->nargout = 0;
	info->evals = 0;
	info->dimension_objectives = 0;
	info->dimension_gradient = 0;
	info->size_objectivesM = 0;
	info->size_objectivesN = 0;
	info->size_gradientM = 0;
	info->size_gradientN = 0;
	if (!mxIsFunctionHandle(functionhandle) && !mxIsChar(functionhandle)) {
		return 1;
	}
	evalobjectiveargsin[0] = NULL;
	evalobjectiveargsin[1] = NULL;
	evalobjectiveargsout[0] = NULL;
	evalobjectiveargsout[1] = NULL;
	info->evals = info->evals + 1;
	if (nargout >= 2 || nargout < 0) {
		//Call MATLAB for objective function with unknown number of output arguments
		if (mxIsFunctionHandle(functionhandle)) {
			evalobjectiveargsin[0] = (mxArray*) functionhandle;//mxDuplicateArray?
			evalobjectiveargsin[1] = (mxArray*) x;
			exception = mexCallMATLABWithTrap(2, evalobjectiveargsout, 2, evalobjectiveargsin, "feval");
		}
		else if (mxIsChar(functionhandle)) {
			evalobjectiveargsin[0] = (mxArray *) x;
			evalobjectiveargsin[1] = NULL;
			functionname = mxArrayToString(functionhandle);
			exception = mexCallMATLABWithTrap(2, evalobjectiveargsout, 2, evalobjectiveargsin, functionname);
			mxFree(functionname);
			functionname = NULL;
		}
		if (exception != NULL) {
			if (!mxIsClass(exception, "MException")) {
				FREE_CALLMATLABARGSOUTOBJCTIVE(2);
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
				return 1;
			}
			exceptionidentifier = mxGetProperty(exception, 0, "identifier");
			if (exceptionidentifier == NULL) {
				FREE_CALLMATLABARGSOUTOBJCTIVE(2);
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
				return 1;
			}
			identifier = mxArrayToString(exceptionidentifier);
			if (identifier == NULL) {
				FREE_CALLMATLABARGSOUTOBJCTIVE(2);
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
				return 1;
			}
			if (strcmpi(identifier, "MATLAB:maxlhs") == 0 || strcmpi(identifier, "MATLAB:TooManyOutputs") == 0 || strcmpi(identifier, "MATLAB:unassignedOutputs") == 0) {
				if (identifier != NULL) {
					mxFree(identifier);
					identifier = NULL;
				}
				FREE_CALLMATLABARGSOUTOBJCTIVE(2);
				//Call MATLAB for objective function with unknown number of output arguments
				if (mxIsFunctionHandle(functionhandle)) {
					evalobjectiveargsin[0] = (mxArray*) functionhandle;//mxDuplicateArray?
					evalobjectiveargsin[1] = (mxArray*) x;
					exception = mexCallMATLABWithTrap(1, evalobjectiveargsout, 2, evalobjectiveargsin, "feval");
				}
				else if (mxIsChar(functionhandle)) {
					evalobjectiveargsin[0] = (mxArray *) x;
					evalobjectiveargsin[1] = NULL;
					functionname = mxArrayToString(functionhandle);
					exception = mexCallMATLABWithTrap(1, evalobjectiveargsout, 2, evalobjectiveargsin, functionname);
					mxFree(functionname);
					functionname = NULL;
				}
				info->evals = info->evals + 1;
				if (exception != NULL) {
					if (!mxIsClass(exception, "MException")) {
						FREE_CALLMATLABARGSOUTOBJCTIVE(2);
						mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
						return 1;
					}
					exceptionidentifier = mxGetProperty(exception, 0, "identifier");
					if (exceptionidentifier == NULL) {
						FREE_CALLMATLABARGSOUTOBJCTIVE(2);
						mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
						return 1;
					}
					identifier = mxArrayToString(exceptionidentifier);
					if (identifier == NULL) {
						FREE_CALLMATLABARGSOUTOBJCTIVE(2);
						mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
						return 1;
					}
					FREE_CALLMATLABARGSOUTOBJCTIVE(2);
					//Call MATLAB and rethrow exception
					evalobjectiveargsin[0] = (mxArray*) exception;
					evalobjectiveargsin[1] = NULL;
					mxFree(identifier);
					identifier = NULL;
					mexCallMATLAB(0, (mxArray **)NULL, 1, evalobjectiveargsin, "throw");
					mexErrMsgIdAndTxt("KSOPT:input", "An error occured in the objective function.");
					return 1;
				}
				if (evalobjectiveargsout[0] != NULL && !mxIsEmpty(evalobjectiveargsout[0])) {
					number_dimensions = mxGetNumberOfDimensions(evalobjectiveargsout[0]);
					dimensions = mxGetDimensions(evalobjectiveargsout[0]);
					if (number_dimensions > 2) {
						mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a matrix.");
						return 1;
					}
					if (dimensions[0] <= 0) {
						mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return at least one element.");
						return 1;
					}
					if (dimensions[1] != 1) {
						mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a column vector.");
						return 1;
					}
					info->nargout = 1;
					info->dimension_objectives = number_dimensions;
					info->size_objectivesM = dimensions[0];
					info->size_objectivesN = dimensions[1];
				}
				else {
					info->nargout = 0;
				}
			}
			else {
				FREE_CALLMATLABARGSOUTOBJCTIVE(2);
				//Call MATLAB and rethrow exception
				evalobjectiveargsin[0] = (mxArray*) exception;
				evalobjectiveargsin[1] = NULL;
				mexCallMATLAB(0, (mxArray **)NULL, 1, evalobjectiveargsin, "throw");
				mexErrMsgIdAndTxt("KSOPT:input", "An error occured in the objective function.");
				return 1;
			}
			if (identifier != NULL) {
				mxFree(identifier);
				identifier = NULL;
			}
		}
		if (evalobjectiveargsout[0] != NULL && !mxIsEmpty(evalobjectiveargsout[0])) {
			number_dimensions = mxGetNumberOfDimensions(evalobjectiveargsout[0]);
			dimensions = mxGetDimensions(evalobjectiveargsout[0]);
			if (number_dimensions > 2) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a matrix.");
				return 1;
			}
			if (dimensions[0] <= 0) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return at least one element.");
				return 1;
			}
			if (dimensions[1] != 1) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a column vector.");
				return 1;
			}
			info->dimension_objectives = number_dimensions;
			info->size_objectivesM = dimensions[0];
			info->size_objectivesN = dimensions[1];
			if (evalobjectiveargsout[1] != NULL && !mxIsEmpty(evalobjectiveargsout[1])) {
				info->nargout = 2;
				number_dimensions = mxGetNumberOfDimensions(evalobjectiveargsout[1]);
				dimensions = mxGetDimensions(evalobjectiveargsout[1]);
				if (number_dimensions > 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a matrix.");
					return 1;
				}
				if (dimensions[1] != info->size_objectivesM) {
					mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a gradient with %d columns.", info->size_objectivesM);
					return 1;
				}
				if (dimensions[0] != number_variables) {
					mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a gradient with %d rows.", number_variables);
					return 1;
				}
				info->dimension_gradient = number_dimensions;
				info->size_gradientM = dimensions[0];
				info->size_gradientN = dimensions[1];
			}
			else {
				info->nargout = 1 + (evalobjectiveargsout[1] != NULL);
			}
		}
		else {
			info->nargout = 0 + (evalobjectiveargsout[0] != NULL);
		}
		FREE_CALLMATLABARGSOUTOBJCTIVE(2);
	}
	else {
		if (mxIsFunctionHandle(functionhandle)) {
			evalobjectiveargsin[0] = (mxArray*) functionhandle;//mxDuplicateArray?
			evalobjectiveargsin[1] = (mxArray*) x;
			success = mexCallMATLAB(1, evalobjectiveargsout, 2, evalobjectiveargsin, "feval");
		}
		else if (mxIsChar(functionhandle)) {
			evalobjectiveargsin[0] = (mxArray *) x;
			evalobjectiveargsin[1] = NULL;
			functionname = mxArrayToString(functionhandle);
			success = mexCallMATLAB(1, evalobjectiveargsout, 2, evalobjectiveargsin, functionname);
			mxFree(functionname);
			functionname = NULL;
		}
		if (success != 0) {
			FREE_CALLMATLABARGSOUTOBJCTIVE(2);
			mexErrMsgIdAndTxt("KSOPT:input", "An error occured in the objective function.");
			return 1;
		}
		if (evalobjectiveargsout[0] != NULL && !mxIsEmpty(evalobjectiveargsout[0])) {
			number_dimensions = mxGetNumberOfDimensions(evalobjectiveargsout[0]);
			dimensions = mxGetDimensions(evalobjectiveargsout[0]);
			if (number_dimensions > 2) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a matrix.");
				return 1;
			}
			if (dimensions[0] <= 0) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return at least one element.");
				return 1;
			}
			if (dimensions[1] != 1) {
				mexErrMsgIdAndTxt("KSOPT:input", "Objective function must return a column vector.");
				return 1;
			}
			info->dimension_objectives = number_dimensions;
			info->size_objectivesM = dimensions[0];
			info->size_objectivesN = dimensions[1];
			info->nargout = 1;
		}
		else {
			info->nargout = 0 + (evalobjectiveargsout[0] != NULL);
		}
		FREE_CALLMATLABARGSOUTOBJCTIVE(2);
	}
	return 0;
}

// TODO: use mxDestroyArray and mxFree or only one?
#define FREE_CALLMATLABARGSOUTCONSTRAINT(size)\
	for (ii = 0; ii < size; ++ii) {\
		if (evalconstraintargsout != NULL && evalconstraintargsout[ii] != NULL) {\
			mxDestroyArray(evalconstraintargsout[ii]);\
			evalconstraintargsout[ii] = NULL;\
		}\
	}
static int eval_constraint(const mxArray *functionhandle, const mxArray *x, const int nargout, constraintinfo *info) {
	/*
	*EVAL_CONSTRAINT evaluate constraint function
	*	Input:
	*		functionhandle:	function handle to use as constraint function
	*		x:				optimization value to test function handle at
	*		nargout:		number of output arguments of function handle
	*	Output:
	*		info:			structure with information about constraint function return values
	*		exitflag:		zero in case of success else nonzero
	*/
	int success = 0, ii = 0;
	unsigned int maxnargout = 4;
	boolean_T hasarg_ineq = false, hasarg_eq = false, hasarg_ineq_grad = false, hasarg_eq_grad = false;
	boolean_T hasgradient_ineq = false;
	boolean_T hasgradient_eq = false;
	size_t number_variables = ((x != NULL) ? mxGetM(x) : 0);
	mxArray *evalconstraintargsin[2], *evalconstraintargsout[4];
	mxArray *exception = NULL, *exceptionidentifier = NULL;
	mwSize number_dimensions;
	const mwSize *dimensions = NULL;
	char *identifier = NULL;
	char *functionname = NULL;
	info->nargout = 0;
	info->evals = 0;
	info->dimension_ineq = 0;
	info->dimension_ineqgradient = 0;
	info->size_ineqM = 0;
	info->size_ineqN = 0;
	info->size_ineqgradientM = 0;
	info->size_ineqgradientN = 0;
	info->dimension_eq = 0;
	info->dimension_eqgradient = 0;
	info->size_eqM = 0;
	info->size_eqN = 0;
	info->size_eqgradientM = 0;
	info->size_eqgradientN = 0;
	evalconstraintargsout[0] = NULL;
	evalconstraintargsout[1] = NULL;
	evalconstraintargsout[2] = NULL;
	evalconstraintargsout[3] = NULL;
	if (!mxIsFunctionHandle(functionhandle) && !mxIsChar(functionhandle)) {
		return 1;
	}
	if (nargout >= 4 || nargout < 0) {
		//Call MATLAB for constraint function with unknown number of output arguments
		if (mxIsFunctionHandle(functionhandle)) {
			evalconstraintargsin[0] = (mxArray*) functionhandle;//mxDuplicateArray?
			evalconstraintargsin[1] = (mxArray*) x;
			exception = mexCallMATLABWithTrap(4, evalconstraintargsout, 2, evalconstraintargsin, "feval");
		}
		else if (mxIsChar(functionhandle)) {
			evalconstraintargsin[0] = (mxArray *) x;
			evalconstraintargsin[1] = NULL;
			functionname = mxArrayToString(functionhandle);
			exception = mexCallMATLABWithTrap(4, evalconstraintargsout, 2, evalconstraintargsin, functionname);
			mxFree(functionname);
			functionname = NULL;
		}
		if (exception != NULL) {
			FREE_CALLMATLABARGSOUTCONSTRAINT(4);
			if (!mxIsClass(exception, "MException")) {
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
				return 1;
			}
			exceptionidentifier = mxGetProperty(exception, 0, "identifier");
			if (exceptionidentifier == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
				return 1;
			}
			identifier = mxArrayToString(exceptionidentifier);
			if (identifier == NULL) {
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
				return 1;
			}
			if (strcmpi(identifier, "MATLAB:maxlhs") == 0 || strcmpi(identifier, "MATLAB:TooManyOutputs") == 0 || strcmpi(identifier, "MATLAB:unassignedOutputs") == 0) {
				mxFree(identifier);
				identifier = NULL;
				maxnargout = 2;
				// call matlab again
				//Call MATLAB for constraint function with 2 output arguments
				if (mxIsFunctionHandle(functionhandle)) {
					evalconstraintargsin[0] = (mxArray*) functionhandle;//mxDuplicateArray?
					evalconstraintargsin[1] = (mxArray*) x;
					success = mexCallMATLAB(2, evalconstraintargsout, 2, evalconstraintargsin, "feval");
				}
				else if (mxIsChar(functionhandle)) {
					evalconstraintargsin[0] = (mxArray *) x;
					evalconstraintargsin[1] = NULL;
					functionname = mxArrayToString(functionhandle);
					success = mexCallMATLAB(2, evalconstraintargsout, 2, evalconstraintargsin, functionname);
					mxFree(functionname);
					functionname = NULL;
				}
				if (success != 0) {
					FREE_CALLMATLABARGSOUTCONSTRAINT(4);
					mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of input arguments for constraint function.");
					return 1;
				}
			}
			else {
				FREE_CALLMATLABARGSOUTCONSTRAINT(4);
				//Call MATLAB and rethrow exception
				evalconstraintargsin[0] = (mxArray*) exception;
				evalconstraintargsin[1] = NULL;
				mxFree(identifier);
				identifier = NULL;
				mexCallMATLAB(0, (mxArray **)NULL, 1, evalconstraintargsin, "throw");
				mexErrMsgIdAndTxt("KSOPT:input", "An error occured in the objective function.");
				return 1;
			}
		}
		// check for inequality constraints and gradients
		if (evalconstraintargsout[0] != NULL) {
			if (!mxIsEmpty(evalconstraintargsout[0])) {
				if (mxIsClass(evalconstraintargsout[0], "unknown")) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return valid inequality constraints.");
					return 1;
				}
				hasarg_ineq = true;
				number_dimensions = mxGetNumberOfDimensions(evalconstraintargsout[0]);
				dimensions = mxGetDimensions(evalconstraintargsout[0]);
				if (number_dimensions > 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix.");
					return 1;
				}
				info->dimension_ineq = number_dimensions;
				if (dimensions[0] <= 0) {
					info->size_ineqM = 0;
					info->size_ineqN = 0;
				}
				else {
					if (dimensions[1] != 1) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a column vector.");
						return 1;
					}
					info->size_ineqM = dimensions[0];
					info->size_ineqN = dimensions[1];
				}
				if (evalconstraintargsout[2] != NULL) {
					if (mxIsClass(evalconstraintargsout[2], "unknown")) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return valid inequality constraint gradients.");
						return 1;
					}
					hasarg_ineq_grad = true;
					number_dimensions = mxGetNumberOfDimensions(evalconstraintargsout[2]);
					dimensions = mxGetDimensions(evalconstraintargsout[2]);
					if (number_dimensions > 2) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix for inequality constraint gradients.");
						return 1;
					}
					if (dimensions[0] <= 0 || mxIsEmpty(evalconstraintargsout[2])) {
						hasgradient_ineq = mxIsEmpty(evalconstraintargsout[0]);
						info->dimension_ineqgradient = 0;
						info->size_ineqgradientM = 0;
						info->size_ineqgradientN = 0;
					}
					else {
						if (dimensions[1] != info->size_ineqM) {
							mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix with %d columns for inequality constraint gradient.", info->size_ineqM);
							return 1;
						}
						if (dimensions[0] != number_variables) {
							mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix with %d rows for inequality constraint gradient.", number_variables);
							return 1;
						}
						hasgradient_ineq = true;
						info->dimension_ineqgradient = number_dimensions;
						info->size_ineqgradientM = dimensions[0];
						info->size_ineqgradientN = dimensions[1];
					}
				}
				else {
					maxnargout = 2;
				}
			}
			else {
				hasarg_ineq = true;
				hasarg_ineq_grad = evalconstraintargsout[1] != NULL;
				info->dimension_ineq = 0;
				info->size_ineqM = 0;
				info->size_ineqN = 0;
			}
		}
		// check for equality constraints and gradients
		if (evalconstraintargsout[1] != NULL) {
			if (!mxIsEmpty(evalconstraintargsout[1])) {
				if (mxIsClass(evalconstraintargsout[1], "unknown")) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return valid equality constraints.");
					return 1;
				}
				hasarg_eq = true;
				number_dimensions = mxGetNumberOfDimensions(evalconstraintargsout[1]);
				dimensions = mxGetDimensions(evalconstraintargsout[1]);
				if (number_dimensions > 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix for equality constraints.");
					return 1;
				}
				info->dimension_eq = number_dimensions;
				if (dimensions[0] <= 0) {
					info->size_eqM = 0;
					info->size_eqN = 0;
				}
				else {
					if (dimensions[1] != 1) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a column vector for equality constraints.");
						return 1;
					}
					info->size_eqM = dimensions[0];
					info->size_eqN = dimensions[1];
				}
				if (evalconstraintargsout[3] != NULL) {
					if (mxIsClass(evalconstraintargsout[3], "unknown")) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return valid equality constraint gradients.");
						return 1;
					}
					hasarg_eq_grad = true;
					number_dimensions = mxGetNumberOfDimensions(evalconstraintargsout[3]);
					dimensions = mxGetDimensions(evalconstraintargsout[3]);
					if (number_dimensions > 2) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix for equality constraint gradients.");
						return 1;
					}
					if (dimensions[0] <= 0 || mxIsEmpty(evalconstraintargsout[3])) {
						hasgradient_eq = mxIsEmpty(evalconstraintargsout[1]);
						info->dimension_eqgradient = 0;
						info->size_eqgradientM = 0;
						info->size_eqgradientN = 0;
					}
					else {
						if (dimensions[1] != info->size_eqM) {
							mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix with %d columns for equality constraint gradient.", info->size_eqM);
							return 1;
						}
						if (dimensions[0] != number_variables) {
							mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix with %d rows for equality constraint gradient.", number_variables);
							return 1;
						}
						hasgradient_eq = true;
						info->dimension_eqgradient = number_dimensions;
						info->size_eqgradientM = dimensions[0];
						info->size_eqgradientN = dimensions[1];
					}
				}
				else {
					maxnargout = 2;
				}
			}
			else {
				hasarg_eq = true;
				hasarg_eq_grad = evalconstraintargsout[3] != NULL;
				info->dimension_eq = 0;
				info->size_eqM = 0;
				info->size_eqN = 0;
			}
		}
		info->nargout = min(maxnargout, (unsigned int)(hasarg_ineq + hasarg_eq + (hasarg_ineq_grad && (hasgradient_ineq || info->size_ineqM <= 0)) + (hasarg_eq_grad && (hasgradient_eq || info->size_eqM <= 0))));
		FREE_CALLMATLABARGSOUTCONSTRAINT(4);
		if (!hasarg_ineq || !hasarg_eq) {
			mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return inequality and equality constraints.");
			return 1;
		}
		if ((hasgradient_ineq && (!hasarg_ineq || !hasarg_eq)) || (hasgradient_eq && (!hasarg_ineq || !hasarg_eq))) {
			mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return inequality and equality constraints gradients.");
			return 1;
		}
	}
	else {
		maxnargout = 2;
		//Call MATLAB for constraint function with 2 output arguments
		if (mxIsFunctionHandle(functionhandle)) {
			evalconstraintargsin[0] = (mxArray*) functionhandle;//mxDuplicateArray?
			evalconstraintargsin[1] = (mxArray*) x;
			exception = mexCallMATLABWithTrap(2, evalconstraintargsout, 2, evalconstraintargsin, "feval");
		}
		else if (mxIsChar(functionhandle)) {
			evalconstraintargsin[0] = (mxArray *) x;
			evalconstraintargsin[1] = NULL;
			functionname = mxArrayToString(functionhandle);
			exception = mexCallMATLABWithTrap(2, evalconstraintargsout, 2, evalconstraintargsin, functionname);
			mxFree(functionname);
			functionname = NULL;
		}
		if (exception != NULL) {
			if (!mxIsClass(exception, "MException")) {
				FREE_CALLMATLABARGSOUTCONSTRAINT(4);
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
				return 1;
			}
			exceptionidentifier = mxGetProperty(exception, 0, "identifier");
			if (exceptionidentifier == NULL) {
				FREE_CALLMATLABARGSOUTCONSTRAINT(4);
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
				return 1;
			}
			identifier = mxArrayToString(exceptionidentifier);
			if (identifier == NULL) {
				FREE_CALLMATLABARGSOUTCONSTRAINT(4);
				mexErrMsgIdAndTxt("KSOPT:input", "Internal error when checking for gradients.");
				return 1;
			}
			FREE_CALLMATLABARGSOUTCONSTRAINT(4);
			//Call MATLAB and rethrow exception
			evalconstraintargsin[0] = (mxArray*) exception;
			evalconstraintargsin[1] = NULL;
			mxFree(identifier);
			identifier = NULL;
			mexCallMATLAB(0, (mxArray **)NULL, 1, evalconstraintargsin, "throw");
			mexErrMsgIdAndTxt("KSOPT:input", "An error occured in the objective function.");
			return 1;
		}
		if (evalconstraintargsout[0] != NULL) {
			if (!mxIsEmpty(evalconstraintargsout[0])) {
				if (mxIsClass(evalconstraintargsout[0], "unknown")) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return valid inequality constraints.");
					return 1;
				}
				hasarg_ineq = true;
				number_dimensions = mxGetNumberOfDimensions(evalconstraintargsout[0]);
				dimensions = mxGetDimensions(evalconstraintargsout[0]);
				if (number_dimensions > 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix for inequality constraints.");
					return 1;
				}
				info->dimension_ineq = number_dimensions;
				if (dimensions[0] <= 0 || mxIsEmpty(evalconstraintargsout[0])) {
					info->size_ineqM = 0;
					info->size_ineqN = 0;
				}
				else {
					if (dimensions[1] != 1) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a column vector.");
						return 1;
					}
					info->size_ineqM = dimensions[0];
					info->size_ineqN = dimensions[1];
				}
			}
			else {
				hasarg_ineq = true;
				info->dimension_ineq = 0;
				info->size_ineqM = 0;
				info->size_ineqN = 0;
			}
		}
		if (evalconstraintargsout[1] != NULL) {
			if (!mxIsEmpty(evalconstraintargsout[1])) {
				if (mxIsClass(evalconstraintargsout[1], "unknown")) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return valid equality constraints.");
					return 1;
				}
				hasarg_eq = true;
				number_dimensions = mxGetNumberOfDimensions(evalconstraintargsout[1]);
				dimensions = mxGetDimensions(evalconstraintargsout[1]);
				if (number_dimensions > 2) {
					mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a matrix for equality constraints.");
					return 1;
				}
				info->dimension_eq = number_dimensions;
				if (dimensions[0] <= 0 || mxIsEmpty(evalconstraintargsout[1])) {
					info->size_eqM = 0;
					info->size_eqN = 0;
				}
				else {
					if (dimensions[1] != 1) {
						mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return a column vector for equality constraints.");
						return 1;
					}
					info->size_eqM = dimensions[0];
					info->size_eqN = dimensions[1];
				}
			}
			else {
				hasarg_eq = true;
				info->dimension_eq = 0;
				info->size_eqM = 0;
				info->size_eqN = 0;
			}
		}
		info->nargout = min(maxnargout, (unsigned int)(hasarg_ineq + hasarg_eq));
		FREE_CALLMATLABARGSOUTCONSTRAINT(4);
		if (!hasarg_ineq || !hasarg_eq) {
			mexErrMsgIdAndTxt("KSOPT:input", "Constraint function must return inequality and equality constraints.");
			return 1;
		}
	}
	return 0;
}