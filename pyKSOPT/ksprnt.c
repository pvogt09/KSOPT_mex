/* ksprnt.f -- translated by f2c (version 20100827).
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

/* Common Block Declarations */

struct {
	doublereal rdf, adf, fdl, fdm, rho, drho, rhomax, fun0, slope, delx,
		alpha, alpmax, a1, a2, a3, a4, f1, f2, f3, f4, alim, atest, ftest;
	integer ifscl, ifoff, isx, isx0, isxlb, isxub, iscl, ig0, idf, islp,
		iobj0, iy, ip, ih, ihess, iside, isact, idobj, idg, itmp1, itmp2,
		inext, jnext, jsel, itcnt, icntr, icnta, isdflg, isdrst, ifncl,
		nunit, ndv, ncon, nobj, nside, nscale, iprnt, itmax, igrad, limit;
} kscomm_;

#define kscomm_1 kscomm_

/* Table of constant values */

static integer c__1 = 1;

/*<    >*/
/* Subroutine */ int ksprnt_(integer *ipflag, integer *iprnt1, integer *
	iprnt2, doublereal *x, doublereal *obj, doublereal *g, doublereal *df,
	doublereal *dg, doublereal *side, doublereal *scale, integer *nodim,
	integer *ncdim, doublereal *temp, doublereal *work)
{
	/* Initialized data */

	static char ip1[4] = "(   ";

	/* Format strings */
	static char fmt_1100[] = "(/33(\002##\002)/\002       Beginning of itera"
		"tion \002,i4)";
	static char fmt_1110[] = "(\002       current value of rho is    \002,e1"
		"2.5)";
	static char fmt_1120[] = "(/\002       design variables\002)";
	static char fmt_1000[] = "(3(3x,a1,i5,\002)=\002,e14.7))";
	static char fmt_1130[] = "(/\002       scaling vector\002)";
	static char fmt_1140[] = "(/\002       objective function number \002,"
		"i3,\002 = \002,e24.16)";
	static char fmt_1150[] = "(/\002       gradients of objective functio"
		"n\002)";
	static char fmt_1160[] = "(/\002       constraint values\002)";
	static char fmt_1170[] = "(/\002       gradients for constraint \002,i4)";
	static char fmt_1180[] = "(/\002       upper and lower bounds\002)";
	static char fmt_1190[] = "(5x,15i4)";
	static char fmt_1340[] = "(/\002       slope d(ks)/d(alpha)=\002,e24.1"
		"6//\002       search direction\002)";
	static char fmt_1350[] = "(/\002       hessian matrix reset\002)";
	static char fmt_1360[] = "(/\002       hessian matrix\002/)";
	static char fmt_1370[] = "(\002       row \002,i3,3x,6e10.3/(17x,6e10.3/"
		"))";
	static char fmt_1200[] = "(\0021      final optimization information\002)"
		;
	static char fmt_1210[] = "(/\002       final design variables\002)";
	static char fmt_1220[] = "(/\002       final value of rho is    \002,e12"
		".5)";
	static char fmt_1230[] = "(/\002       final objective function number"
		" \002,i3,\002 = \002,e24.16)";
	static char fmt_1240[] = "(/\002       final constraint values\002)";
	static char fmt_1250[] = "(/\002       final upper and lower bounds\002)";
	static char fmt_1260[] = "(5x,15i4)";
	static char fmt_1270[] = "(/\002       termination information\002)";
	static char fmt_1280[] = "(/\002       number of iterations=\002,i6)";
	static char fmt_1290[] = "(/\002       objective functions were evaluate"
		"d \002,i4,\002 times\002)";
	static char fmt_1300[] = "(/\002       optimization terminated by \002)";
	static char fmt_1310[] = "(/\002       iteration count exceeded itmax"
		"\002)";
	static char fmt_1320[] = "(/\002       relative change less than rdfun f"
		"or 3\002,\002 iterations\002)";
	static char fmt_1330[] = "(/\002       absolute change less than adfun f"
		"or 3\002,\002 iterations\002)";

	/* System generated locals */
	integer df_dim1, df_offset, dg_dim1, dg_offset, i__1, i__2;
	doublereal d__1;

	/* Builtin functions */
	integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

	/* Local variables */
	static integer i__, j, k, l, m;
	extern /* Subroutine */ int ksunsc_(doublereal *, doublereal *,
		doublereal *, integer *);

	/* Fortran I/O blocks */
	static cilist io___2 = { 0, 0, 0, fmt_1100, 0 };
	static cilist io___3 = { 0, 0, 0, fmt_1110, 0 };
	static cilist io___4 = { 0, 0, 0, fmt_1120, 0 };
	static cilist io___5 = { 0, 0, 0, fmt_1000, 0 };
	static cilist io___7 = { 0, 0, 0, fmt_1130, 0 };
	static cilist io___8 = { 0, 0, 0, fmt_1000, 0 };
	static cilist io___10 = { 0, 0, 0, fmt_1140, 0 };
	static cilist io___11 = { 0, 0, 0, fmt_1150, 0 };
	static cilist io___12 = { 0, 0, 0, fmt_1000, 0 };
	static cilist io___13 = { 0, 0, 0, fmt_1160, 0 };
	static cilist io___14 = { 0, 0, 0, fmt_1000, 0 };
	static cilist io___15 = { 0, 0, 0, fmt_1170, 0 };
	static cilist io___16 = { 0, 0, 0, fmt_1000, 0 };
	static cilist io___18 = { 0, 0, 0, fmt_1180, 0 };
	static cilist io___19 = { 0, 0, 0, fmt_1190, 0 };
	static cilist io___20 = { 0, 0, 0, fmt_1340, 0 };
	static cilist io___21 = { 0, 0, 0, fmt_1000, 0 };
	static cilist io___22 = { 0, 0, 0, fmt_1350, 0 };
	static cilist io___24 = { 0, 0, 0, fmt_1360, 0 };
	static cilist io___26 = { 0, 0, 0, fmt_1370, 0 };
	static cilist io___27 = { 0, 0, 0, fmt_1200, 0 };
	static cilist io___28 = { 0, 0, 0, fmt_1210, 0 };
	static cilist io___29 = { 0, 0, 0, fmt_1000, 0 };
	static cilist io___30 = { 0, 0, 0, fmt_1220, 0 };
	static cilist io___31 = { 0, 0, 0, fmt_1230, 0 };
	static cilist io___32 = { 0, 0, 0, fmt_1240, 0 };
	static cilist io___33 = { 0, 0, 0, fmt_1000, 0 };
	static cilist io___34 = { 0, 0, 0, fmt_1250, 0 };
	static cilist io___35 = { 0, 0, 0, fmt_1260, 0 };
	static cilist io___36 = { 0, 0, 0, fmt_1270, 0 };
	static cilist io___37 = { 0, 0, 0, fmt_1280, 0 };
	static cilist io___38 = { 0, 0, 0, fmt_1290, 0 };
	static cilist io___39 = { 0, 0, 0, fmt_1300, 0 };
	static cilist io___40 = { 0, 0, 0, fmt_1310, 0 };
	static cilist io___41 = { 0, 0, 0, fmt_1320, 0 };
	static cilist io___42 = { 0, 0, 0, fmt_1330, 0 };


/*<       implicit double precision (a-h,o-z) >*/
/*<    >*/
/*<       character*4 ip1 >*/
/*<       dimension x(*),obj(*),g(*),df(nodim,*),dg(ncdim,*) >*/
/*<       dimension side(*),scale(*),temp(*),work(*) >*/
/*<       data ip1 /'(   '/ >*/
	/* Parameter adjustments */
	--x;
	--obj;
	--g;
	--side;
	--scale;
	df_dim1 = *nodim;
	df_offset = 1 + df_dim1;
	df -= df_offset;
	dg_dim1 = *ncdim;
	dg_offset = 1 + dg_dim1;
	dg -= dg_offset;
	--temp;
	--work;

	/* Function Body */

/*          main print routine */
/*            if (ipflag = 1) print iteration data */
/*            if (ipflag = 2) print final optimization data */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 19 July 1996 */

/*<       if (ipflag .ne. 1) go to 200 >*/
	if (*ipflag != 1) {
	goto L200;
	}

/*          iteration data */

/*<   100 continue >*/
/* L100: */
/*<       if (iprnt1 .eq. 0) go to 300 >*/
	if (*iprnt1 == 0) {
	goto L300;
	}
/*<       if (iprnt1 .lt. 2 .and. itcnt .gt. 1) go to 300 >*/
	if (*iprnt1 < 2 && kscomm_1.itcnt > 1) {
	goto L300;
	}

/*<       call ksunsc (x,work(isx),work(iscl),ndv) >*/
	ksunsc_(&x[1], &work[kscomm_1.isx], &work[kscomm_1.iscl], &kscomm_1.ndv);
/*<       write (nunit,1100) itcnt >*/
	io___2.ciunit = kscomm_1.nunit;
	s_wsfe(&io___2);
	do_fio(&c__1, (char *)&kscomm_1.itcnt, (ftnlen)sizeof(integer));
	e_wsfe();
/*<       write (nunit,1110) rho >*/
	io___3.ciunit = kscomm_1.nunit;
	s_wsfe(&io___3);
	do_fio(&c__1, (char *)&kscomm_1.rho, (ftnlen)sizeof(doublereal));
	e_wsfe();

/*          design variables */

/*<       write (nunit,1120) >*/
	io___4.ciunit = kscomm_1.nunit;
	s_wsfe(&io___4);
	e_wsfe();
/*<       write (nunit,1000) (ip1,i,x(i),i=1,ndv) >*/
	io___5.ciunit = kscomm_1.nunit;
	s_wsfe(&io___5);
	i__1 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, ip1, (ftnlen)4);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&x[i__], (ftnlen)sizeof(doublereal));
	}
	e_wsfe();

