/* ksside.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksside (x,xlb,xub,side,dfun,ndv,nside) >*/
/* Subroutine */ int ksside_(doublereal *x, doublereal *xlb, doublereal *xub, 
	doublereal *side, doublereal *dfun, integer *ndv, integer *nside)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Local variables */
    static integer i__;
    static doublereal xl, xu, xx;

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension x(*),xlb(*),xub(*),side(*),dfun(*) >*/

/*          routine to compute a vector of flags side(i),i=1,ndv */
/*          side(i) =  0  -- d.v. i is not at a side constraint */
/*                   = -1  -- d.v. i is at a lower bound */
/*                   = +1  -- d.v. i is at an upper bound */
/*                   = 999 -- d.v. i is at both upper and lower bounds */

/*          also zero components of dfun that would violate */
/*          side constraints */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 19 July 1996 */

/*<       do 10 i = 1,ndv >*/
    /* Parameter adjustments */
    --dfun;
    --side;
    --xub;
    --xlb;
    --x;

    /* Function Body */
    i__1 = *ndv;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         side(i) = 0.0 >*/
	side[i__] = 0.f;
/*<         if (nside .eq. 0) go to 10 >*/
	if (*nside == 0) {
	    goto L10;
	}

/*<         xl = abs(xlb(i)) >*/
	xl = (d__1 = xlb[i__], abs(d__1));
/*<         if (xl .lt. 1.0) xl = 1.0 >*/
	if (xl < 1.f) {
	    xl = 1.f;
	}
/*<         xx = (xlb(i) - x(i)) / xl >*/
	xx = (xlb[i__] - x[i__]) / xl;
/*<         if (xx .ge. -1.0e-6) side(i) = -1.0 >*/
	if (xx >= -1e-6f) {
	    side[i__] = -1.f;
	}
/*<         if (side(i) .lt. 0.0 .and. dfun(i) .gt. 0.0) dfun(i) = 0.0 >*/
	if (side[i__] < 0.f && dfun[i__] > 0.f) {
	    dfun[i__] = 0.f;
	}

/*<         xu = abs(xub(i)) >*/
	xu = (d__1 = xub[i__], abs(d__1));
/*<         if (xu .lt. 1.0) xu = 1.0 >*/
	if (xu < 1.f) {
	    xu = 1.f;
	}
/*<         xx = (x(i) - xub(i)) / xu >*/
	xx = (x[i__] - xub[i__]) / xu;
/*<         if (xx .ge. -1.0e-6 .and. side(i) .ne. 0.0) side(i) = 999.0 >*/
	if (xx >= -1e-6f && side[i__] != 0.f) {
	    side[i__] = 999.f;
	}
/*<         if (xx .ge. -1.0e-6 .and. side(i) .eq. 0.0) side(i) = 1.0 >*/
	if (xx >= -1e-6f && side[i__] == 0.f) {
	    side[i__] = 1.f;
	}
/*<         if (side(i) .gt. 0.0 .and. dfun(i) .lt. 0.0) dfun(i) = 0.0 >*/
	if (side[i__] > 0.f && dfun[i__] < 0.f) {
	    dfun[i__] = 0.f;
	}
/*<    10 continue >*/
L10:
	;
    }

/*<       return >*/
    return 0;
/*<       end >*/
} /* ksside_ */

