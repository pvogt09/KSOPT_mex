/* ksando.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksando (obj,g,fscale,offset,ncon,nobj) >*/
/* Subroutine */ int ksando_(doublereal *obj, doublereal *g, doublereal *
	fscale, doublereal *offset, integer *ncon, integer *nobj)
{
    /* Initialized data */

    static doublereal eps = 1e-6;

    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Local variables */
    static integer i__, j;
    static doublereal foff, fscl, gmax;

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension obj(*),g(*),fscale(*),offset(*) >*/
/*<       data eps/1.0e-6/ >*/
    /* Parameter adjustments */
    --offset;
    --fscale;
    --g;
    --obj;

    /* Function Body */

/*          routine to compute scale and offset for obj */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 17 July 1996 */

/*<       if (ncon .le. 0) go to 20 >*/
    if (*ncon <= 0) {
	goto L20;
    }
/*<       gmax = g(1) >*/
    gmax = g[1];
/*<       if (ncon .eq. 1) go to 20 >*/
    if (*ncon == 1) {
	goto L20;
    }
/*<       do 10 i = 2,ncon >*/
    i__1 = *ncon;
    for (i__ = 2; i__ <= i__1; ++i__) {
/*<         if (g(i) .gt. gmax) gmax = g(i) >*/
	if (g[i__] > gmax) {
	    gmax = g[i__];
	}
/*<    10 continue >*/
/* L10: */
    }

/*<    20 continue >*/
L20:
/*<       do 30 j = 1,nobj >*/
    i__1 = *nobj;
    for (j = 1; j <= i__1; ++j) {
/*<         fscl = abs(obj(j)) >*/
	fscl = (d__1 = obj[j], abs(d__1));
/*<         foff = abs(obj(j)) >*/
	foff = (d__1 = obj[j], abs(d__1));
/*<         if (fscl .lt. 1.0) fscl = 1.0 >*/
	if (fscl < 1.f) {
	    fscl = 1.f;
	}
/*<         if (foff .gt. 1.0) foff = 1.0 >*/
	if (foff > 1.f) {
	    foff = 1.f;
	}
/*<         fscale(j) = fscl >*/
	fscale[j] = fscl;
/*<         offset(j) = 0.0 >*/
	offset[j] = 0.f;

/*          compute offset if ncon > 0 */

/*<         if (ncon .le. 0) go to 30 >*/
	if (*ncon <= 0) {
	    goto L30;
	}
/*<         if (obj(j) .ge. 0.0) offset(j) = -foff - gmax - eps >*/
	if (obj[j] >= 0.f) {
	    offset[j] = -foff - gmax - eps;
	}
/*<         if (obj(j) .lt. 0.0) offset(j) =  foff - gmax + eps >*/
	if (obj[j] < 0.f) {
	    offset[j] = foff - gmax + eps;
	}
/*<    30 continue >*/
L30:
	;
    }

/*<       return >*/
    return 0;
/*<       end >*/
} /* ksando_ */

