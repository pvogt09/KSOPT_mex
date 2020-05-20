/* ksqmin.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksqmin (a1,a2,a3,a4,alpha,f1,f2,f3,f4,fun) >*/
/* Subroutine */ int ksqmin_(doublereal *a1, doublereal *a2, doublereal *a3, 
	doublereal *a4, doublereal *alpha, doublereal *f1, doublereal *f2, 
	doublereal *f3, doublereal *f4, doublereal *fun)
{
/*<       implicit double precision (a-h,o-z) >*/

/*          routine to place alpha and fun in the appropriate */
/*          position relative to the three or four previously */
/*          defined points a1,f1 through a4,f4, dropping the */
/*          most distant point from the current minimum. */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 26 April 1989 */

/*<       if (a4 .gt. 0.0) go to 20 >*/
    if (*a4 > 0.f) {
	goto L20;
    }

/*<       if (alpha .lt. a2) go to 10 >*/
    if (*alpha < *a2) {
	goto L10;
    }
/*<       a4 = a3 >*/
    *a4 = *a3;
/*<       f4 = f3 >*/
    *f4 = *f3;
/*<       a3 = alpha >*/
    *a3 = *alpha;
/*<       f3 = fun >*/
    *f3 = *fun;
/*<       go to 50 >*/
    goto L50;
/*<    10 continue >*/
L10:
/*<       a4 = a3 >*/
    *a4 = *a3;
/*<       f4 = f3 >*/
    *f4 = *f3;
/*<       a3 = a2 >*/
    *a3 = *a2;
/*<       f3 = f2 >*/
    *f3 = *f2;
/*<       a2 = alpha >*/
    *a2 = *alpha;
/*<       f2 = fun >*/
    *f2 = *fun;
/*<       go to 50 >*/
    goto L50;
/*<    20 continue >*/
L20:
/*<       if (alpha .le. a2) go to 10 >*/
    if (*alpha <= *a2) {
	goto L10;
    }
/*<       if (alpha .lt. a3) go to 30 >*/
    if (*alpha < *a3) {
	goto L30;
    }
/*<       a1 = a2 >*/
    *a1 = *a2;
/*<       f1 = f2 >*/
    *f1 = *f2;
/*<       a2 = a3 >*/
    *a2 = *a3;
/*<       f2 = f3 >*/
    *f2 = *f3;
/*<       a3 = alpha >*/
    *a3 = *alpha;
/*<       f3 = fun >*/
    *f3 = *fun;
/*<       go to 50 >*/
    goto L50;
/*<    30 continue >*/
L30:
/*<       if ( (alpha - a1) .gt. (a4 - alpha) ) go to 40 >*/
    if (*alpha - *a1 > *a4 - *alpha) {
	goto L40;
    }
/*<       a4 = a3 >*/
    *a4 = *a3;
/*<       f4 = f3 >*/
    *f4 = *f3;
/*<       a3 = alpha >*/
    *a3 = *alpha;
/*<       f3 = fun >*/
    *f3 = *fun;
/*<       go to 50 >*/
    goto L50;
/*<    40 continue >*/
L40:
/*<       a1 = a2 >*/
    *a1 = *a2;
/*<       f1 = f2 >*/
    *f1 = *f2;
/*<       a2 = alpha >*/
    *a2 = *alpha;
/*<       f2 = fun >*/
    *f2 = *fun;

/*<    50 continue >*/
L50:
/*<       return >*/
    return 0;
/*<       end >*/
} /* ksqmin_ */

