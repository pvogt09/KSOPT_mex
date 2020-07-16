/* ksquad.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksquad (a1,a2,a3,f1,f2,f3,astar,fstar) >*/
/* Subroutine */ int ksquad_(doublereal *a1, doublereal *a2, doublereal *a3,
	doublereal *f1, doublereal *f2, doublereal *f3, doublereal *astar,
	doublereal *fstar)
{
	static doublereal a, b, c__, d__, a21, a31, f21, f31, a3121, a21s, a31s;

/*<       implicit double precision (a-h,o-z) >*/

/*          routine to compute the minimum of a quadratic */
/*          curve through points a1,f1 a2,f2 and a3,f3. */
/*          f2 must be less than f1 and f3. */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 27 April 1989 */

/*<       f21 = f2 - f1 >*/
	f21 = *f2 - *f1;
/*<       a21 = a2 - a1 >*/
	a21 = *a2 - *a1;
/*<       if (a21 .lt. 1.0e-12) go to 10 >*/
	if (a21 < 1e-12f) {
	goto L10;
	}
/*<       f31 = f3 - f1 >*/
	f31 = *f3 - *f1;
/*<       a31 = a3 - a1 >*/
	a31 = *a3 - *a1;
/*<       a3121 = a31 / a21 >*/
	a3121 = a31 / a21;
/*<       a21s = a2 * a2 - a1 * a1 >*/
	a21s = *a2 * *a2 - *a1 * *a1;
/*<       a31s = a3 * a3 - a1 * a1 >*/
	a31s = *a3 * *a3 - *a1 * *a1;

/*<       d = a31s - a21s * a3121 >*/
	d__ = a31s - a21s * a3121;
/*<       if (d .lt. 1.0e-12) go to 10 >*/
	if (d__ < 1e-12f) {
	goto L10;
	}

/*<       a = (f31 - f21 * a3121) / d >*/
	a = (f31 - f21 * a3121) / d__;
/*<       b = (f21 - a * a21s) / a21 >*/
	b = (f21 - a * a21s) / a21;
/*<       c = f1 - b * a1 - a * a1 * a1 >*/
	c__ = *f1 - b * *a1 - a * *a1 * *a1;

/*<       if (a .lt. 1.0e-12) go to 10 >*/
	if (a < 1e-12f) {
	goto L10;
	}
/*<       astar = -b / (2.0 * a) >*/
	*astar = -b / (a * 2.f);
/*<       if (astar .lt. a1 .or. astar .gt. a3) go to 10 >*/
	if (*astar < *a1 || *astar > *a3) {
	goto L10;
	}
/*<       fstar = a * astar * astar + b * astar + c >*/
	*fstar = a * *astar * *astar + b * *astar + c__;
/*<       go to 30 >*/
	goto L30;

/*<    10 continue >*/
L10:
/*<       astar = a1 >*/
	*astar = *a1;
/*<       fstar = f1 >*/
	*fstar = *f1;
/*<       if (f2 .ge. fstar) go to 20 >*/
	if (*f2 >= *fstar) {
	goto L20;
	}
/*<       astar = a2 >*/
	*astar = *a2;
/*<       fstar = f2 >*/
	*fstar = *f2;
/*<    20 continue >*/
L20:
/*<       if (f3 .ge. fstar) go to 30 >*/
	if (*f3 >= *fstar) {
	goto L30;
	}
/*<       astar = a3 >*/
	*astar = *a3;
/*<       fstar = f3 >*/
	*fstar = *f3;

/*<    30 continue >*/
L30:
/*<       return >*/
	return 0;
/*<       end >*/
} /* ksquad_ */