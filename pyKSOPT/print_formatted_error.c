#include <mex.h>
#include <stdio.h>
#include <stdarg.h>

char* print_formatted_error(size_t *buffersize, const char *format, ...) {
	/*
	 *PRINT_FORMATTED_ERROR allocate memory for formatted string with variable number of replacements by printf
	 *	Input:
	 *		buffersize:		length of formatted text (without terminating \0)
	 *		format:			format string
	 *		varargin:		replacements for placeholders
	 *	Output:
	 *		buffer:			string containing formatted text
	 */
	char *buffer = NULL;
	*buffersize = 0;
	{// WORKAROUND: Visual Studio 2012 does not like va_list args; in the parent scope and crashes with error C2275
		va_list args;
		va_start(args, format);
		#if defined(_MSC_VER)
			*buffersize = _vscprintf(format, args);
		#else
			*buffersize = vsnprintf(NULL, 0, format, args);
		#endif
		buffer = (char*) mxCalloc(*buffersize + 1, sizeof(char));
		if (buffer == NULL || *buffersize < 0) {
			va_end(args);
			return NULL;
		}
		#if defined(_MSC_VER)
			_vsnprintf(buffer, *buffersize, format, args);
		#else
			vsnprintf(buffer, *buffersize, format, args);
		#endif
		va_end(args);
	}
	if (buffer != NULL) {
		// _vsnprintf does not add \0
		buffer[*buffersize] = '\0';
	}
	return buffer;
}