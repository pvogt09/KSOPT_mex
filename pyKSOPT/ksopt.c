/* ksopt.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksopt (isel,x,obj,g,df,dg,nomax,ngmax,work) >*/
/* Subroutine */ int ksopt_(integer *isel, doublereal *x, doublereal *obj,
	doublereal *g, doublereal *df, doublereal *dg, integer *nomax,
	integer *ngmax, doublereal *work)
{
	/* System generated locals */
	integer df_dim1, df_offset, dg_dim1, dg_offset, i__1, i__2;
	doublereal d__1;

	/* Local variables */
	static integer i__, j, k, l;
	static doublereal af, ss, fun, adelf;
	static integer ncdim;
	static doublereal rdelf;
	static integer ileft, nodim;
	extern /* Subroutine */ int ksdfp_(doublereal *, doublereal *, doublereal
		*, doublereal *, integer *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *, integer *)
		, ksfun_(doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, integer *, integer *, doublereal *);
	static integer iprnt1, iprnt2, iprnt3, iscale, ipflag, itflag;
	extern /* Subroutine */ int ksgrad_(integer *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, integer *, integer *, integer *,
		integer *, doublereal *, doublereal *, integer *, integer *),
		ksando_(doublereal *, doublereal *, doublereal *, doublereal *,
		integer *, integer *), ksscal_(doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, integer *, integer *,
		integer *), kscomg_(doublereal *), ksside_(doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *, integer *,
		integer *), ksoned_(integer *, integer *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, integer *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *, integer *,
		integer *, integer *, integer *, doublereal *, doublereal *,
		integer *), ksdfun_(doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, integer *, integer *, integer *, doublereal *,
		doublereal *, integer *, integer *), kscomp_(doublereal *),
		kshess_(doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, doublereal *, integer *,
		integer *, integer *, doublereal *, doublereal *, integer *,
		integer *), ksunsc_(doublereal *, doublereal *, doublereal *,
		integer *), ksxlim_(doublereal *, doublereal *, doublereal *,
		integer *, integer *), ksprnt_(integer *, integer *, integer *,
		doublereal *, doublereal *, doublereal *, doublereal *,
		doublereal *, doublereal *, doublereal *, integer *, integer *,
		doublereal *, doublereal *);

/*<       implicit double precision (a-h,o-z) >*/
/*<    >*/
/*<       dimension x(*),obj(*),g(*),df(nomax,*),dg(ngmax,*),work(*) >*/

/*          KSOPT is a general purpose multiobjective optimization */
/*          program which performs a constrained to unconstrained */
/*          transformation on the given optimization problem. */
/*          The problem is then solved using a Davidon-Fletcher-Powell */
/*          unconstrained optimization method. */

/*          input parameters are */
/*               obj   -- current set of objective function values */
/*               g     -- current set of constraints */
/*               df    -- derivatives of objective funcs. w.r.t. x-vector */
/*               dg    -- derivatives of constraints w.r.t. x-vector */
/*               nomax -- first dimension of df matrix */
/*               ngmax -- first dimension of dg matrix */
/*               work  -- working array from routine ksinit */
/*                        which must not be altered during optimization. */

/*          output parameters are */
/*               isel  -- flag requesting user supplied information */
/*                        =0 -- minimization terminated */
/*                        =1 -- user supplies obj and g */
/*                        =2 -- user supplies df and dg */
/*               x     -- new set of design variables */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 19 July 1996 */


/*          choose a path based on the value of jsel */

/*<       call kscomg (work(1)) >*/
	/* Parameter adjustments */
	--x;
	--obj;
	--g;
	df_dim1 = *nomax;
	df_offset = 1 + df_dim1;
	df -= df_offset;
	dg_dim1 = *ngmax;
	dg_offset = 1 + dg_dim1;
	dg -= dg_offset;
	--work;

	/* Function Body */
	kscomg_(&work[1]);
/*<       iprnt3 = iprnt / 100 >*/
	iprnt3 = kscomm_1.iprnt / 100;
/*<       ileft  = mod(iprnt,100) >*/
	ileft = kscomm_1.iprnt % 100;
/*<       iprnt2 = ileft / 10 >*/
	iprnt2 = ileft / 10;
/*<       iprnt1 = mod(ileft,10) >*/
	iprnt1 = ileft % 10;
/*<       nodim  = nobj >*/
	nodim = kscomm_1.nobj;
/*<       if (nodim .le. 0) nodim = 1 >*/
	if (nodim <= 0) {
	nodim = 1;
	}
/*<       ncdim  = ncon >*/
	ncdim = kscomm_1.ncon;
/*<       if (ncdim .le. 0) ncdim = 1 >*/
	if (ncdim <= 0) {
	ncdim = 1;
	}

/*<       isel = 0 >*/
	*isel = 0;
/*<       if (jsel .le. 0) return >*/
	if (kscomm_1.jsel <= 0) {
	return 0;
	}

/*<       go to (100,200,300,400,500,600) jsel >*/
	switch (kscomm_1.jsel) {
	case 1:  goto L100;
	case 2:  goto L200;
	case 3:  goto L300;
	case 4:  goto L400;
	case 5:  goto L500;
	case 6:  goto L600;
	}

/*          begin an iteration */

/*<   100 continue >*/
L100:
/*<       isel = 1 >*/
	*isel = 1;
/*<       jsel = 2 >*/
	kscomm_1.jsel = 2;
/*<       inext = 0 >*/
	kscomm_1.inext = 0;
/*<       if (itcnt .eq. 1) go to 900 >*/
	if (kscomm_1.itcnt == 1) {
	goto L900;
	}

/*          compute each objective function's scale and offset */

/*<   200 continue >*/
L200:
/*<       call ksando (obj,g,work(ifscl),work(ifoff),ncon,nobj) >*/
	ksando_(&obj[1], &g[1], &work[kscomm_1.ifscl], &work[kscomm_1.ifoff], &
		kscomm_1.ncon, &kscomm_1.nobj);

/*          calculate initial unconstrained function */

/*<    >*/
	ksfun_(&kscomm_1.fun0, &obj[1], &g[1], &kscomm_1.rho, &work[
		kscomm_1.ifscl], &work[kscomm_1.ifoff], &kscomm_1.ncon, &
		kscomm_1.nobj, &work[kscomm_1.itmp1]);
/*<       if (igrad .eq. 1) go to 350 >*/
	if (kscomm_1.igrad == 1) {
	goto L350;
	}

/*          get finite difference gradients of obj and constraints */

/*<   300 continue >*/
L300:
/*<       jsel = 3 >*/
	kscomm_1.jsel = 3;
/*<       isel = 1 >*/
	*isel = 1;
/*<    >*/
	ksgrad_(&kscomm_1.inext, &work[kscomm_1.isx], &work[kscomm_1.isx0], &work[
		kscomm_1.isxlb], &work[kscomm_1.isxub], &g[1], &work[kscomm_1.ig0]
		, &obj[1], &work[kscomm_1.iobj0], &work[kscomm_1.idobj], &work[
		kscomm_1.idg], &work[kscomm_1.iscl], &kscomm_1.delx, &
		kscomm_1.ndv, &kscomm_1.ncon, &kscomm_1.nobj, &kscomm_1.nside, &
		kscomm_1.fdl, &kscomm_1.fdm, &nodim, &ncdim);
/*<       if (inext .ne. 0) go to 900 >*/
	if (kscomm_1.inext != 0) {
	goto L900;
	}

/*<       do 310 i = 1,nobj >*/
	i__1 = kscomm_1.nobj;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         obj(i) = work(iobj0 + i - 1) >*/
	obj[i__] = work[kscomm_1.iobj0 + i__ - 1];
/*<   310 continue >*/
/* L310: */
	}

