/* ksd.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksd (df,g,dgdx,ng,rho) >*/
/* Subroutine */ int ksd_(doublereal *df, doublereal *g, doublereal *dgdx,
	integer *ng, doublereal *rho)
{
	/* Initialized data */

	static doublereal toler = -40.;

	/* System generated locals */
	integer i__1;

	/* Builtin functions */
	double exp(doublereal);

	/* Local variables */
	static integer i__;
	static doublereal val, sum1, sum2, gmax;

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension g(ng),dgdx(ng) >*/
/*<       data toler /-40.0/ >*/
	/* Parameter adjustments */
	--dgdx;
	--g;

	/* Function Body */

/*          routine to compute partial k-s w.r.t. x (type 2). */
/*          partial of g(i) w.r.t. x (e.g.  by finite difference) */
/*          must be supplied in array dgdx(i). */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification -  9 July 1988 */

/*<       sum1 = 0.0 >*/
	sum1 = 0.f;
/*<       sum2 = 0.0 >*/
	sum2 = 0.f;
/*<       gmax = g(1) >*/
	gmax = g[1];
/*<       if (ng .lt. 2) go to 30 >*/
	if (*ng < 2) {
	goto L30;
	}
/*<       do 10 i = 2,ng >*/
	i__1 = *ng;
	for (i__ = 2; i__ <= i__1; ++i__) {
/*<         if (g(i) .gt. gmax) gmax = g(i) >*/
	if (g[i__] > gmax) {
		gmax = g[i__];
	}
/*<    10 continue >*/
/* L10: */
	}
/*<       do 20 i = 1,ng >*/
	i__1 = *ng;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         val = rho * (g(i) - gmax) >*/
	val = *rho * (g[i__] - gmax);
/*<         if (val .lt. toler) go to 20 >*/
	if (val < toler) {
		goto L20;
	}
/*<         sum1 = sum1 + exp(val) * dgdx(i) >*/
	sum1 += exp(val) * dgdx[i__];
/*<         sum2 = sum2 + exp(val) >*/
	sum2 += exp(val);
/*<    20 continue >*/
L20:
	;
	}
/*<    30 continue >*/
L30:
/*<       df = dgdx(1) >*/
	*df = dgdx[1];
/*<       if (ng .gt. 1) df = sum1 / sum2 >*/
	if (*ng > 1) {
	*df = sum1 / sum2;
	}

/*<       return >*/
	return 0;
/*<       end >*/
} /* ksd_ */