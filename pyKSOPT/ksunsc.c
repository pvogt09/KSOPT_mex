/* ksunsc.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksunsc (x,sx,scale,ndv) >*/
/* Subroutine */ int ksunsc_(doublereal *x, doublereal *sx, doublereal *scale,
	integer *ndv)
{
	/* System generated locals */
	integer scale_dim1, scale_offset, i__1;

	/* Local variables */
	static integer i__;
	static doublereal ss;

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension x(*),sx(*),scale(ndv,2) >*/

/*          routine to un-scale design variables before returning */
/*          to the real world */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 17 July 1996 */

/*<       do 10 i = 1,ndv >*/
	/* Parameter adjustments */
	--x;
	--sx;
	scale_dim1 = *ndv;
	scale_offset = 1 + scale_dim1;
	scale -= scale_offset;

	/* Function Body */
	i__1 = *ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         ss = scale(i,1) >*/
	ss = scale[i__ + scale_dim1];
/*<         x(i) = sx(i) * ss >*/
	x[i__] = sx[i__] * ss;
/*<    10 continue >*/
/* L10: */
	}

/*<       return >*/
	return 0;
/*<       end >*/
} /* ksunsc_ */