/* ksinit.f -- translated by f2c (version 20100827).
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
/* Subroutine */ int ksinit_(doublereal *x, doublereal *xlb, doublereal *xub,
	doublereal *scale, doublereal *work, integer *mdv, integer *mcon,
	integer *mobj, integer *mside, integer *mscale, integer *jprnt,
	integer *jtmax, integer *jgrad, integer *jsdrst, doublereal *rdfun,
	doublereal *adfun, doublereal *fdelt, doublereal *fdmin, doublereal *
	rhomn, doublereal *rhomx, doublereal *rhodl, integer *munit, integer *
	ireq)
{
	/* Format strings */
	static char fmt_70[] = "(\0021\002,20x,\002============================="
		"===========\002/21x,\002=                                      "
		"=\002/21x,\002=    KSOPT Multiobjective Optimizer    =\002/21x"
		",\002=                                      =\002/21x,\002=    V"
		"ersion 2.8         19 July 96    =\002/21x,\002=                "
		"                      =\002/21x,\002=    Written by    Gregory A"
		". Wrenn    =\002/21x,\002=                                      ="
		"\002/21x,\002========================================\002///)";
	static char fmt_80[] = "(10x,\002The work array must be dimensioned at l"
		"east\002,i10,\002 words long.\002//)";
	static char fmt_90[] = "(/\002   Number of design variables is zero -- o"
		"ptimization\002,\002 is not possible\002//)";
	static char fmt_100[] = "(/\002   Number of objective functions is zero "
		"-- \002,\002optimization is not possible\002//)";
	static char fmt_110[] = "(/\002      ndv=\002,i5,\002     ncon=\002,i5"
		",\002     nobj=\002,i5,\002    nside=\002,i5,//\002   nscale="
		"\002,i5,\002   iprint=\002,i5,\002    itmax=\002,i5,\002    igra"
		"d=\002,i5)";
	static char fmt_120[] = "(/\002    rdfun=\002,e14.7,2x,\002    adfun="
		"\002,e14.7//\002    fdelt=\002,e14.7,2x,\002    fdmin=\002,e14.7"
		"//\002   rhomin=\002,e14.7,2x,\002   rhomax=\002,e14.7,2x,\002  "
		"   drho=\002,e14.7)";
	static char fmt_130[] = "(/42(\002-\002)/5x,\002 d.v. \002,6x,\002init"
		"ial\002,9x,\002 scale\002/5x,\002number\002,6x,\002 value \002,9"
		"x,\002factor\002/1x,42(\002-\002))";
	static char fmt_140[] = "(4x,i5,4x,e12.5,4x,e12.5)";
	static char fmt_150[] = "(/73(\002-\002)/5x,\002 d.v. \002,6x,\002init"
		"ial\002,9x,\002 scale\002,10x,\002lower\002,11x,\002upper\002/5x,"
		"\002number\002,6x,\002 value \002,9x,\002factor\002,10x,\002bound"
		"\002,11x,\002bound\002/1x,73(\002-\002))";
	static char fmt_160[] = "(4x,i5,4x,e12.5,4x,e12.5,4x,e12.5,4x,e12.5)";

	/* System generated locals */
	integer i__1;
	doublereal d__1, d__2;

	/* Builtin functions */
	integer s_wsfe(cilist *), e_wsfe(void), do_fio(integer *, char *, ftnlen);

	/* Local variables */
	static integer i__, j;
	static doublereal xb, ss, xx, xbl, xbu;
	static integer ntmp;
	extern /* Subroutine */ int kscomp_(doublereal *);

	/* Fortran I/O blocks */
	static cilist io___9 = { 0, 0, 0, fmt_70, 0 };
	static cilist io___10 = { 0, 0, 0, fmt_80, 0 };
	static cilist io___11 = { 0, 0, 0, fmt_90, 0 };
	static cilist io___12 = { 0, 0, 0, fmt_100, 0 };
	static cilist io___13 = { 0, 0, 0, fmt_110, 0 };
	static cilist io___14 = { 0, 0, 0, fmt_120, 0 };
	static cilist io___15 = { 0, 0, 0, fmt_130, 0 };
	static cilist io___16 = { 0, 0, 0, fmt_140, 0 };
	static cilist io___17 = { 0, 0, 0, fmt_150, 0 };
	static cilist io___18 = { 0, 0, 0, fmt_160, 0 };


/*<       implicit double precision (a-h,o-z) >*/
/*<    >*/
/*<       dimension x(*),xlb(*),xub(*),scale(*),work(*) >*/

/*          routine to initialize optimization parameters */
/*          for routine ksopt */

/*          input parameters are */

/*               x     -- initial design variable values */

/*               xlb   -- lower bounds on design variables */

/*               xub   -- upper bounds on design variables */

/*               scale -- scale factors for design variables */

/*               work  -- scratch array */

/*               mdv   -- number of design variables */

/*               mcon  -- number of constraints */

/*               mobj  -- number of objective functions */

/*               mside -- flag  =0 if no side constraints */

/*               mscale-- flag selecting design variable scaling */
/*                        =0 -- no scaling */
/*                        <0 -- user supplied scaling in vector scale */
/*                        >0 -- automatic scaling by ksopt every mscale */
/*                              iterations */

/*               jprnt -- print level flag (3 digit) */
/*                        each print level includes all lower print levels */

/*                        hundreds digit - one-dimensional search print */
/*                                         0 = no print */
/*                                         1 = alpha and k-s function */
/*                                         2 = proposed d.v. vector */

/*                        tens     digit - gradient print */
/*                                         0 = no print */
/*                                         1 = df and dg */

/*                        ones     digit - d.v. and constraint print */
/*                                         0 = no print */
/*                                         1 = initial and final iterations */
/*                                         2 = all iterations */
/*                                         3 = slope and search direction */
/*                                         4 = hessian matrix */

/*               jtmax -- maximum number of iterations */
/*                        (default is 20) */

/*               jgrad -- flag selecting user supplied information */
/*                        =0 -- user supplies function and constraints */
/*                              with gradients computed by finite */
/*                              differences from within ksopt */
/*                        =1 -- user supplies all function, constraint, */
/*                              and gradient information */
/*                        (default is 0) */

/*               jsdrst-- number of iterations before restarting search */
/*                        direction with steepest descent */
/*                        (default is ndv+1) */

/*               rdfun -- relative function change for termination */
/*                        (default is 0.01) */

/*               adfun -- absolute function change for termination */
/*                        (default is 0.0) */

/*               fdelt -- step size for computing finite differences */
/*                        derivatives */
/*                        (default is 0.01) */

/*               fdmin -- minimum difference for computing */
/*                        finite differences */
/*                        (default is 0.0001) */

/*               rhomn -- minimum multiplier for ks function */
/*                        (default is 5.0) */

/*               rhomx -- maximum multiplier for ks function */
/*                        (default is 100.0) */

/*               rhodl -- increment for rho */
/*                        (default is computed internally) */

/*               munit -- fortran unit number for output file */
/*                        (default is 0) */

/*          output parameters are */

/*               work  -- array containing information required */
/*                        by routine ksopt.  this array must not */
/*                        be altered during execution of ksopt. */

/*               ireq  -- required length of work array */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 19 July 1996 */

/*          these subroutines are compatible with fortran 77 */

/*<       jsel   = 1 >*/
	/* Parameter adjustments */
	--work;
	--scale;
	--xub;
	--xlb;
	--x;

	/* Function Body */
	kscomm_1.jsel = 1;
/*<       ndv    = mdv >*/
	kscomm_1.ndv = *mdv;
/*<       ncon   = mcon >*/
	kscomm_1.ncon = *mcon;
/*<       nobj   = mobj >*/
	kscomm_1.nobj = *mobj;
/*<       nside  = 0 >*/
	kscomm_1.nside = 0;
/*<       nscale = mscale >*/
	kscomm_1.nscale = *mscale;
/*<       iprnt  = 1 >*/
	kscomm_1.iprnt = 1;
/*<       itmax  = 20 >*/
	kscomm_1.itmax = 20;
/*<       igrad  = 0 >*/
	kscomm_1.igrad = 0;
/*<       isdrst = jsdrst >*/
	kscomm_1.isdrst = *jsdrst;
/*<       rdf    = 0.01 >*/
	kscomm_1.rdf = .01f;
/*<       adf    = 0.0 >*/
	kscomm_1.adf = 0.f;
/*<       fdl    = 0.01 >*/
	kscomm_1.fdl = .01f;
/*<       fdm    = 0.0001 >*/
	kscomm_1.fdm = 1e-4f;
/*<       rho    = 5.0 >*/
	kscomm_1.rho = 5.f;
/*<       rhomax = 100.0 >*/
	kscomm_1.rhomax = 100.f;
/*<       drho   = 0.0 >*/
	kscomm_1.drho = 0.f;
/*<       nunit  = 0 >*/
	kscomm_1.nunit = 0;
/*<       itcnt  = 1 >*/
	kscomm_1.itcnt = 1;
/*<       icntr  = 0 >*/
	kscomm_1.icntr = 0;
/*<       icnta  = 0 >*/
	kscomm_1.icnta = 0;
/*<       isdflg = 0 >*/
	kscomm_1.isdflg = 0;
/*<       ifncl  = 0 >*/
	kscomm_1.ifncl = 0;

/*<       if (mdv   .le. 0)   ndv   = 0 >*/
	if (*mdv <= 0) {
	kscomm_1.ndv = 0;
	}
/*<       if (mcon  .le. 0)   ncon  = 0 >*/
	if (*mcon <= 0) {
	kscomm_1.ncon = 0;
	}
/*<       if (mobj  .le. 0)   nobj  = 0 >*/
	if (*mobj <= 0) {
	kscomm_1.nobj = 0;
	}
/*<       if (mside .ne. 0)   nside = 1 >*/
	if (*mside != 0) {
	kscomm_1.nside = 1;
	}
/*<       if (jprnt .ge. 0)   iprnt = jprnt >*/
	if (*jprnt >= 0) {
	kscomm_1.iprnt = *jprnt;
	}
/*<       if (jtmax .gt. 0)   itmax = jtmax >*/
	if (*jtmax > 0) {
	kscomm_1.itmax = *jtmax;
	}
/*<       if (jgrad .ne. 0)   igrad = 1 >*/
	if (*jgrad != 0) {
	kscomm_1.igrad = 1;
	}
/*<       if (jsdrst.le. 0)   isdrst= ndv + 1 >*/
	if (*jsdrst <= 0) {
	kscomm_1.isdrst = kscomm_1.ndv + 1;
	}
/*<       if (rdfun .gt. 0.0) rdf   = rdfun >*/
	if (*rdfun > 0.f) {
	kscomm_1.rdf = *rdfun;
	}
/*<       if (adfun .gt. 0.0) adf   = adfun >*/
	if (*adfun > 0.f) {
	kscomm_1.adf = *adfun;
	}
/*<       if (fdelt .gt. 0.0) fdl   = fdelt >*/
	if (*fdelt > 0.f) {
	kscomm_1.fdl = *fdelt;
	}
/*<       if (fdmin .gt. 0.0) fdm   = fdmin >*/
	if (*fdmin > 0.f) {
	kscomm_1.fdm = *fdmin;
	}
/*<       if (rhomn .gt. 0.0) rho   = rhomn >*/
	if (*rhomn > 0.f) {
	kscomm_1.rho = *rhomn;
	}
/*<       if (rhomx .ge. rho) rhomax= rhomx >*/
	if (*rhomx >= kscomm_1.rho) {
	kscomm_1.rhomax = *rhomx;
	}
/*<       if (rhodl .gt. 0.0) drho  = rhodl >*/
	if (*rhodl > 0.f) {
	kscomm_1.drho = *rhodl;
	}
/*<       if (munit .gt. 0)   nunit = munit >*/
	if (*munit > 0) {
	kscomm_1.nunit = *munit;
	}

/*<       ntmp  = 2 * ndv >*/
	ntmp = kscomm_1.ndv << 1;
/*<       if (ntmp .lt. (nobj + ncon)) ntmp = nobj + ncon >*/
	if (ntmp < kscomm_1.nobj + kscomm_1.ncon) {
	ntmp = kscomm_1.nobj + kscomm_1.ncon;
	}
/*<       ifscl = 64 >*/
	kscomm_1.ifscl = 64;
/*<       ifoff = ifscl + nobj >*/
	kscomm_1.ifoff = kscomm_1.ifscl + kscomm_1.nobj;
/*<       isx   = ifoff + nobj >*/
	kscomm_1.isx = kscomm_1.ifoff + kscomm_1.nobj;
/*<       isx0  = isx   + ndv >*/
	kscomm_1.isx0 = kscomm_1.isx + kscomm_1.ndv;
/*<       isxlb = isx0  + ndv >*/
	kscomm_1.isxlb = kscomm_1.isx0 + kscomm_1.ndv;
/*<       isxub = isxlb + ndv >*/
	kscomm_1.isxub = kscomm_1.isxlb + kscomm_1.ndv;
/*<       iscl  = isxub + ndv >*/
	kscomm_1.iscl = kscomm_1.isxub + kscomm_1.ndv;
/*<       ig0   = iscl  + 2 * ndv >*/
	kscomm_1.ig0 = kscomm_1.iscl + (kscomm_1.ndv << 1);
/*<       idf   = ig0   + ncon >*/
	kscomm_1.idf = kscomm_1.ig0 + kscomm_1.ncon;
/*<       islp  = idf   + ndv >*/
	kscomm_1.islp = kscomm_1.idf + kscomm_1.ndv;
/*<       iobj0 = islp  + ndv >*/
	kscomm_1.iobj0 = kscomm_1.islp + kscomm_1.ndv;
/*<       iy    = iobj0 + nobj >*/
	kscomm_1.iy = kscomm_1.iobj0 + kscomm_1.nobj;
/*<       ip    = iy    + ndv >*/
	kscomm_1.ip = kscomm_1.iy + kscomm_1.ndv;
/*<       ih    = ip    + ndv >*/
	kscomm_1.ih = kscomm_1.ip + kscomm_1.ndv;
/*<       ihess = ih    + ndv * (ndv + 1) / 2 >*/
	kscomm_1.ihess = kscomm_1.ih + kscomm_1.ndv * (kscomm_1.ndv + 1) / 2;
/*<       iside = ihess + ndv * (ndv + 1) / 2 >*/
	kscomm_1.iside = kscomm_1.ihess + kscomm_1.ndv * (kscomm_1.ndv + 1) / 2;
/*<       isact = iside + ndv >*/
	kscomm_1.isact = kscomm_1.iside + kscomm_1.ndv;
/*<       idobj = isact + ndv >*/
	kscomm_1.idobj = kscomm_1.isact + kscomm_1.ndv;
/*<       idg   = idobj + nobj * ndv >*/
	kscomm_1.idg = kscomm_1.idobj + kscomm_1.nobj * kscomm_1.ndv;
/*<       itmp1 = idg   + ncon * ndv >*/
	kscomm_1.itmp1 = kscomm_1.idg + kscomm_1.ncon * kscomm_1.ndv;
/*<       itmp2 = itmp1 + ntmp >*/
	kscomm_1.itmp2 = kscomm_1.itmp1 + ntmp;
/*<       ireq  = itmp2 + ntmp - 1 >*/
	*ireq = kscomm_1.itmp2 + ntmp - 1;

/*<       if (ndv .eq. 0) go to 40 >*/
	if (kscomm_1.ndv == 0) {
	goto L40;
	}
/*<       do 30 i = 1,ndv >*/
	i__1 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         j = i - 1 >*/
	j = i__ - 1;
/*<         xx  = x(i) >*/
	xx = x[i__];
/*<         if (nside .eq. 0) go to 20 >*/
	if (kscomm_1.nside == 0) {
		goto L20;
	}
/*<         xbl = xlb(i) >*/
	xbl = xlb[i__];
/*<         xbu = xub(i) >*/
	xbu = xub[i__];
/*<         if (xbl .le. xbu) go to 10 >*/
	if (xbl <= xbu) {
		goto L10;
	}
/*<         xb = (xbl + xbu) / 2.0 >*/
	xb = (xbl + xbu) / 2.f;
/*<         xbl = xb >*/
	xbl = xb;
/*<         xbu = xb >*/
	xbu = xb;
/*<    10 continue >*/
L10:
/*<         if (xx .lt. xbl) xx = xbl >*/
	if (xx < xbl) {
		xx = xbl;
	}
/*<         if (xx .gt. xbu) xx = xbu >*/
	if (xx > xbu) {
		xx = xbu;
	}
/*<         x(i) = xx >*/
	x[i__] = xx;
/*<    20 continue >*/
L20:
/*<         ss = 1.0 >*/
	ss = 1.f;
/*<         if (nscale .gt. 0) ss = abs(xx) >*/
	if (kscomm_1.nscale > 0) {
		ss = abs(xx);
	}
/*<         if (nscale .lt. 0) ss = abs(scale(i)) >*/
	if (kscomm_1.nscale < 0) {
		ss = (d__1 = scale[i__], abs(d__1));
	}
/*<         if (ss .lt. 1.0e-04) ss = 1.0 >*/
	if (ss < 1e-4f) {
		ss = 1.f;
	}
/*<         work(iscl  + j) = ss >*/
	work[kscomm_1.iscl + j] = ss;
/*<         work(isx   + j) = xx / ss >*/
	work[kscomm_1.isx + j] = xx / ss;
/*<         work(isx0  + j) = xx / ss >*/
	work[kscomm_1.isx0 + j] = xx / ss;
/*<         if (nside .eq. 0) go to 30 >*/
	if (kscomm_1.nside == 0) {
		goto L30;
	}
/*<         work(isxlb + j) = xbl / ss >*/
	work[kscomm_1.isxlb + j] = xbl / ss;
/*<         work(isxub + j) = xbu / ss >*/
	work[kscomm_1.isxub + j] = xbu / ss;
/*<    30 continue >*/
L30:
	;
	}

