/* kscube.f -- translated by f2c (version 20100827).
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

/*<       subroutine kscube (a1,a2,a3,a4,f1,f2,f3,f4,astar,fstar) >*/
/* Subroutine */ int kscube_(doublereal *a1, doublereal *a2, doublereal *a3, 
	doublereal *a4, doublereal *f1, doublereal *f2, doublereal *f3, 
	doublereal *f4, doublereal *astar, doublereal *fstar)
{
    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal d1, d2, b3, b2, b1, b0, q1, q2, q3, q4, q5, q6, bb, a1c,
	     a2c, a3c, a4c, as2, as3;

/*<       implicit double precision (a-h,o-z) >*/

/*          routine to compute the minimum of a cubic */
/*          curve through points a1,f1 a2,f2 a3,f3 and a4,f4. */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 27 April 1989 */

/*<       a1c = a1 * a1 * a1 >*/
    a1c = *a1 * *a1 * *a1;
/*<       a2c = a2 * a2 * a2 >*/
    a2c = *a2 * *a2 * *a2;
/*<       a3c = a3 * a3 * a3 >*/
    a3c = *a3 * *a3 * *a3;
/*<       a4c = a4 * a4 * a4 >*/
    a4c = *a4 * *a4 * *a4;

/*<       q1 = a3c * (a2 - a1) - a2c * (a3 - a1) + a1c * (a3 - a2) >*/
    q1 = a3c * (*a2 - *a1) - a2c * (*a3 - *a1) + a1c * (*a3 - *a2);
/*<       q2 = a4c * (a2 - a1) - a2c * (a4 - a1) + a1c * (a4 - a2) >*/
    q2 = a4c * (*a2 - *a1) - a2c * (*a4 - *a1) + a1c * (*a4 - *a2);
/*<       q3 = (a3 - a2) * (a2 - a1) * (a3 - a1) >*/
    q3 = (*a3 - *a2) * (*a2 - *a1) * (*a3 - *a1);
/*<       q4 = (a4 - a2) * (a2 - a1) * (a4 - a1) >*/
    q4 = (*a4 - *a2) * (*a2 - *a1) * (*a4 - *a1);
/*<       q5 = f3 * (a2 - a1) - f2 * (a3 - a1) + f1 * (a3 - a2) >*/
    q5 = *f3 * (*a2 - *a1) - *f2 * (*a3 - *a1) + *f1 * (*a3 - *a2);
/*<       q6 = f4 * (a2 - a1) - f2 * (a4 - a1) + f1 * (a4 - a2) >*/
    q6 = *f4 * (*a2 - *a1) - *f2 * (*a4 - *a1) + *f1 * (*a4 - *a2);

/*<       d1 = q2 * q3 - q1 * q4 >*/
    d1 = q2 * q3 - q1 * q4;
/*<       d2 = a2 - a1 >*/
    d2 = *a2 - *a1;
/*<       if (d1 .lt. 1.0e-12 .or. d2 .lt. 1.0e-12) go to 10 >*/
    if (d1 < 1e-12f || d2 < 1e-12f) {
	goto L10;
    }

/*<       b3 = (q3 * q6 - q4 * q5) / d1 >*/
    b3 = (q3 * q6 - q4 * q5) / d1;
/*<       b2 = (q5 - b3 * q1) / q3 >*/
    b2 = (q5 - b3 * q1) / q3;
/*<       b1 = (f2 - f1) / d2 - b3 * (a2c - a1c) / d2 - b2 * (a1 + a2) >*/
    b1 = (*f2 - *f1) / d2 - b3 * (a2c - a1c) / d2 - b2 * (*a1 + *a2);
/*<       b0 = f1 - b1 * a1 - b2 * a1 * a1 - b3 * a1c >*/
    b0 = *f1 - b1 * *a1 - b2 * *a1 * *a1 - b3 * a1c;

/*<       bb = b2 * b2 - 3.0 * b1 * b3 >*/
    bb = b2 * b2 - b1 * 3.f * b3;
/*<       if (bb .lt. 0.0 .or. abs(b3) .lt. 1.0e-12) go to 10 >*/
    if (bb < 0.f || abs(b3) < 1e-12f) {
	goto L10;
    }
/*<       astar = (-b2 + sqrt(bb)) / (3.0 * b3) >*/
    *astar = (-b2 + sqrt(bb)) / (b3 * 3.f);
/*<       if (astar .lt. a1 .or. astar .gt. a4) go to 10 >*/
    if (*astar < *a1 || *astar > *a4) {
	goto L10;
    }
/*<       as2 = astar * astar >*/
    as2 = *astar * *astar;
/*<       as3 = astar * as2 >*/
    as3 = *astar * as2;
/*<       fstar = b0 + b1 * astar + b2 * as2 + b3 * as3 >*/
    *fstar = b0 + b1 * *astar + b2 * as2 + b3 * as3;
/*<       go to 40 >*/
    goto L40;

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
/*<       if (f4 .ge. fstar) go to 40 >*/
    if (*f4 >= *fstar) {
	goto L40;
    }
/*<       astar = a4 >*/
    *astar = *a4;
/*<       fstar = f4 >*/
    *fstar = *f4;

/*<    40 continue >*/
L40:
/*<       return >*/
    return 0;
/*<       end >*/
} /* kscube_ */