/*          scaling vector */

/*<       if (nscale .eq. 0) go to 110 >*/
	if (kscomm_1.nscale == 0) {
	goto L110;
	}
/*<       write (nunit,1130) >*/
	io___7.ciunit = kscomm_1.nunit;
	s_wsfe(&io___7);
	e_wsfe();
/*<       write (nunit,1000) (ip1,i,scale(i),i=1,ndv) >*/
	io___8.ciunit = kscomm_1.nunit;
	s_wsfe(&io___8);
	i__1 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, ip1, (ftnlen)4);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&scale[i__], (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
/*<   110 continue >*/
L110:

/*          objective functions and their gradients */

/*<       do 120 j = 1,nobj >*/
	i__1 = kscomm_1.nobj;
	for (j = 1; j <= i__1; ++j) {
/*<         write (nunit,1140) j,obj(j) >*/
	io___10.ciunit = kscomm_1.nunit;
	s_wsfe(&io___10);
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&obj[j], (ftnlen)sizeof(doublereal));
	e_wsfe();
/*<         if (iprnt2 .eq. 0) go to 120 >*/
	if (*iprnt2 == 0) {
		goto L120;
	}
/*<         write (nunit,1150) >*/
	io___11.ciunit = kscomm_1.nunit;
	s_wsfe(&io___11);
	e_wsfe();
/*<         write (nunit,1000) (ip1,i,(df(j,i) / scale(i)),i=1,ndv) >*/
	io___12.ciunit = kscomm_1.nunit;
	s_wsfe(&io___12);
	i__2 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__2; ++i__) {
		do_fio(&c__1, ip1, (ftnlen)4);
		do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
		d__1 = df[j + i__ * df_dim1] / scale[i__];
		do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
/*<   120 continue >*/
L120:
	;
	}