/*<       if (ncon .le. 0) go to 330 >*/
	if (kscomm_1.ncon <= 0) {
	goto L330;
	}
/*<       do 320 i = 1,ncon >*/
	i__1 = kscomm_1.ncon;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         g(i) = work(ig0 + i - 1) >*/
	g[i__] = work[kscomm_1.ig0 + i__ - 1];
/*<   320 continue >*/
/* L320: */
	}
/*<   330 continue >*/
L330:
/*<       go to 440 >*/
	goto L440;

/*          get analytical gradients of obj and constraints */

/*<   350 continue >*/
L350:
/*<       do 360 i = 1,nobj >*/
	i__1 = kscomm_1.nobj;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         work(iobj0 + i - 1) = obj(i) >*/
	work[kscomm_1.iobj0 + i__ - 1] = obj[i__];
/*<   360 continue >*/
/* L360: */
	}

/*<       if (ncon .eq. 0) go to 380 >*/
	if (kscomm_1.ncon == 0) {
	goto L380;
	}
/*<       do 370 i = 1,ncon >*/
	i__1 = kscomm_1.ncon;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         work(ig0 + i - 1) = g(i) >*/
	work[kscomm_1.ig0 + i__ - 1] = g[i__];
/*<   370 continue >*/
/* L370: */
	}
