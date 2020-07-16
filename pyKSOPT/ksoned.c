/* ksoned.f -- translated by f2c (version 20100827).
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

/* Table of constant values */

static integer c__1 = 1;

/*<    >*/
/* Subroutine */ int ksoned_(integer *inext, integer *jtry, doublereal *x,
	doublereal *x0, doublereal *xlb, doublereal *xub, doublereal *fun,
	doublereal *fun0, doublereal *s, doublereal *slope, doublereal *alpha,
	doublereal *alpmax, integer *ndv, doublereal *a1, doublereal *a2,
	doublereal *a3, doublereal *a4, doublereal *f1, doublereal *f2,
	doublereal *f3, doublereal *f4, doublereal *alim, doublereal *atest,
	doublereal *ftest, integer *nside, integer *limit, integer *nunit,
	integer *iprnt3, doublereal *scale, doublereal *temp, integer *isdflg)
{
	/* Initialized data */

	static char ip1[4] = "(   ";
	static doublereal rtoler = 1e-5;
	static doublereal atoler = .001;
	static integer jmax = 20;

	/* Format strings */
	static char fmt_1010[] = "(/\002       one-dimensional minimization star"
		"ted\002//\002       initial alpha estimate = \002,e12.6/\002    "
		"   maximum alpha allowed  = \002,e12.6/\002       initial k-s fu"
		"nction   = \002,e12.6/)";
	static char fmt_1050[] = "(/\002       current alpha = \002,e12.6,\002  "
		" function = \002,e12.6)";
	static char fmt_1020[] = "(\002       inext = \002,i2,2x,\002 jtry = "
		"\002,i2,2x,\002 proposed alpha = \002,e12.6)";
	static char fmt_1080[] = "(\002       inext = \002,i2,2x,\002 jtry = "
		"\002,i2,2x,\002 final alpha = \002,e12.6)";
	static char fmt_1060[] = "(/\002       upper or lower bounds limit has b"
		"een reached\002)";
	static char fmt_1030[] = "(/\002       proposed design variable set\002/)"
		;
	static char fmt_1070[] = "(/\002       final design variable set\002/)";
	static char fmt_1000[] = "(3(3x,a1,i5,\002)=\002,e14.7))";
	static char fmt_1040[] = "(/)";

	/* System generated locals */
	integer scale_dim1, scale_offset, i__1;
	doublereal d__1;

	/* Builtin functions */
	integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

	/* Local variables */
	static integer i__;
	static doublereal aa, f12, f32, ff, si, adiv, fdiv, atol, ftol, astar,
		fstar, btest, xtest, otest1, xtest1;
	extern /* Subroutine */ int kscube_(doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *), ksales_(
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *), ksquad_(
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *), ksqmin_(
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *), ksunsc_(doublereal *, doublereal *,
		doublereal *, integer *);

	/* Fortran I/O blocks */
	static cilist io___11 = { 0, 0, 0, fmt_1010, 0 };
	static cilist io___12 = { 0, 0, 0, fmt_1050, 0 };
	static cilist io___23 = { 0, 0, 0, fmt_1020, 0 };
	static cilist io___24 = { 0, 0, 0, fmt_1080, 0 };
	static cilist io___25 = { 0, 0, 0, fmt_1060, 0 };
	static cilist io___26 = { 0, 0, 0, fmt_1030, 0 };
	static cilist io___27 = { 0, 0, 0, fmt_1070, 0 };
	static cilist io___28 = { 0, 0, 0, fmt_1000, 0 };
	static cilist io___29 = { 0, 0, 0, fmt_1040, 0 };


/*<       implicit double precision (a-h,o-z) >*/
/*<       character*4 ip1 >*/
/*<       dimension x(*),x0(*),xlb(*),xub(*),s(*),scale(ndv,2),temp(*) >*/
/*<       data ip1 /'(   '/ >*/
	/* Parameter adjustments */
	--x;
	--x0;
	--xlb;
	--xub;
	--s;
	scale_dim1 = *ndv;
	scale_offset = 1 + scale_dim1;
	scale -= scale_offset;
	--temp;

	/* Function Body */
/*<       data rtoler /0.00001/ >*/
/*<       data atoler /0.001/ >*/
/*<       data jmax /20/ >*/

/*          routine to perform one-dimensional search */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 19 July 1996 */

/*<       if (inext .gt. 0) go to 50 >*/
	if (*inext > 0) {
	goto L50;
	}

/*          initialization */

/*<       do 10 i = 1,ndv >*/
	i__1 = *ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         x0(i) = x(i) >*/
	x0[i__] = x[i__];
/*<    10 continue >*/
/* L10: */
	}
