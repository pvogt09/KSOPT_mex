#include <mex.h>
#include <string.h>
#include "ksopt.h"
#include "ksopt_mex.h"

// TODO: use mxDestroyArray and mxFree or only one?
#define FREE_CALLMATLABARGSOUTNARGIN(size)\
	for (ii = 0; ii < size; ++ii) {\
		if (narginargsout != NULL && narginargsout[ii] != NULL) {\
			mxDestroyArray(narginargsout[ii]);\
			narginargsout[ii] = NULL;\
		}\
	}

int ksopt_get_nargout(const mxArray* functionhandle, const KSOPT_functionType type, int *nargout) {
	/*
	 *KSOPT_GET_NARGOUT get number of output arguments of a matlab function handle
	 *	Input:
	 *		functionhandle:	function handle to get number of output arguments for
	 *		type:			type of function the function handle should be used for (objective function, constraint function, iteration function)
	 *	Output:
	 *		nargout:		number of output arguments of the function handle
	 *		exitflag:		zero in case of success else nonzero
	 */
	int success = 0, ii = 0;
	mxArray *narginargsin[1], *narginargsout[1];
	if (functionhandle == NULL || mxIsEmpty(functionhandle)) {
		*nargout = 0;
		return 1;
	}
	if (mxIsFunctionHandle(functionhandle)) {
		narginargsout[0] = NULL;
		narginargsin[0] = (mxArray*) functionhandle;
		//Call MATLAB for nargin
		success = mexCallMATLAB(1, narginargsout, 1, narginargsin, "nargout");
		if (success) {
			*nargout = 0;
			switch (type) {
				case KSOPT_OBJECTIVE:
					FREE_CALLMATLABARGSOUTNARGIN(1);
					mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of output arguments for objective function.");
					return 1;
				case KSOPT_CONSTRAINT:
					FREE_CALLMATLABARGSOUTNARGIN(1);
					mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of output arguments for constraint function.");
					return 1;
				case KSOPT_ITER:
					FREE_CALLMATLABARGSOUTNARGIN(1);
					mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of output arguments for iteration function.");
					return 1;
				default:
					FREE_CALLMATLABARGSOUTNARGIN(1);
					mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of output arguments for function.");
					return 1;
			}
		}
		if (narginargsout[0] != NULL && !mxIsEmpty(narginargsout[0]) && mxIsScalar(narginargsout[0])) {
			*nargout = (int) mxGetScalar(narginargsout[0]);
		}
		else {
			*nargout = 0;
			FREE_CALLMATLABARGSOUTNARGIN(1);
			mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of output arguments for function.");
			return 1;
		}
		FREE_CALLMATLABARGSOUTNARGIN(1);
	}
	else if (mxIsChar(functionhandle)) {
		narginargsout[0] = NULL;
		narginargsin[0] = (mxArray*) functionhandle;
		//Call MATLAB for nargin
		success = mexCallMATLAB(1, narginargsout, 1, narginargsin, "nargout");
		if (success) {
			*nargout = 0;
			switch (type) {
				case KSOPT_OBJECTIVE:
					FREE_CALLMATLABARGSOUTNARGIN(1);
					mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of output arguments for objective function.");
					return 1;
				case KSOPT_CONSTRAINT:
					FREE_CALLMATLABARGSOUTNARGIN(1);
					mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of output arguments for constraint function.");
					return 1;
				case KSOPT_ITER:
					FREE_CALLMATLABARGSOUTNARGIN(1);
					mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of output arguments for iteration function.");
					return 1;
				default:
					FREE_CALLMATLABARGSOUTNARGIN(1);
					mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of output arguments for function.");
					return 1;
			}
		}
		if (narginargsout[0] != NULL && !mxIsEmpty(narginargsout[0]) && mxIsScalar(narginargsout[0])) {
			*nargout = (int) mxGetScalar(narginargsout[0]);
		}
		else {
			*nargout = 0;
			FREE_CALLMATLABARGSOUTNARGIN(1);
			mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of output arguments for function.");
			return 1;
		}
		FREE_CALLMATLABARGSOUTNARGIN(1);
	}
	else {
		*nargout = 0;
		mexErrMsgIdAndTxt("KSOPT:input", "Could not get number of output arguments for function.");
		return 1;
	}
	return 0;
}