/*<   380 continue >*/
L380:
/*<       isel = 2 >*/
	*isel = 2;
/*<       jsel = 4 >*/
	kscomm_1.jsel = 4;
/*<       go to 900 >*/
	goto L900;

/*          scale analytical gradients of obj and constraints */

/*<   400 continue >*/
L400:
/*<       do 430 i = 1,ndv >*/
	i__1 = kscomm_1.ndv;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*<         ss = work(iscl + i - 1) >*/
	ss = work[kscomm_1.iscl + i__ - 1];

/*<         do 410 l = 1,nobj >*/
	i__2 = kscomm_1.nobj;
	for (l = 1; l <= i__2; ++l) {
/*<           j = (i - 1) * nobj + l >*/
		j = (i__ - 1) * kscomm_1.nobj + l;
/*<           work(idobj + j - 1) = df(l,i) * ss >*/
		work[kscomm_1.idobj + j - 1] = df[l + i__ * df_dim1] * ss;
/*<   410   continue >*/
/* L410: */
	}

/*<         if (ncon .eq. 0) go to 430 >*/
	if (kscomm_1.ncon == 0) {
		goto L430;
	}
/*<         do 420 k = 1,ncon >*/
	i__2 = kscomm_1.ncon;
	for (k = 1; k <= i__2; ++k) {
/*<           j = (i - 1) * ncon + k >*/
		j = (i__ - 1) * kscomm_1.ncon + k;
/*<           work(idg + j - 1) = dg(k,i) * ss >*/
		work[kscomm_1.idg + j - 1] = dg[k + i__ * dg_dim1] * ss;
/*<   420   continue >*/
/* L420: */
	}
/*<   430 continue >*/
L430:
	;
	}

/*          compute gradient of unconstrained function */
/*          to be minimizied */

/*<   440 continue >*/
L440:
/*<    >*/
	ksdfun_(&work[kscomm_1.idf], &work[kscomm_1.iobj0], &work[kscomm_1.ifscl],
		&work[kscomm_1.ifoff], &work[kscomm_1.idobj], &work[kscomm_1.ig0]
		, &work[kscomm_1.idg], &kscomm_1.rho, &kscomm_1.ndv, &
		kscomm_1.ncon, &kscomm_1.nobj, &work[kscomm_1.itmp1], &work[
		kscomm_1.itmp2], &nodim, &ncdim);

/*          compute an initial or restart hessian matrix */

/*<    >*/
	kshess_(&work[kscomm_1.ihess], &work[kscomm_1.iobj0], &work[
		kscomm_1.ifscl], &work[kscomm_1.ifoff], &work[kscomm_1.idobj], &
		work[kscomm_1.ig0], &work[kscomm_1.idg], &kscomm_1.rho, &
		kscomm_1.ndv, &kscomm_1.ncon, &kscomm_1.nobj, &work[
		kscomm_1.itmp1], &work[kscomm_1.itmp2], &nodim, &ncdim);

/*          check for side constraint violation */

/*<    >*/
	ksside_(&work[kscomm_1.isx], &work[kscomm_1.isxlb], &work[kscomm_1.isxub],
		&work[kscomm_1.iside], &work[kscomm_1.idf], &kscomm_1.ndv, &
		kscomm_1.nside);

/*          compute search direction */

