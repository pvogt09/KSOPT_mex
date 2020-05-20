/* ksales.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksales (a1,a2,a3,f1,f2,f3,alim,alpha) >*/
/* Subroutine */ int ksales_(doublereal *a1, doublereal *a2, doublereal *a3, 
	doublereal *f1, doublereal *f2, doublereal *f3, doublereal *alim, 
	doublereal *alpha)
{
    /* System generated locals */
    doublereal d__1;

    /* Local variables */
    static doublereal a, b, x1, y1, x2, y2, alpmin, alpmax;

/*<       implicit double precision (a-h,o-z) >*/

/*          routine to estimate the next one-dimensional */
/*          step size alpha, based on the slopes of the */
/*          previous two trial points. */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 21 April 1989 */

/*<       alpha = 0.0 >*/
    *alpha = 0.f;
/*<       alpmin = 2.0 * a3 - a2 >*/
    alpmin = *a3 * 2.f - *a2;
/*<       alpmax = 3.0 * a3 >*/
    alpmax = *a3 * 3.f;

/*<       if (abs(a2 - a1) .lt. 1.0e-08) go to 10 >*/
    if ((d__1 = *a2 - *a1, abs(d__1)) < 1e-8f) {
	goto L10;
    }
/*<       x1 = a2 >*/
    x1 = *a2;
/*<       y1 = (f2 - f1) / (a2 - a1) >*/
    y1 = (*f2 - *f1) / (*a2 - *a1);

/*<       if (abs(a3 - a2) .lt. 1.0e-08) go to 10 >*/
    if ((d__1 = *a3 - *a2, abs(d__1)) < 1e-8f) {
	goto L10;
    }
/*<       x2 = a3 >*/
    x2 = *a3;
/*<       y2 = (f3 - f2) / (a3 - a2) >*/
    y2 = (*f3 - *f2) / (*a3 - *a2);

/*<       a = (y2 - y1) / (x2 - x1) >*/
    a = (y2 - y1) / (x2 - x1);
/*<       b = y1 - a * x1 >*/
    b = y1 - a * x1;

/*<       if (abs(a) .ge. 1.0e-08) alpha = -b / a >*/
    if (abs(a) >= 1e-8f) {
	*alpha = -b / a;
    }
/*<    10 continue >*/
L10:
/*<       if (alpha .le. 0.0   ) alpha = alpmax >*/
    if (*alpha <= 0.f) {
	*alpha = alpmax;
    }
/*<       if (alpha .lt. alpmin) alpha = alpmin >*/
    if (*alpha < alpmin) {
	*alpha = alpmin;
    }
/*<       if (alpha .gt. alpmax) alpha = alpmax >*/
    if (*alpha > alpmax) {
	*alpha = alpmax;
    }
/*<       if (alim .gt. 0.0 .and. alpha .ge. alim) alpha = 0.5 * (a3 + alim) >*/
    if (*alim > 0.f && *alpha >= *alim) {
	*alpha = (*a3 + *alim) * .5f;
    }

/*<       return >*/
    return 0;
/*<       end >*/
} /* ksales_ */

