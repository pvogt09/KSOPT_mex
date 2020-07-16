/* kshess.f -- translated by f2c (version 20100827).
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
/* Subroutine */ int kshess_(doublereal *hess, doublereal *obj, doublereal *
	fscale, doublereal *offset, doublereal *df, doublereal *g, doublereal
	*dg, doublereal *rho, integer *ndv, integer *ncon, integer *nobj,
	doublereal *temp1, doublereal *temp2, integer *nodim, integer *ncdim)
{
	/* Initialized data */

	static doublereal toler = -40.;

	/* System generated locals */
	integer df_dim1, df_offset, dg_dim1, dg_offset, i__1, i__2, i__3;

	/* Builtin functions */
	double exp(doublereal);

	/* Local variables */
	static integer i__, j, m, n, ng, kk;
	static doublereal val, sum1, sum2, sum3, sum4, gmax, d2grad;

/*<       implicit double precision (a-h,o-z) >*/
/*<       dimension hess(*),obj(*),fscale(*),offset(*),df(nodim,*) >*/
/*<       dimension g(*),dg(ncdim,*),temp1(*),temp2(2,*) >*/
/*<       data toler /-40.0/ >*/
	/* Parameter adjustments */
	--hess;
	--obj;
	--fscale;
	--offset;
	--g;
	--temp1;
	temp2 -= 3;
	df_dim1 = *nodim;
	df_offset = 1 + df_dim1;
	df -= df_offset;
	dg_dim1 = *ncdim;
	dg_offset = 1 + dg_dim1;
	dg -= dg_offset;

	/* Function Body */

/*          routine to compute approximate hessian matrix from */
/*          first order gradient information */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 19 July 1996 */

/*<       do 10 i = 1,nobj >*/
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

/*<       kk = 0 >*/
	kk = 0;
/*<       do 140 m = 1,ndv >*/
	i__1 = *ndv;
	for (m = 1; m <= i__1; ++m) {

/*<         do 40 i = 1,nobj >*/
	i__2 = *nobj;
	for (i__ = 1; i__ <= i__2; ++i__) {
/*<           temp2(1,i) = df(i,m) / fscale(i) >*/
		temp2[(i__ << 1) + 1] = df[i__ + m * df_dim1] / fscale[i__];
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
/*<           temp2(1,j) = dg(i,m) >*/
		temp2[(j << 1) + 1] = dg[i__ + m * dg_dim1];
/*<    50   continue >*/
/* L50: */
	}
/*<    60   continue >*/
L60:

/*<         do 130 n = 1,m >*/
	i__2 = m;
	for (n = 1; n <= i__2; ++n) {

/*<           do 70 i = 1,nobj >*/
		i__3 = *nobj;
		for (i__ = 1; i__ <= i__3; ++i__) {
/*<             temp2(2,i) = df(i,n) / fscale(i) >*/
		temp2[(i__ << 1) + 2] = df[i__ + n * df_dim1] / fscale[i__];
/*<    70     continue >*/
/* L70: */
		}
/*<           j = nobj >*/
		j = *nobj;
/*<           if (ncon .le. 0) go to 90 >*/
		if (*ncon <= 0) {
		goto L90;
		}
/*<           do 80 i = 1,ncon >*/
		i__3 = *ncon;
		for (i__ = 1; i__ <= i__3; ++i__) {
/*<             j = j + 1 >*/
		++j;
/*<             temp2(2,j) = dg(i,n) >*/
		temp2[(j << 1) + 2] = dg[i__ + n * dg_dim1];
/*<    80     continue >*/
/* L80: */
		}
/*<    90     continue >*/
L90:

/*<           sum1 = 0.0 >*/
		sum1 = 0.f;
/*<           sum2 = 0.0 >*/
		sum2 = 0.f;
/*<           sum3 = 0.0 >*/
		sum3 = 0.f;
/*<           sum4 = 0.0 >*/
		sum4 = 0.f;
/*<           ng = nobj + ncon >*/
		ng = *nobj + *ncon;
/*<           gmax = temp1(1) >*/
		gmax = temp1[1];
/*<           if (ng .lt. 2) go to 110 >*/
		if (ng < 2) {
		goto L110;
		}
/*<           do 100 i = 2,ng >*/
		i__3 = ng;
		for (i__ = 2; i__ <= i__3; ++i__) {
/*<             if (temp1(i) .gt. gmax) gmax = temp1(i) >*/
		if (temp1[i__] > gmax) {
			gmax = temp1[i__];
		}
/*<   100     continue >*/
/* L100: */
		}
/*<   110     continue >*/
L110:
/*<           do 120 i = 1,ng >*/
		i__3 = ng;
		for (i__ = 1; i__ <= i__3; ++i__) {
/*<             d2grad = 0.0 >*/
		d2grad = 0.f;
/*<             if (n .eq. m) d2grad = 1.0 >*/
		if (n == m) {
			d2grad = 1.f;
		}
/*<             val = rho * (temp1(i) - gmax) >*/
		val = *rho * (temp1[i__] - gmax);
/*<             if (val .lt. toler) go to 120 >*/
		if (val < toler) {
			goto L120;
		}
/*<             sum1 = sum1 + exp(val) >*/
		sum1 += exp(val);
/*<             sum2 = sum2 + exp(val) * temp2(1,i) >*/
		sum2 += exp(val) * temp2[(i__ << 1) + 1];
/*<             sum3 = sum3 + exp(val) * rho * temp2(2,i) >*/
		sum3 += exp(val) * *rho * temp2[(i__ << 1) + 2];
/*<    >*/
		sum4 += exp(val) * (*rho * temp2[(i__ << 1) + 1] * temp2[(i__
			<< 1) + 2] + d2grad);
/*<   120     continue >*/
L120:
		;
		}

/*<           kk = kk + 1 >*/
		++kk;
/*<    >*/
		if (n == m) {
		hess[kk] = sum4 / sum1 - sum2 * sum3 / (sum1 * sum1);
		}
/*<           if (n .ne. m) hess(kk) = 0.0 >*/
		if (n != m) {
		hess[kk] = 0.f;
		}

/*<   130   continue >*/
/* L130: */
	}
/*<   140 continue >*/
/* L140: */
	}

/*<       return >*/
	return 0;
/*<       end >*/
} /* kshess_ */