/*<       alpha = 0.0 >*/
	*alpha = 0.f;
/*<       alim = 0.0 >*/
	*alim = 0.f;
/*<       a4 = -1.0 >*/
	*a4 = -1.f;
/*<       atest = 0.0 >*/
	*atest = 0.f;
/*<       if (slope .ge. 0.0) go to 500 >*/
	if (*slope >= 0.f) {
	goto L500;
	}

/*          estimate initial alpha to limit obj to a small change */

/*<       alpha  = abs((0.10 * fun0) / slope) >*/
	*alpha = (d__1 = *fun0 * .1f / *slope, abs(d__1));
/*<       otest1 = abs(0.10 / slope) >*/
	otest1 = (d__1 = .1f / *slope, abs(d__1));
/*<       if (alpha .gt. otest1) alpha = otest1 >*/
	if (*alpha > otest1) {
	*alpha = otest1;
	}

/*          estimate initial alpha to limit x(i) to a small change */

/*<       do 20 i = 1,ndv >*/
	i__1 = *ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         si = abs(s(i)) >*/
	si = (d__1 = s[i__], abs(d__1));
/*<         if (si .lt. 1.0e-08) go to 20 >*/
	if (si < 1e-8f) {
		goto L20;
	}
/*<         xtest  = abs((0.10 * x(i)) / si) >*/
	xtest = (d__1 = x[i__] * .1f / si, abs(d__1));
/*<         xtest1 = abs(0.10 / si) >*/
	xtest1 = (d__1 = .1f / si, abs(d__1));
/*<         if (xtest .lt. xtest1) xtest = xtest1 >*/
	if (xtest < xtest1) {
		xtest = xtest1;
	}
/*<         if (alpha .gt. xtest) alpha = xtest >*/
	if (*alpha > xtest) {
		*alpha = xtest;
	}
/*<    20 continue >*/
L20:
	;
	}

/*          estimate initial alpha to prevent a bounds violation */

/*<       alpmax = 1.0e+10 >*/
	*alpmax = 1e10f;
/*<       if (nside .eq. 0) go to 40 >*/
	if (*nside == 0) {
	goto L40;
	}
/*<       do 30 i = 1,ndv >*/
	i__1 = *ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         if (abs(s(i)) .lt. 1.0e-08) go to 30 >*/
	if ((d__1 = s[i__], abs(d__1)) < 1e-8f) {
		goto L30;
	}
/*<         btest = (xub(i) - x(i)) / s(i) >*/
	btest = (xub[i__] - x[i__]) / s[i__];
/*<         if (btest .gt. 0.0 .and. btest .lt. alpmax) alpmax = btest >*/
	if (btest > 0.f && btest < *alpmax) {
		*alpmax = btest;
	}
/*<         btest = (xlb(i) - x(i)) / s(i) >*/
	btest = (xlb[i__] - x[i__]) / s[i__];
/*<         if (btest .gt. 0.0 .and. btest .lt. alpmax) alpmax = btest >*/
	if (btest > 0.f && btest < *alpmax) {
		*alpmax = btest;
	}
/*<    30 continue >*/
L30:
	;
	}

/*<       if (alpha .gt. alpmax) alpha = alpmax >*/
	if (*alpha > *alpmax) {
	*alpha = *alpmax;
	}
/*<       if (alpha .lt. 1.0e-06) alpha = 1.0e-06 >*/
	if (*alpha < 1e-6f) {
	*alpha = 1e-6f;
	}

/*<    40 continue >*/
L40:

