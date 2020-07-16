
#include "ksopt.h"
#include <string.h>
#define KSOPT_ERRORIDnumber 32
extern const int KSOPT_ErrorID[] = {
	// memory errors
	90,
	// input errors
	100,
	101,
	102,
	103,
	//
	200,
	201,
	//
	300,
	//
	400,
	401,
	402,
	403,
	404,
	405,
	406,
	// option errors
	1100,
	//
	1200,
	1201,
	1202,
	1203,
	1204,
	1205,
	//
	1300,
	//
	1400,
	//
	1500,
	//
	1600,
	//
	1700,
	//
	1800,
	1801,
	//
	1900,
	//
	2000,
	//
	2100,
	//
	2200
};
extern const char *KSOPT_Error_Message[] = {
	// memory errors
	"Out of Memory.",
	// input errors
	"number of variable must not be smaller than 1.",
	"number of objective functions must not be smaller than 1.",
	"Objective function must not be empty.",
	"Gradient function must not be empty.",
	//
	"KSOPT could not be initialized.",
	"KSOPT could not be initialized because of too small work array.",
	//
	"KSOPT did not return information.",
	//
	"Result structure must not be empty.",
	"Result structure field for optimal solution must have as many elements as optimization variables.",
	"Result structure field for optimal objective must have as many elements as objective functions.",
	"Result structure field for optimal constraint must have as many elements as constraints.",
	"Result structure field for optimal solution is not initialized.",
	"Result structure field for optimal objective is not initialized.",
	"Result structure field for optimal constraint is not initialized.",
	// option errors
	"Maximum number of function evaluations must be greater than 0.",
	//
	"Maximum number of iterations must be greater than 0.",
	"Print value must be greater than 0.",
	"Ones digit of print value for iteration printing must be 0, 1, 2, 3 or 4.",
	"Ones digit of print value for gradient printing must be 0 or 1.",
	"Ones digit of print value for search direction printing must be 0, 1 or 2.",
	"Print value must not have more than 3 digits.",
	//
	"Gradient indicator must be 'true' or 'false'.",
	//
	"Absolute function tolerance must not be smaller than 0.",
	//
	"Relative function tolerance must not be smaller than 0.",
	//
	"Finite difference step size must not be smaller than 0.",
	//
	"Minimum finite difference step size must not be smaller than 0.",
	//
	"User supplied scaling vector must have as many elements as optimization variables.",
	"User supplied scaling vector must not be empty.",
	//
	"Maximum execution time must not be smaller than 0.",
	//
	"Minimum scaling factor for rho must not be smaller than 0.",
	//
	"Maximum scaling factor for rho must not be smaller than 0.",
	//
	"Increment factor for rho must not be smaller than 0."
};

int ksgeterrormessage(const int id, char* errormessage, const int buffersize) {
	/*
	*KSGETERRORMESSAGE get error message from error id for KSOPT
	*	Input:
	*		id:				ID of error to convert to message
	*		buffersize:		length of string
	*	Output:
	*		errormessage:	string to copy error message to
	*		exitflag:		nonzero in case of failure, zero in case of success
	*/
	unsigned int ii = 0;
	unsigned short int found = 0;
	char* copyptr = NULL;
	if (errormessage == NULL || buffersize < 1) {
		return 1;
	}
	for (ii = 0; ii < KSOPT_ERRORIDnumber; ++ii) {
		if (id == KSOPT_ErrorID[ii]) {
			found = 1;
			copyptr = strncpy(errormessage, KSOPT_Error_Message[ii], buffersize - 1);
			if (copyptr == NULL) {
				return 1;
			}
		}
	}
	if (!found) {
		*errormessage = '\0';
	}
	errormessage[buffersize - 1] = '\0';
	return 0;
}