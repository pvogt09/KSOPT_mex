/* ksscal.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksscal (x,x0,xlb,xub,scale,ndv,nside,nscale) >*/
/* Subroutine */ int ksscal_(doublereal *x, doublereal *x0, doublereal *xlb,
	doublereal *xub, doublereal *scale, integer *ndv, integer *nside,
	integer *nscale)
{
	/* System generated locals */
	integer scale_dim1, scale_offset, i__1;
	doublereal d__1;

	/* Local variables */
	static integer i__;
	static doublereal xx, xx0, sold, snew;

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension x(*),x0(*),xlb(*),xub(*),scale(ndv,2) >*/

/*          routine to compute new scaling vector and re-scale */
/*          design variables and lower and upper bounds */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 17 July 1996 */

/*<       do 10 i = 1,ndv >*/
	/* Parameter adjustments */
	--x;
	--x0;
	--xlb;
	--xub;
	scale_dim1 = *ndv;
	scale_offset = 1 + scale_dim1;
	scale -= scale_offset;

	/* Function Body */
	i__1 = *ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         xx    = x(i) >*/
	xx = x[i__];
/*<         xx0   = x0(i) >*/
	xx0 = x0[i__];
/*<         sold  = scale(i,1) >*/
	sold = scale[i__ + scale_dim1];
/*<         snew  = sold >*/
	snew = sold;
/*<         if (nscale .gt. 0) snew = abs(xx * sold) >*/
	if (*nscale > 0) {
		snew = (d__1 = xx * sold, abs(d__1));
	}
/*<         if (nscale .lt. 0) snew = abs(scale(i,2)) >*/
	if (*nscale < 0) {
		snew = (d__1 = scale[i__ + (scale_dim1 << 1)], abs(d__1));
	}
/*<         if (snew .lt. 1.0e-08) snew = 1.0 >*/
	if (snew < 1e-8f) {
		snew = 1.f;
	}
/*<         scale(i,1) = snew >*/
	scale[i__ + scale_dim1] = snew;
/*<         x(i)    = xx  * sold / snew >*/
	x[i__] = xx * sold / snew;
/*<         x0(i)   = xx0 * sold / snew >*/
	x0[i__] = xx0 * sold / snew;
/*<         if (nside .le. 0) go to 10 >*/
	if (*nside <= 0) {
		goto L10;
	}
/*<         xlb(i)  = xlb(i) * sold / snew >*/
	xlb[i__] = xlb[i__] * sold / snew;
/*<         xub(i)  = xub(i) * sold / snew >*/
	xub[i__] = xub[i__] * sold / snew;
/*<    10 continue >*/
L10:
	;
	}

/*<       return >*/
	return 0;
/*<       end >*/
} /* ksscal_ */