/*<    >*/
	ksdfp_(&work[kscomm_1.isx], &work[kscomm_1.iside], &work[kscomm_1.isact],
		&work[kscomm_1.idf], &kscomm_1.ndv, &work[kscomm_1.islp], &
		kscomm_1.slope, &work[kscomm_1.iy], &work[kscomm_1.ip], &work[
		kscomm_1.ih], &work[kscomm_1.ihess], &kscomm_1.isdflg);

/*          print beginning of iteration information */

/*<       ipflag = 1 >*/
	ipflag = 1;
/*<    >*/
	ksprnt_(&ipflag, &iprnt1, &iprnt2, &x[1], &work[kscomm_1.iobj0], &work[
		kscomm_1.ig0], &work[kscomm_1.idobj], &work[kscomm_1.idg], &work[
		kscomm_1.iside], &work[kscomm_1.iscl], &nodim, &ncdim, &work[
		kscomm_1.itmp1], &work[1]);

/*          compute unconstrained function to be minimized */

/*<   500 continue >*/
L500:
/*<       if (inext .eq. 0) go to 510 >*/
	if (kscomm_1.inext == 0) {
	goto L510;
	}
/*<    >*/
	ksfun_(&fun, &obj[1], &g[1], &kscomm_1.rho, &work[kscomm_1.ifscl], &work[
		kscomm_1.ifoff], &kscomm_1.ncon, &kscomm_1.nobj, &work[
		kscomm_1.itmp1]);
/*<   510 continue >*/
L510:

/*          perform one-dimensional search */

/*<       jsel = 5 >*/
	kscomm_1.jsel = 5;
/*<       isel = 1 >*/
	*isel = 1;
/*<    >*/
	ksoned_(&kscomm_1.inext, &kscomm_1.jnext, &work[kscomm_1.isx], &work[
		kscomm_1.isx0], &work[kscomm_1.isxlb], &work[kscomm_1.isxub], &
		fun, &kscomm_1.fun0, &work[kscomm_1.islp], &kscomm_1.slope, &
		kscomm_1.alpha, &kscomm_1.alpmax, &kscomm_1.ndv, &kscomm_1.a1, &
		kscomm_1.a2, &kscomm_1.a3, &kscomm_1.a4, &kscomm_1.f1, &
		kscomm_1.f2, &kscomm_1.f3, &kscomm_1.f4, &kscomm_1.alim, &
		kscomm_1.atest, &kscomm_1.ftest, &kscomm_1.nside, &kscomm_1.limit,
		&kscomm_1.nunit, &iprnt3, &work[kscomm_1.iscl], &work[
		kscomm_1.itmp1], &kscomm_1.isdflg);
/*<       if (inext .ne. 0) go to 900 >*/
	if (kscomm_1.inext != 0) {
	goto L900;
	}
/*<       jsel = 6 >*/
	kscomm_1.jsel = 6;
/*<       isel = 1 >*/
	*isel = 1;
/*<       go to 900 >*/
	goto L900;

/*          test for termination criteria */

/*<   600 continue >*/
L600:
/*<    >*/
	ksfun_(&fun, &obj[1], &g[1], &kscomm_1.rho, &work[kscomm_1.ifscl], &work[
		kscomm_1.ifoff], &kscomm_1.ncon, &kscomm_1.nobj, &work[
		kscomm_1.itmp1]);
/*<       if (itcnt .ge. itmax) go to 700 >*/
	if (kscomm_1.itcnt >= kscomm_1.itmax) {
	goto L700;
	}
/*<       af    = abs(fun) >*/
	af = abs(fun);
/*<       adelf = abs(fun - fun0) >*/
	adelf = (d__1 = fun - kscomm_1.fun0, abs(d__1));
/*<       rdelf = adelf >*/
	rdelf = adelf;
/*<       if (af .gt. 1.0e-12) rdelf = adelf / af >*/
	if (af > 1e-12f) {
	rdelf = adelf / af;
	}
/*<       icntr = icntr + 1 >*/
	++kscomm_1.icntr;
/*<       icnta = icnta + 1 >*/
	++kscomm_1.icnta;
/*<       if (rdelf .ge. rdf) icntr = 0 >*/
	if (rdelf >= kscomm_1.rdf) {
	kscomm_1.icntr = 0;
	}