/*<       if (drho .gt. 0.0) go to 40 >*/
	if (kscomm_1.drho > 0.f) {
	goto L40;
	}
/*<       drho = (rhomax - rho) / 5.0 >*/
	kscomm_1.drho = (kscomm_1.rhomax - kscomm_1.rho) / 5.f;
/*<       if (drho .lt. 10.0) drho = 10.0 >*/
	if (kscomm_1.drho < 10.f) {
	kscomm_1.drho = 10.f;
	}
/*<       if (drho .gt. 40.0) drho = 40.0 >*/
	if (kscomm_1.drho > 40.f) {
	kscomm_1.drho = 40.f;
	}

/*<    40 continue >*/
L40:

/*<       if (iprnt .eq. 0 .and. ndv .gt. 0 .and. nobj .gt. 0) go to 60 >*/
	if (kscomm_1.iprnt == 0 && kscomm_1.ndv > 0 && kscomm_1.nobj > 0) {
	goto L60;
	}
/*<       write (nunit,70) >*/
	io___9.ciunit = kscomm_1.nunit;
	s_wsfe(&io___9);
	e_wsfe();
/*<       write (nunit,80) ireq >*/
	io___10.ciunit = kscomm_1.nunit;
	s_wsfe(&io___10);
	do_fio(&c__1, (char *)&(*ireq), (ftnlen)sizeof(integer));
	e_wsfe();