/*          constraints and their gradients */

/*<       if (ncon .eq. 0) go to 140 >*/
	if (kscomm_1.ncon == 0) {
	goto L140;
	}
/*<       write (nunit,1160) >*/
	io___13.ciunit = kscomm_1.nunit;
	s_wsfe(&io___13);
	e_wsfe();
/*<       write (nunit,1000) (ip1,i,g(i),i=1,ncon) >*/
	io___14.ciunit = kscomm_1.nunit;
	s_wsfe(&io___14);
	i__1 = kscomm_1.ncon;
	for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, ip1, (ftnlen)4);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&g[i__], (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
/*<       if (iprnt2 .eq. 0) go to 140 >*/
	if (*iprnt2 == 0) {
	goto L140;
	}
/*<       do 130 j = 1,ncon >*/
	i__1 = kscomm_1.ncon;
	for (j = 1; j <= i__1; ++j) {
/*<         write (nunit,1170) j >*/
	io___15.ciunit = kscomm_1.nunit;
	s_wsfe(&io___15);
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	e_wsfe();
/*<         write (nunit,1000) (ip1,i,(dg(j,i) / scale(i)),i=1,ndv) >*/
	io___16.ciunit = kscomm_1.nunit;
	s_wsfe(&io___16);
	i__2 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__2; ++i__) {
		do_fio(&c__1, ip1, (ftnlen)4);
		do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
		d__1 = dg[j + i__ * dg_dim1] / scale[i__];
		do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
/*<   130 continue >*/
/* L130: */
	}
/*<   140 continue >*/
L140:

/*          side constraints */

/*<       if (nside .eq. 0) go to 160 >*/
	if (kscomm_1.nside == 0) {
	goto L160;
	}
/*<       m = 0 >*/
	m = 0;
/*<       do 150 i = 1,ndv >*/
	i__1 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         if (side(i) .eq. 0.0) go to 150 >*/
	if (side[i__] == 0.f) {
		goto L150;
	}
/*<         m = m + 1 >*/
	++m;
/*<         if (side(i) .eq.  -1.0) temp(m) = float(-i) >*/
	if (side[i__] == -1.f) {
		temp[m] = (real) (-i__);
	}
/*<         if (side(i) .eq.   1.0) temp(m) = float(i) >*/
	if (side[i__] == 1.f) {
		temp[m] = (real) i__;
	}
/*<         if (side(i) .ne. 999.0) go to 150 >*/
	if (side[i__] != 999.f) {
		goto L150;
	}
/*<         temp(m) = float(-i) >*/
	temp[m] = (real) (-i__);
/*<         m = m + 1 >*/
	++m;
/*<         temp(m) = float(i) >*/
	temp[m] = (real) i__;
/*<   150 continue >*/
L150:
	;
	}
