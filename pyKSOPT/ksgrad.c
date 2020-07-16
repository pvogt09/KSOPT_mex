/* ksgrad.f -- translated by f2c (version 20100827).
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

/*<    >*/
/* Subroutine */ int ksgrad_(integer *inext, doublereal *x, doublereal *x0,
	doublereal *xlb, doublereal *xub, doublereal *g, doublereal *g0,
	doublereal *obj, doublereal *obj0, doublereal *df, doublereal *dg,
	doublereal *scale, doublereal *delx, integer *ndv, integer *ncon,
	integer *nobj, integer *nside, doublereal *fdelt, doublereal *fdmin,
	integer *nodim, integer *ncdim)
{
	/* System generated locals */
	integer df_dim1, df_offset, dg_dim1, dg_offset, scale_dim1, scale_offset,
		i__1;
	doublereal d__1;

	/* Local variables */
	static integer i__;
	static doublereal delx1, xhere, xtest, delmin;

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension x(*),x0(*),xlb(*),xub(*),g(*),g0(*) >*/
/*<       dimension obj(*),obj0(*),df(nodim,*),dg(ncdim,*),scale(ndv,2) >*/

/*          routine to determine df and dg by finite differences */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 19 July 1996 */

/*<       if (inext .gt. 0) go to 40 >*/
	/* Parameter adjustments */
	--x;
	--x0;
	--xlb;
	--xub;
	--g;
	--g0;
	--obj;
	--obj0;
	scale_dim1 = *ndv;
	scale_offset = 1 + scale_dim1;
	scale -= scale_offset;
	df_dim1 = *nodim;
	df_offset = 1 + df_dim1;
	df -= df_offset;
	dg_dim1 = *ncdim;
	dg_offset = 1 + dg_dim1;
	dg -= dg_offset;

	/* Function Body */
	if (*inext > 0) {
	goto L40;
	}

/*          initialize obj0, x0, and g0 */

/*<       do 10 i = 1,nobj >*/
	i__1 = *nobj;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         obj0(i) = obj(i) >*/
	obj0[i__] = obj[i__];
/*<    10 continue >*/
/* L10: */
	}

/*<       do 20 i = 1,ndv >*/
	i__1 = *ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         x0(i) = x(i) >*/
	x0[i__] = x[i__];
/*<    20 continue >*/
/* L20: */
	}
/*<       if (ncon .le. 0) go to 40 >*/
	if (*ncon <= 0) {
	goto L40;
	}
/*<       do 30 i = 1,ncon >*/
	i__1 = *ncon;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         g0(i) = g(i) >*/
	g0[i__] = g[i__];
/*<    30 continue >*/
/* L30: */
	}

/*          calculate gradients and restore design variables */

/*          note - these gradients are already scaled by the */
/*                 design variable scale factors */

/*<    40 continue >*/
L40:
/*<       if (inext .eq. 0) go to 80 >*/
	if (*inext == 0) {
	goto L80;
	}

/*<       do 50 i = 1,nobj >*/
	i__1 = *nobj;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         df(i,inext) = (obj(i) - obj0(i)) / delx >*/
	df[i__ + *inext * df_dim1] = (obj[i__] - obj0[i__]) / *delx;
/*<    50 continue >*/
/* L50: */
	}

/*<       if (ncon .le. 0) go to 70 >*/
	if (*ncon <= 0) {
	goto L70;
	}
/*<       do 60 i = 1,ncon >*/
	i__1 = *ncon;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         dg(i,inext) = (g(i) - g0(i)) / delx >*/
	dg[i__ + *inext * dg_dim1] = (g[i__] - g0[i__]) / *delx;
/*<    60 continue >*/
/* L60: */
	}
/*<    70 continue >*/
L70:
/*<       x(inext) = x0(inext) >*/
	x[*inext] = x0[*inext];

/*          increment design variable inext and get obj and g */

/*<    80 continue >*/
L80:
/*<       inext = inext + 1 >*/
	++(*inext);
/*<       if (inext .gt. ndv) go to 100 >*/
	if (*inext > *ndv) {
	goto L100;
	}
/*<       xhere = x0(inext) >*/
	xhere = x0[*inext];
/*<       delmin = fdmin / scale(inext,1) >*/
	delmin = *fdmin / scale[*inext + scale_dim1];
/*<       delx = abs(xhere * fdelt) >*/
	*delx = (d__1 = xhere * *fdelt, abs(d__1));
/*<       if (delx .lt. delmin) delx = delmin >*/
	if (*delx < delmin) {
	*delx = delmin;
	}

/*<       if (nside .le. 0) go to 90 >*/
	if (*nside <= 0) {
	goto L90;
	}

/*<       xtest = xhere + delx >*/
	xtest = xhere + *delx;
/*<       if (xtest .le. xub(inext)) go to 90 >*/
	if (xtest <= xub[*inext]) {
	goto L90;
	}

/*          adjust delta-x for upper bound */
/*<       delx1 = delx >*/
	delx1 = *delx;
/*<       delx = abs(xub(inext) - xhere) >*/
	*delx = (d__1 = xub[*inext] - xhere, abs(d__1));
/*<       if (delx .ge. delmin) go to 90 >*/
	if (*delx >= delmin) {
	goto L90;
	}

/*          adjust delta-x for lower bound */

/*<       delx = -delx1 >*/
	*delx = -delx1;
/*<       xtest = xhere + delx >*/
	xtest = xhere + *delx;
/*<       if (xtest .lt. xlb(inext)) delx = xlb(inext) - xhere >*/
	if (xtest < xlb[*inext]) {
	*delx = xlb[*inext] - xhere;
	}

/*<    90 continue >*/
L90:
/*<       x(inext) = x0(inext) + delx >*/
	x[*inext] = x0[*inext] + *delx;
/*<       go to 110 >*/
	goto L110;

/*          reset inext to zero when finished */

/*<   100 continue >*/
L100:
/*<       inext = 0 >*/
	*inext = 0;
/*<   110 continue >*/
L110:

/*<       return >*/
	return 0;
/*<       end >*/
} /* ksgrad_ */