/*<       if (iprnt3 .gt. 0) write (nunit,1010) alpha,alpmax,fun0 >*/
	if (*iprnt3 > 0) {
	io___11.ciunit = *nunit;
	s_wsfe(&io___11);
	do_fio(&c__1, (char *)&(*alpha), (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&(*alpmax), (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&(*fun0), (ftnlen)sizeof(doublereal));
	e_wsfe();
	}

/*<       a1 = 0.0 >*/
	*a1 = 0.f;
/*<       f1 = fun0 >*/
	*f1 = *fun0;
/*<       jtry = 0 >*/
	*jtry = 0;
/*<       inext = 1 >*/
	*inext = 1;
/*<       go to 600 >*/
	goto L600;

/*          store fun in appropriate location */

/*<    50 continue >*/
L50:
/*<       if (iprnt3 .gt. 0) write (nunit,1050) alpha,fun >*/
	if (*iprnt3 > 0) {
	io___12.ciunit = *nunit;
	s_wsfe(&io___12);
	do_fio(&c__1, (char *)&(*alpha), (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&(*fun), (ftnlen)sizeof(doublereal));
	e_wsfe();
	}
/*<       go to (100,200,300,400) inext >*/
	switch (*inext) {
	case 1:  goto L100;
	case 2:  goto L200;
	case 3:  goto L300;
	case 4:  goto L400;
	}

/*          store trial point a2 */

/*<   100 continue >*/
L100:
/*<       a2 = alpha >*/
	*a2 = *alpha;
/*<       f2 = fun >*/
	*f2 = *fun;
/*<       if (f2 .gt. f1) go to 150 >*/
	if (*f2 > *f1) {
	goto L150;
	}
/*<       if (limit .eq. 1) go to 500 >*/
	if (*limit == 1) {
	goto L500;
	}
/*<       alpha = 2.0 * a2 >*/
	*alpha = *a2 * 2.f;
/*<       inext = 3 >*/
	*inext = 3;
/*<       go to 600 >*/
	goto L600;

/*<   150 continue >*/
L150:
/*<       a3 = a2 >*/
	*a3 = *a2;
/*<       f3 = f2 >*/
	*f3 = *f2;
/*<       alpha = 0.1 * (9.0 * a1 + a3) >*/
	*alpha = (*a1 * 9.f + *a3) * .1f;
/*<       if (alpha .le. 1.0e-08) go to 500 >*/
	if (*alpha <= 1e-8f) {
	goto L500;
	}
/*<       inext = 2 >*/
	*inext = 2;
/*<       go to 600 >*/
	goto L600;

/*          point between a1 and a3 */

/*<   200 continue >*/
L200:
/*<       jtry = jtry + 1 >*/
	++(*jtry);
/*<       if (jtry .gt. jmax) go to 500 >*/
	if (*jtry > jmax) {
	goto L500;
	}

/*<       a2 = alpha >*/
	*a2 = *alpha;
/*<       f2 = fun >*/
	*f2 = *fun;
/*<       if (f2 .gt. f1) go to 150 >*/
	if (*f2 > *f1) {
	goto L150;
	}
/*<       go to 350 >*/
	goto L350;

/*          point after a2 */

/*<   300 continue >*/
L300:

/*<       jtry = jtry + 1 >*/
	++(*jtry);
/*<       if (jtry .gt. jmax) go to 500 >*/
	if (*jtry > jmax) {
	goto L500;
	}

/*<       a3 = alpha >*/
	*a3 = *alpha;
/*<       f3 = fun >*/
	*f3 = *fun;
/*<       if (f3 .lt. f2) go to 310 >*/
	if (*f3 < *f2) {
	goto L310;
	}
/*<       f12 = f1 - f2 >*/
	f12 = *f1 - *f2;
/*<       f32 = f3 - f2 >*/
	f32 = *f3 - *f2;
/*<       if (f12 .lt. 1.e-6) f12 = 1.e-6 >*/
	if (f12 < 1e-6f) {
	f12 = 1e-6f;
	}
/*<       if (f32 .lt. 10.0 * f12) go to 350 >*/
	if (f32 < f12 * 10.f) {
	goto L350;
	}
/*<       alpha = 0.5 * (a2 + a3) >*/
	*alpha = (*a2 + *a3) * .5f;
/*<       alim = a3 >*/
	*alim = *a3;
/*<       inext = 3 >*/
	*inext = 3;
/*<       go to 600 >*/
	goto L600;
/*<   310 continue >*/
L310:
/*<       if (limit .eq. 1) go to 500 >*/
	if (*limit == 1) {
	goto L500;
	}
/*<       call ksales (a1,a2,a3,f1,f2,f3,alim,alpha) >*/
	ksales_(a1, a2, a3, f1, f2, f3, alim, alpha);
/*<       a1 = a2 >*/
	*a1 = *a2;
/*<       f1 = f2 >*/
	*f1 = *f2;
/*<       a2 = a3 >*/
	*a2 = *a3;
/*<       f2 = f3 >*/
	*f2 = *f3;
/*<       inext = 3 >*/
	*inext = 3;
/*<       go to 600 >*/
	goto L600;

/*          perform quadratic or cubic interpolation */

/*<   350 continue >*/
L350:
/*<       if (a4 .le. 0.0) call ksquad (a1,a2,a3,f1,f2,f3,astar,fstar) >*/
	if (*a4 <= 0.f) {
	ksquad_(a1, a2, a3, f1, f2, f3, &astar, &fstar);
	}
/*<       if (a4 .gt. 0.0) call kscube (a1,a2,a3,a4,f1,f2,f3,f4,astar,fstar) >*/
	if (*a4 > 0.f) {
	kscube_(a1, a2, a3, a4, f1, f2, f3, f4, &astar, &fstar);
	}
/*<       alpha = astar >*/
	*alpha = astar;
/*<       inext = 4 >*/
	*inext = 4;
/*<       go to 600 >*/
	goto L600;

/*          check for convergence */

/*<   400 continue >*/
L400:

/*<       jtry = jtry + 1 >*/
	++(*jtry);
/*<       if (jtry .gt. jmax) go to 500 >*/
	if (*jtry > jmax) {
	goto L500;
	}

/*<       if (atest .le. 0.0) go to 450 >*/
	if (*atest <= 0.f) {
	goto L450;
	}
/*<       adiv = alpha >*/
	adiv = *alpha;
/*<       fdiv = abs(fun) >*/
	fdiv = abs(*fun);
/*<       if (adiv .lt. 1.e-06) adiv = 1.0 >*/
	if (adiv < 1e-6f) {
	adiv = 1.f;
	}
/*<       if (fdiv .lt. 1.e-06) fdiv = 1.0 >*/
	if (fdiv < 1e-6f) {
	fdiv = 1.f;
	}
/*<       aa = abs(atest - alpha) >*/
	aa = (d__1 = *atest - *alpha, abs(d__1));
/*<       ff = abs(ftest - fun) >*/
	ff = (d__1 = *ftest - *fun, abs(d__1));
/*<       atol = atoler * abs(alpha) >*/
	atol = atoler * abs(*alpha);
/*<       ftol = atoler * abs(fun) >*/
	ftol = atoler * abs(*fun);
/*<       if (atol .lt. 1.e-06) atol = 1.0e-06 >*/
	if (atol < 1e-6f) {
	atol = 1e-6f;
	}
/*<       if (ftol .lt. 1.e-06) ftol = 1.0e-06 >*/
	if (ftol < 1e-6f) {
	ftol = 1e-6f;
	}
/*<       if (aa .lt. atol .or. ff .lt. ftol) go to 500 >*/
	if (aa < atol || ff < ftol) {
	goto L500;
	}
/*<       aa = aa / adiv >*/
	aa /= adiv;
/*<       ff = ff / fdiv >*/
	ff /= fdiv;
/*<       if (aa .lt. rtoler .or. ff .lt. rtoler) go to 500 >*/
	if (aa < rtoler || ff < rtoler) {
	goto L500;
	}
/*<   450 continue >*/
L450:
/*<       atest = alpha >*/
	*atest = *alpha;
/*<       ftest = fun >*/
	*ftest = *fun;
/*<       call ksqmin (a1,a2,a3,a4,alpha,f1,f2,f3,f4,fun) >*/
	ksqmin_(a1, a2, a3, a4, alpha, f1, f2, f3, f4, fun);
/*<       go to 350 >*/
	goto L350;

/*          end of one-dimensional search */

/*<   500 continue >*/
L500:
/*<       inext = 0 >*/
	*inext = 0;
/*<       if (jtry .gt. jmax .or. limit .eq. 1) isdflg = -1 >*/
	if (*jtry > jmax || *limit == 1) {
	*isdflg = -1;
	}

/*          compute x-vector and get new function */

/*<   600 continue >*/
L600:
/*<       limit = 0 >*/
	*limit = 0;
/*<       if ( alpha .lt. alpmax) go to 610 >*/
	if (*alpha < *alpmax) {
	goto L610;
	}
/*<       alpha = alpmax >*/
	*alpha = *alpmax;
/*<       limit = 1 >*/
	*limit = 1;
/*<   610 continue >*/
L610:
/*<       do 620 i = 1,ndv >*/
	i__1 = *ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         x(i) = x0(i) + alpha * s(i) >*/
	x[i__] = x0[i__] + *alpha * s[i__];
/*<   620 continue >*/
/* L620: */
	}
/*<       if (iprnt3 .lt. 1) return >*/
	if (*iprnt3 < 1) {
	return 0;
	}
/*<       if (inext .gt. 0) write (nunit,1020) inext,jtry,alpha >*/
	if (*inext > 0) {
	io___23.ciunit = *nunit;
	s_wsfe(&io___23);
	do_fio(&c__1, (char *)&(*inext), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*jtry), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*alpha), (ftnlen)sizeof(doublereal));
	e_wsfe();
	}