/*<       if (m .eq. 0) go to 160 >*/
	if (m == 0) {
	goto L160;
	}
/*<       write (nunit,1180) >*/
	io___18.ciunit = kscomm_1.nunit;
	s_wsfe(&io___18);
	e_wsfe();
/*<       write (nunit,1190) (int(temp(i)),i=1,m) >*/
	io___19.ciunit = kscomm_1.nunit;
	s_wsfe(&io___19);
	i__1 = m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = (integer) temp[i__];
	do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
	}
	e_wsfe();
/*<   160 continue >*/
L160:

/*          print search direction and slope */

/*<       if (iprnt1 .lt. 3) go to 300 >*/
	if (*iprnt1 < 3) {
	goto L300;
	}
/*<       write (nunit,1340) slope >*/
	io___20.ciunit = kscomm_1.nunit;
	s_wsfe(&io___20);
	do_fio(&c__1, (char *)&kscomm_1.slope, (ftnlen)sizeof(doublereal));
	e_wsfe();
/*<       write (nunit,1000) (ip1,i,work(islp+i-1),i=1,ndv) >*/
	io___21.ciunit = kscomm_1.nunit;
	s_wsfe(&io___21);
	i__2 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__2; ++i__) {
	do_fio(&c__1, ip1, (ftnlen)4);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&work[kscomm_1.islp + i__ - 1], (ftnlen)sizeof(
		doublereal));
	}
	e_wsfe();

/*          print hessian matrix */

/*<       if (iprnt1 .lt. 4) go to 300 >*/
	if (*iprnt1 < 4) {
	goto L300;
	}
/*<       if (isdflg .eq. 0) write (nunit,1350) >*/
	if (kscomm_1.isdflg == 0) {
	io___22.ciunit = kscomm_1.nunit;
	s_wsfe(&io___22);
	e_wsfe();
	}
/*<       k = 1 >*/
	k = 1;
/*<       write (nunit,1360) >*/
	io___24.ciunit = kscomm_1.nunit;
	s_wsfe(&io___24);
	e_wsfe();
