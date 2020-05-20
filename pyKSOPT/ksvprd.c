/* ksvprd.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksvprd (x,y,prod,nrow) >*/
/* Subroutine */ int ksvprd_(doublereal *x, doublereal *y, doublereal *prod, 
	integer *nrow)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension x(*),y(*) >*/

/*          routine to multiply vectors x and y */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 17 July 1996 */

/*<       prod = 0.0 >*/
    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    *prod = 0.f;
/*<       do 10 i = 1,nrow >*/
    i__1 = *nrow;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         prod = prod + x(i) * y(i) >*/
	*prod += x[i__] * y[i__];
/*<    10 continue >*/
/* L10: */
    }

/*<       return >*/
    return 0;
/*<       end >*/
} /* ksvprd_ */