/*<       if (inext .eq. 0) write (nunit,1080) inext,jtry,alpha >*/
	if (*inext == 0) {
	io___24.ciunit = *nunit;
	s_wsfe(&io___24);
	do_fio(&c__1, (char *)&(*inext), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*jtry), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*alpha), (ftnlen)sizeof(doublereal));
	e_wsfe();
	}
/*<       if (limit .eq. 1) write (nunit,1060) >*/
	if (*limit == 1) {
	io___25.ciunit = *nunit;
	s_wsfe(&io___25);
	e_wsfe();
	}
/*<       if (iprnt3 .lt. 2) return >*/
	if (*iprnt3 < 2) {
	return 0;
	}
/*<       call ksunsc (temp,x,scale,ndv) >*/
	ksunsc_(&temp[1], &x[1], &scale[scale_offset], ndv);
/*<       if (inext .ne. 0) write (nunit,1030) >*/
	if (*inext != 0) {
	io___26.ciunit = *nunit;
	s_wsfe(&io___26);
	e_wsfe();
	}
/*<       if (inext .eq. 0) write (nunit,1070) >*/
	if (*inext == 0) {
	io___27.ciunit = *nunit;
	s_wsfe(&io___27);
	e_wsfe();
	}
/*<       write (nunit,1000) (ip1,i,temp(i),i=1,ndv) >*/
	io___28.ciunit = *nunit;
	s_wsfe(&io___28);
	i__1 = *ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, ip1, (ftnlen)4);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&temp[i__], (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
/*<       write (nunit,1040) >*/
	io___29.ciunit = *nunit;
	s_wsfe(&io___29);
	e_wsfe();
/*<       return >*/
	return 0;

/*<  1000 format (3(3x,a1,i5,2h)=,e14.7)) >*/
/*<  1 >*/
/*<  1 >*/
/*<  1030 format (/35h       proposed design variable set/) >*/
/*<  1040 format (/) >*/
/*<  1050 format (/23h       current alpha = ,e12.6,14h   function = ,e12.6) >*/
/*<  1060 format (/51h       upper or lower bounds limit has been reached) >*/
/*<  1070 format (/32h       final design variable set/) >*/
/*<  1 >*/
/*<       end >*/
} /* ksoned_ */