/*<       if (ndv .eq. 0) write (nunit,90) >*/
	if (kscomm_1.ndv == 0) {
	io___11.ciunit = kscomm_1.nunit;
	s_wsfe(&io___11);
	e_wsfe();
	}
/*<       if (nobj .eq. 0) write (nunit,100) >*/
	if (kscomm_1.nobj == 0) {
	io___12.ciunit = kscomm_1.nunit;
	s_wsfe(&io___12);
	e_wsfe();
	}
/*<       write (nunit,110) ndv,ncon,nobj,nside,nscale,iprnt,itmax,igrad >*/
	io___13.ciunit = kscomm_1.nunit;
	s_wsfe(&io___13);
	do_fio(&c__1, (char *)&kscomm_1.ndv, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&kscomm_1.ncon, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&kscomm_1.nobj, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&kscomm_1.nside, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&kscomm_1.nscale, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&kscomm_1.iprnt, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&kscomm_1.itmax, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&kscomm_1.igrad, (ftnlen)sizeof(integer));
	e_wsfe();
/*<       write (nunit,120) rdf,adf,fdl,fdm,rho,rhomax,drho >*/
	io___14.ciunit = kscomm_1.nunit;
	s_wsfe(&io___14);
	do_fio(&c__1, (char *)&kscomm_1.rdf, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&kscomm_1.adf, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&kscomm_1.fdl, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&kscomm_1.fdm, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&kscomm_1.rho, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&kscomm_1.rhomax, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&kscomm_1.drho, (ftnlen)sizeof(doublereal));
	e_wsfe();