/*<       do 170 i = 1,ndv >*/
	i__2 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__2; ++i__) {
/*<         l = k + i - 1 >*/
	l = k + i__ - 1;
/*<         write (nunit,1370) i,(work(ih+j-1),j=k,l) >*/
	io___26.ciunit = kscomm_1.nunit;
	s_wsfe(&io___26);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	i__1 = l;
	for (j = k; j <= i__1; ++j) {
		do_fio(&c__1, (char *)&work[kscomm_1.ih + j - 1], (ftnlen)sizeof(
			doublereal));
	}
	e_wsfe();
/*<         k = l + 1 >*/
	k = l + 1;
/*<   170 continue >*/
/* L170: */
	}
/*<       go to 300 >*/
	goto L300;

/*          final optimization data */

/*<   200 continue >*/
L200:
/*<       if (ipflag .ne. 2) go to 300 >*/
	if (*ipflag != 2) {
	goto L300;
	}
/*<       if (iprnt1 .eq. 0) go to 300 >*/
	if (*iprnt1 == 0) {
	goto L300;
	}

/*<       call ksunsc (x,work(isx),work(iscl),ndv) >*/
	ksunsc_(&x[1], &work[kscomm_1.isx], &work[kscomm_1.iscl], &kscomm_1.ndv);
/*<       write (nunit,1200) >*/
	io___27.ciunit = kscomm_1.nunit;
	s_wsfe(&io___27);
	e_wsfe();
/*<       write (nunit,1210) >*/
	io___28.ciunit = kscomm_1.nunit;
	s_wsfe(&io___28);
	e_wsfe();
