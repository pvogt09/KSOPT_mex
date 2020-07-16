/* ksdfun.f -- translated by f2c (version 20100827).
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
/* Subroutine */ int ksdfun_(doublereal *dfun, doublereal *obj, doublereal *
	fscale, doublereal *offset, doublereal *df, doublereal *g, doublereal
	*dg, doublereal *rho, integer *ndv, integer *ncon, integer *nobj,
	doublereal *temp1, doublereal *temp2, integer *nodim, integer *ncdim)
{
	/* System generated locals */
	integer df_dim1, df_offset, dg_dim1, dg_offset, i__1, i__2;

	/* Local variables */
	static integer i__, j, n;
	extern /* Subroutine */ int ksd_(doublereal *, doublereal *, doublereal *,
		integer *, doublereal *);

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension dfun(*),obj(*),fscale(*),offset(*),df(nodim,*) >*/
/*<       dimension g(*),dg(ncdim,*),temp1(*),temp2(*) >*/

/*          routine to compute gradients of function to be minimized */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 19 July 1996 */

/*<       do 10 i = 1,nobj >*/
	/* Parameter adjustments */
	--dfun;
	--obj;
	--fscale;
	--offset;
	--g;
	--temp1;
	--temp2;
	df_dim1 = *nodim;
	df_offset = 1 + df_dim1;
	df -= df_offset;
	dg_dim1 = *ncdim;
	dg_offset = 1 + dg_dim1;
	dg -= dg_offset;

	/* Function Body */
	i__1 = *nobj;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         temp1(i) = obj(i) / fscale(i) + offset(i) >*/
	temp1[i__] = obj[i__] / fscale[i__] + offset[i__];
/*<    10 continue >*/
/* L10: */
	}
/*<       j = nobj >*/
	j = *nobj;
/*<       if (ncon .le. 0) go to 30 >*/
	if (*ncon <= 0) {
	goto L30;
	}
/*<       do 20 i = 1,ncon >*/
	i__1 = *ncon;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         j = j + 1 >*/
	++j;
/*<         temp1(j) = g(i) >*/
	temp1[j] = g[i__];
/*<    20 continue >*/
/* L20: */
	}
/*<    30 continue >*/
L30:

/*<       do 70 n = 1,ndv >*/
	i__1 = *ndv;
	for (n = 1; n <= i__1; ++n) {
/*<         do 40 i = 1,nobj >*/
	i__2 = *nobj;
	for (i__ = 1; i__ <= i__2; ++i__) {
/*<           temp2(i) = df(i,n) / fscale(i) >*/
		temp2[i__] = df[i__ + n * df_dim1] / fscale[i__];
/*<    40   continue >*/
/* L40: */
	}
/*<         j = nobj >*/
	j = *nobj;
/*<         if (ncon .le. 0) go to 60 >*/
	if (*ncon <= 0) {
		goto L60;
	}
/*<         do 50 i = 1,ncon >*/
	i__2 = *ncon;
	for (i__ = 1; i__ <= i__2; ++i__) {
/*<           j = j + 1 >*/
		++j;
/*<           temp2(j) = dg(i,n) >*/
		temp2[j] = dg[i__ + n * dg_dim1];
/*<    50   continue >*/
/* L50: */
	}
/*<    60   continue >*/
L60:

/*<       call ksd (dfun(n),temp1,temp2,j,rho) >*/
	ksd_(&dfun[n], &temp1[1], &temp2[1], &j, rho);

/*<    70 continue >*/
/* L70: */
	}

/*<       return >*/
	return 0;
/*<       end >*/
} /* ksdfun_ */