/*<       if (adelf .ge. adf) icnta = 0 >*/
	if (adelf >= kscomm_1.adf) {
	kscomm_1.icnta = 0;
	}
/*<       if (icntr .ge. 3 .or. icnta .ge. 3) go to 700 >*/
	if (kscomm_1.icntr >= 3 || kscomm_1.icnta >= 3) {
	goto L700;
	}

/*          go to next iteration */

/*<       itcnt = itcnt + 1 >*/
	++kscomm_1.itcnt;

/*          increment rho when necessary */

/*<       if (icntr .eq. 0 .and. icnta .eq. 0) go to 650 >*/
	if (kscomm_1.icntr == 0 && kscomm_1.icnta == 0) {
	goto L650;
	}
/*<       rho = rho + drho >*/
	kscomm_1.rho += kscomm_1.drho;
/*<       if (rho .gt. rhomax) rho = rhomax >*/
	if (kscomm_1.rho > kscomm_1.rhomax) {
	kscomm_1.rho = kscomm_1.rhomax;
	}
/*<   650 continue >*/
L650:

/*          set flag for hessian matrix restart */

/*<       isdflg = isdflg + 1 >*/
	++kscomm_1.isdflg;
/*<       if (isdflg .gt. isdrst) isdflg = 0 >*/
	if (kscomm_1.isdflg > kscomm_1.isdrst) {
	kscomm_1.isdflg = 0;
	}

/*          re-scale when necessary */

/*<       iscale = 0 >*/
	iscale = 0;
/*<       if (nscale .gt. 0) iscale = nscale >*/
	if (kscomm_1.nscale > 0) {
	iscale = kscomm_1.nscale;
	}
/*<       if (iscale .ne. 0) itflag = mod(itcnt - 1,iscale) >*/
	if (iscale != 0) {
	itflag = (kscomm_1.itcnt - 1) % iscale;
	}
/*<       if (iscale .eq. 0) itflag = 1 >*/
	if (iscale == 0) {
	itflag = 1;
	}

/*          currently re-scale only when hessian matrix is reset */
/*          28 March 1991  g.a.w. */

/*<       itflag = 1 >*/
	itflag = 1;
/*<       if (isdflg .eq. 0) itflag = 0 >*/
	if (kscomm_1.isdflg == 0) {
	itflag = 0;
	}
/*<    >*/
	if (itflag == 0) {
	ksscal_(&work[kscomm_1.isx], &work[kscomm_1.isx0], &work[
		kscomm_1.isxlb], &work[kscomm_1.isxub], &work[kscomm_1.iscl],
		&kscomm_1.ndv, &kscomm_1.nside, &kscomm_1.nscale);
	}
/*<       go to 100 >*/
	goto L100;

/*          terminate */

/*<   700 continue >*/
L700:
/*<       isel = 0 >*/
	*isel = 0;

/*<       ipflag = 2 >*/
	ipflag = 2;
/*<    >*/
	ksprnt_(&ipflag, &iprnt1, &iprnt2, &x[1], &obj[1], &g[1], &work[
		kscomm_1.idobj], &work[kscomm_1.idg], &work[kscomm_1.iside], &
		work[kscomm_1.iscl], &nodim, &ncdim, &work[kscomm_1.itmp1], &work[
		1]);

/*          un-scale design variables and return to calling routine */

/*<   900 continue >*/
L900:
/*<       call ksxlim (work(isx),work(isxlb),work(isxub),ndv,nside) >*/
	ksxlim_(&work[kscomm_1.isx], &work[kscomm_1.isxlb], &work[kscomm_1.isxub],
		&kscomm_1.ndv, &kscomm_1.nside);
/*<       call ksunsc (x,work(isx),work(iscl),ndv) >*/
	ksunsc_(&x[1], &work[kscomm_1.isx], &work[kscomm_1.iscl], &kscomm_1.ndv);
/*<       if (isel .eq. 1) ifncl = ifncl + 1 >*/
	if (*isel == 1) {
	++kscomm_1.ifncl;
	}
/*<       call kscomp (work(1)) >*/
	kscomp_(&work[1]);
/*<       return >*/
	return 0;

/*<       end >*/
} /* ksopt_ */