/*<       write (nunit,1000) (ip1,i,x(i),i=1,ndv) >*/
	io___29.ciunit = kscomm_1.nunit;
	s_wsfe(&io___29);
	i__2 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__2; ++i__) {
	do_fio(&c__1, ip1, (ftnlen)4);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&x[i__], (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
/*<       write (nunit,1220) rho >*/
	io___30.ciunit = kscomm_1.nunit;
	s_wsfe(&io___30);
	do_fio(&c__1, (char *)&kscomm_1.rho, (ftnlen)sizeof(doublereal));
	e_wsfe();
/*<       do 210 j = 1,nobj >*/
	i__2 = kscomm_1.nobj;
	for (j = 1; j <= i__2; ++j) {
/*<         write (nunit,1230) j,obj(j) >*/
	io___31.ciunit = kscomm_1.nunit;
	s_wsfe(&io___31);
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&obj[j], (ftnlen)sizeof(doublereal));
	e_wsfe();
/*<   210 continue >*/
/* L210: */
	}
/*<       if (ncon .eq. 0) go to 220 >*/
	if (kscomm_1.ncon == 0) {
	goto L220;
	}
/*<       write (nunit,1240) >*/
	io___32.ciunit = kscomm_1.nunit;
	s_wsfe(&io___32);
	e_wsfe();
/*<       write (nunit,1000) (ip1,i,g(i),i=1,ncon) >*/
	io___33.ciunit = kscomm_1.nunit;
	s_wsfe(&io___33);
	i__2 = kscomm_1.ncon;
	for (i__ = 1; i__ <= i__2; ++i__) {
	do_fio(&c__1, ip1, (ftnlen)4);
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&g[i__], (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
/*<   220 continue >*/
L220:
/*<       if (nside .eq. 0) go to 240 >*/
	if (kscomm_1.nside == 0) {
	goto L240;
	}
/*<       m = 0 >*/
	m = 0;
/*<       do 230 i = 1,ndv >*/
	i__2 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__2; ++i__) {
/*<         if (side(i) .eq. 0.0) go to 230 >*/
	if (side[i__] == 0.f) {
		goto L230;
	}
/*<         m = m + 1 >*/
	++m;
/*<         if (side(i) .eq.  -1.0) temp(m) = float(-i) >*/
	if (side[i__] == -1.f) {
		temp[m] = (real) (-i__);
	}
/*<         if (side(i) .eq.   1.0) temp(m) = float(i) >*/
	if (side[i__] == 1.f) {
		temp[m] = (real) i__;
	}
/*<         if (side(i) .ne. 999.0) go to 230 >*/
	if (side[i__] != 999.f) {
		goto L230;
	}
/*<         temp(m) = float(-i) >*/
	temp[m] = (real) (-i__);
/*<         m = m + 1 >*/
	++m;
/*<         temp(m) = float(i) >*/
	temp[m] = (real) i__;
/*<   230 continue >*/
L230:
	;
	}
/*<       if (m .eq. 0) go to 240 >*/
	if (m == 0) {
	goto L240;
	}
/*<       write (nunit,1250) >*/
	io___34.ciunit = kscomm_1.nunit;
	s_wsfe(&io___34);
	e_wsfe();
/*<       write (nunit,1260) (int(temp(i)),i=1,m) >*/
	io___35.ciunit = kscomm_1.nunit;
	s_wsfe(&io___35);
	i__2 = m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = (integer) temp[i__];
	do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	}
	e_wsfe();
/*<   240 continue >*/
L240:
/*<       write (nunit,1270) >*/
	io___36.ciunit = kscomm_1.nunit;
	s_wsfe(&io___36);
	e_wsfe();
/*<       write (nunit,1280) itcnt >*/
	io___37.ciunit = kscomm_1.nunit;
	s_wsfe(&io___37);
	do_fio(&c__1, (char *)&kscomm_1.itcnt, (ftnlen)sizeof(integer));
	e_wsfe();
/*<       write (nunit,1290) ifncl >*/
	io___38.ciunit = kscomm_1.nunit;
	s_wsfe(&io___38);
	do_fio(&c__1, (char *)&kscomm_1.ifncl, (ftnlen)sizeof(integer));
	e_wsfe();
/*<       write (nunit,1300) >*/
	io___39.ciunit = kscomm_1.nunit;
	s_wsfe(&io___39);
	e_wsfe();
/*<       if (itcnt .ge. itmax) write (nunit,1310) >*/
	if (kscomm_1.itcnt >= kscomm_1.itmax) {
	io___40.ciunit = kscomm_1.nunit;
	s_wsfe(&io___40);
	e_wsfe();
	}
/*<       if (icntr .ge. 3    ) write (nunit,1320) >*/
	if (kscomm_1.icntr >= 3) {
	io___41.ciunit = kscomm_1.nunit;
	s_wsfe(&io___41);
	e_wsfe();
	}
/*<       if (icnta .ge. 3    ) write (nunit,1330) >*/
	if (kscomm_1.icnta >= 3) {
	io___42.ciunit = kscomm_1.nunit;
	s_wsfe(&io___42);
	e_wsfe();
	}

/*<   300 continue >*/
L300:
/*<       return >*/
	return 0;

/*<  1000 format (3(3x,a1,i5,2h)=,e14.7)) >*/
/*<  1100 format (/33(2h##)/30h       Beginning of iteration ,i4) >*/
/*<  1110 format (34h       current value of rho is    ,e12.5) >*/
/*<  1120 format (/23h       design variables) >*/
/*<  1130 format (/21h       scaling vector) >*/
/*<  1140 format (/33h       objective function number ,i3,3h = ,e24.16) >*/
/*<  1150 format (/38h       gradients of objective function) >*/
/*<  1160 format (/24h       constraint values) >*/
/*<  1170 format (/32h       gradients for constraint ,i4) >*/
/*<  1180 format (/29h       upper and lower bounds) >*/
/*<  1190 format (5x,15i4) >*/

/*<  1200 format (37h1      final optimization information) >*/
/*<  1210 format (/29h       final design variables) >*/
/*<  1220 format (/32h       final value of rho is    ,e12.5) >*/
/*<  1 >*/
/*<  1240 format (/30h       final constraint values) >*/
/*<  1250 format (/35h       final upper and lower bounds) >*/
/*<  1260 format (5x,15i4) >*/
/*<  1270 format (/30h       termination information) >*/
/*<  1280 format (/28h       number of iterations=,i6) >*/
/*<  1 >*/
/*<  1300 format (/34h       optimization terminated by ) >*/
/*<  1310 format (/37h       iteration count exceeded itmax) >*/
/*<  1 >*/
/*<  1 >*/
/*<  1 >*/
/*<  1350 format (/27h       hessian matrix reset) >*/
/*<  1360 format (/21h       hessian matrix/) >*/
/*<  1370 format (11h       row ,i3,3x,6e10.3/(17x,6e10.3/)) >*/

/*<       end >*/
} /* ksprnt_ */