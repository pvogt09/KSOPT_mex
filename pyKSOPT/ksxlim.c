/* ksxlim.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksxlim (x,xlb,xub,ndv,nside) >*/
/* Subroutine */ int ksxlim_(doublereal *x, doublereal *xlb, doublereal *xub, 
	integer *ndv, integer *nside)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;
    static doublereal xl, xu, xx;

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension x(*),xlb(*),xub(*) >*/

/*          routine to insure x-vector does not violate */
/*          upper or lower bounds */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 17 July 1996 */

/*<       if (nside .eq. 0) go to 20 >*/
    /* Parameter adjustments */
    --xub;
    --xlb;
    --x;

    /* Function Body */
    if (*nside == 0) {
	goto L20;
    }
/*<       do 10 i = 1,ndv >*/
    i__1 = *ndv;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         xx  = x(i) >*/
	xx = x[i__];
/*<         xl  = xlb(i) >*/
	xl = xlb[i__];
/*<         xu  = xub(i) >*/
	xu = xub[i__];
/*<         if (xx .lt. xl) xx = xl >*/
	if (xx < xl) {
	    xx = xl;
	}
/*<         if (xx .gt. xu) xx = xu >*/
	if (xx > xu) {
	    xx = xu;
	}
/*<         x(i) = xx >*/
	x[i__] = xx;
/*<    10 continue >*/
/* L10: */
    }
/*<    20 continue >*/
L20:

/*<       return >*/
    return 0;
/*<       end >*/
} /* ksxlim_ */

