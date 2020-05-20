#include <mex.h>
#include <string.h>
#include "ksopt.h"
#include "ksopt_mex.h"

int ksopt_sparse2full(const mxArray *sparse, double *full, const size_t m, const size_t n, const boolean_T throwerror) {
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
	int ii = 0, jj = 0, kk = 0;
	double *numericSparseDouble = NULL;
	mwIndex *numericSparseJC = NULL, *numericSparseIR = NULL;
	if (full == NULL) {
		return 1;
	}
	if (sparse == NULL) {
		return 1;
	}
	if (mxIsSparse(sparse)) {
		if (mxGetNumberOfDimensions(sparse) != 2) {
			if (throwerror) {
				mexErrMsgIdAndTxt("KSOPT:input", "Dimension mismatch.");
			}
			return 1;
		}
		if (mxGetM(sparse) != m) {
			if (throwerror) {
				mexErrMsgIdAndTxt("KSOPT:input", "Dimension mismatch.");
			}
			return 1;
		}
		if (mxGetN(sparse) != n) {
			if (throwerror) {
				mexErrMsgIdAndTxt("KSOPT:input", "Dimension mismatch.");
			}
			return 1;
		}
		numericSparseDouble = mxGetPr(sparse);
		if (numericSparseDouble == NULL) {
			if (throwerror) {
				mexErrMsgIdAndTxt("KSOPT:input", "Numeric data for sparse matrix is invalid.");
			}
			return 1;
		}
		numericSparseIR = mxGetIr(sparse);
		if (numericSparseIR == NULL) {
			if (throwerror) {
				mexErrMsgIdAndTxt("KSOPT:input", "Numeric data for sparse matrix is invalid.");
			}
			return 1;
		}
		numericSparseJC = mxGetJc(sparse);
		if (numericSparseJC == NULL) {
			if (throwerror) {
				mexErrMsgIdAndTxt("KSOPT:input", "Numeric data for sparse matrix is invalid.");
			}
			return 1;
		}
		jj = 0;
		for (ii = 0; ii < n; ++ii) {
			if (numericSparseJC[ii] == numericSparseJC[ii + 1]) {
				continue;
			}
			for (kk = numericSparseJC[ii]; kk < numericSparseJC[ii + 1]; ++kk) {
				if (numericSparseIR[kk] + m*ii >= m*n) {
					return 3;
				}
				if (jj > numericSparseJC[n]) {
					return 4;
				}
				full[numericSparseIR[kk] + m*ii] = numericSparseDouble[jj++];
			}
		}
		numericSparseDouble = NULL;
		numericSparseJC = NULL;
		numericSparseIR = NULL;
		return 0;
	}
	else {
		return 2;
	}
	return 0;
}