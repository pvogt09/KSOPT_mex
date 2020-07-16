/* ksfun.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksfun (fun,obj,g,rho,fscale,offset,ncon,nobj,temp) >*/
/* Subroutine */ int ksfun_(doublereal *fun, doublereal *obj, doublereal *g,
	doublereal *rho, doublereal *fscale, doublereal *offset, integer *
	ncon, integer *nobj, doublereal *temp)
{
	/* System generated locals */
	integer i__1;

	/* Local variables */
	static integer i__, j;
	extern /* Subroutine */ int ks_(doublereal *, doublereal *, integer *,
		doublereal *);

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension obj(*),g(*),fscale(*),offset(*),temp(*) >*/

/*          routine to compute unconstrained function to be minimized */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 17 July 1996 */

/*<       do 10 i = 1,nobj >*/
	/* Parameter adjustments */
	--temp;
	--offset;
	--fscale;
	--g;
	--obj;

	/* Function Body */
	i__1 = *nobj;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         temp(i) = obj(i) / fscale(i) + offset(i) >*/
	temp[i__] = obj[i__] / fscale[i__] + offset[i__];
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
/*<         temp(j) = g(i) >*/
	temp[j] = g[i__];
/*<    20 continue >*/
/* L20: */
	}
/*<    30 continue >*/
L30:

/*<       call ks (fun,temp,j,rho) >*/
	ks_(fun, &temp[1], &j, rho);

/*<       return >*/
	return 0;
/*<       end >*/
} /* ksfun_ */