/*<       if (nside .ne. 0) go to 50 >*/
	if (kscomm_1.nside != 0) {
	goto L50;
	}
/*<       write (nunit,130) >*/
	io___15.ciunit = kscomm_1.nunit;
	s_wsfe(&io___15);
	e_wsfe();
/*<       write (nunit,140) (i,x(i),work(iscl+i-1),i=1,ndv) >*/
	io___16.ciunit = kscomm_1.nunit;
	s_wsfe(&io___16);
	i__1 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&x[i__], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&work[kscomm_1.iscl + i__ - 1], (ftnlen)sizeof(
		doublereal));
	}
	e_wsfe();
/*<       go to 60 >*/
	goto L60;
/*<    50 continue >*/
L50:
/*<       write (nunit,150) >*/
	io___17.ciunit = kscomm_1.nunit;
	s_wsfe(&io___17);
	e_wsfe();
/*<    >*/
	io___18.ciunit = kscomm_1.nunit;
	s_wsfe(&io___18);
	i__1 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&x[i__], (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&work[kscomm_1.iscl + i__ - 1], (ftnlen)sizeof(
		doublereal));
	d__1 = work[kscomm_1.isxlb + i__ - 1] * work[kscomm_1.iscl + i__ - 1];
	do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
	d__2 = work[kscomm_1.isxub + i__ - 1] * work[kscomm_1.iscl + i__ - 1];
	do_fio(&c__1, (char *)&d__2, (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
/*<    60 continue >*/
L60:
/*<       if (ndv .eq. 0 .or. nobj .eq. 0) jsel = 0 >*/
	if (kscomm_1.ndv == 0 || kscomm_1.nobj == 0) {
	kscomm_1.jsel = 0;
	}
/*<       call kscomp (work(1)) >*/
	kscomp_(&work[1]);
/*<       return >*/
	return 0;

/*<    >*/
/*<    >*/
/*<    >*/
/*<    >*/
/*<    >*/
/*<    >*/
/*<    >*/
/*<   140 format (4x,i5,4x,e12.5,4x,e12.5) >*/
/*<    >*/
/*<   160 format (4x,i5,4x,e12.5,4x,e12.5,4x,e12.5,4x,e12.5) >*/
/*<       end >*/
} /* ksinit_ */