/* kshmul.f -- translated by f2c (version 20100827).
	You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/*<       subroutine kshmul (a,b,x,nrow) >*/
/* Subroutine */ int kshmul_(doublereal *a, doublereal *b, doublereal *x,
	integer *nrow)
{
	/* System generated locals */
	integer i__1, i__2;

	/* Local variables */
	static integer i__, j, k;

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension a(*),b(*),x(*) >*/

/*          routine to perform matrix multiplication a * b = x */
/*          for a triangular matrix a */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 17 July 1996 */

/*<       k = 0 >*/
	/* Parameter adjustments */
	--x;
	--b;
	--a;

	/* Function Body */
	k = 0;
/*<       do 10 i = 1,nrow >*/
	i__1 = *nrow;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         x(i) = 0.0 >*/
	x[i__] = 0.f;
/*<    10 continue >*/
/* L10: */
	}
/*<       do 20 i = 1,nrow >*/
	i__1 = *nrow;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<       do 20 j = 1,i >*/
	i__2 = i__;
	for (j = 1; j <= i__2; ++j) {
/*<         k = k + 1 >*/
		++k;
/*<         x(i) = x(i) + a(k) * b(j) >*/
		x[i__] += a[k] * b[j];
/*<         if (j .lt. i) x(j) = x(j) + a(k) * b(i) >*/
		if (j < i__) {
		x[j] += a[k] * b[i__];
		}
/*<    20 continue >*/
/* L20: */
	}
	}

/*<       return >*/
	return 0;
/*<       end >*/
} /* kshmul_ */