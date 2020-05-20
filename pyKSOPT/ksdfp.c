/* ksdfp.f -- translated by f2c (version 20100827).
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
/* Subroutine */ int ksdfp_(doublereal *x, doublereal *side, doublereal *act, 
	doublereal *dfun, integer *ndv, doublereal *s, doublereal *slope, 
	doublereal *y, doublereal *p, doublereal *h__, doublereal *hess, 
	integer *isdflg)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer i__, j, k, nh;
    static doublereal si, tau, smax, sigma;
    extern /* Subroutine */ int kshmul_(doublereal *, doublereal *, 
	    doublereal *, integer *), ksvprd_(doublereal *, doublereal *, 
	    doublereal *, integer *);

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension x(*),side(*),act(*),dfun(*),s(*),y(*),p(*),h(*) >*/
/*<       dimension hess(*) >*/

/*          routine to compute search direction using the */
/*          davidon-fletcher-powell method */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 19 July 1996 */

/*<       if (isdflg .gt. 0) go to 40 >*/
    /* Parameter adjustments */
    --hess;
    --h__;
    --p;
    --y;
    --s;
    --dfun;
    --act;
    --side;
    --x;

    /* Function Body */
    if (*isdflg > 0) {
	goto L40;
    }

/*          reset approximate hessian matrix to initial condition */

/*<    10 continue >*/
L10:

/*          set h matrix to hess matrix */

/*<       isdflg = 0 >*/
    *isdflg = 0;
/*<       nh = ndv * (ndv + 1) / 2 >*/
    nh = *ndv * (*ndv + 1) / 2;
/*<       do 20 i = 1,nh >*/
    i__1 = nh;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         h(i) = hess(i) >*/
	h__[i__] = hess[i__];
/*<    20 continue >*/
/* L20: */
    }
/*<       do 30 i = 1,ndv >*/
    i__1 = *ndv;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         act(i) = side(i) >*/
	act[i__] = side[i__];
/*<    30 continue >*/
/* L30: */
    }
/*<       go to 80 >*/
    goto L80;

/*          dfp algorithm */

/*<    40 continue >*/
L40:

/*          check for validity of hessian matrix */
/*          in the case of side constraints */

/*<       do 50 i = 1,ndv >*/
    i__1 = *ndv;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         if (side(i) .ne. act(i)) go to 10 >*/
	if (side[i__] != act[i__]) {
	    goto L10;
	}
/*<    50 continue >*/
/* L50: */
    }

/*          compute vectors p and y */

/*<       do 60 i = 1,ndv >*/
    i__1 = *ndv;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         y(i) = dfun(i) - y(i) >*/
	y[i__] = dfun[i__] - y[i__];
/*<         p(i) = x(i) - p(i) >*/
	p[i__] = x[i__] - p[i__];
/*<    60 continue >*/
/* L60: */
    }

/*          compute update to hessian matrix */

/*<       call kshmul (h,y,s,ndv) >*/
    kshmul_(&h__[1], &y[1], &s[1], ndv);
/*<       call ksvprd (p,y,sigma,ndv) >*/
    ksvprd_(&p[1], &y[1], &sigma, ndv);
/*<       call ksvprd (y,s,tau,ndv) >*/
    ksvprd_(&y[1], &s[1], &tau, ndv);
/*<       if (abs(sigma) .lt. 1.0e-08) sigma = 1.0e-08 >*/
    if (abs(sigma) < 1e-8f) {
	sigma = 1e-8f;
    }
/*<       if (abs(tau) .lt. 1.0e-08) tau = 1.0e-08 >*/
    if (abs(tau) < 1e-8f) {
	tau = 1e-8f;
    }
/*<       sigma = 1.0 / sigma >*/
    sigma = 1.f / sigma;
/*<       tau = 1.0 / tau >*/
    tau = 1.f / tau;

/*<       k = 0 >*/
    k = 0;
/*<       do 70 i = 1,ndv >*/
    i__1 = *ndv;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<       do 70 j = 1,i >*/
	i__2 = i__;
	for (j = 1; j <= i__2; ++j) {
/*<         k = k + 1 >*/
	    ++k;
/*<         h(k) = h(k) + sigma * p(i) * p(j) - tau * s(i) * s(j) >*/
	    h__[k] = h__[k] + sigma * p[i__] * p[j] - tau * s[i__] * s[j];
/*<    70 continue >*/
/* L70: */
	}
    }

/*<    80 continue >*/
L80:
/*<       call kshmul (h,dfun,s,ndv) >*/
    kshmul_(&h__[1], &dfun[1], &s[1], ndv);

/*          negate s-vector */

/*<       smax = 1.0e-08 >*/
    smax = 1e-8f;
/*<       do 90 i = 1,ndv >*/
    i__2 = *ndv;
    for (i__ = 1; i__ <= i__2; ++i__) {
/*<         s(i) = -s(i) >*/
	s[i__] = -s[i__];
/*<         si = abs(s(i)) >*/
	si = (d__1 = s[i__], abs(d__1));
/*<         if (si .gt. smax) smax = si >*/
	if (si > smax) {
	    smax = si;
	}
/*<    90 continue >*/
/* L90: */
    }

/*          normalize search direction to unit maximum */
/*          and save previous gradients in y-vector */
/*          and previous design variables in p-vector */

/*<       do 100 i = 1,ndv >*/
    i__2 = *ndv;
    for (i__ = 1; i__ <= i__2; ++i__) {
/*<         s(i) = s(i) / smax >*/
	s[i__] /= smax;
/*<         y(i) = dfun(i) >*/
	y[i__] = dfun[i__];
/*<         p(i) = x(i) >*/
	p[i__] = x[i__];
/*<   100 continue >*/
/* L100: */
    }

/*          compute slope */

/*<       call ksvprd (s,dfun,slope,ndv) >*/
    ksvprd_(&s[1], &dfun[1], slope, ndv);
/*<       if (slope .gt. 0.0 .and. isdflg .gt. 0) go to 10 >*/
    if (*slope > 0.f && *isdflg > 0) {
	goto L10;
    }
/*<       if (slope .gt. 0.0) slope = 0.0 >*/
    if (*slope > 0.f) {
	*slope = 0.f;
    }

/*<       return >*/
    return 0;
/*<       end >*/
} /* ksdfp_ */

