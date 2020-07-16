/* kscomp.f -- translated by f2c (version 20100827).
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

/*<       subroutine kscomp (work) >*/
/* Subroutine */ int kscomp_(doublereal *work)
{
/*<       implicit double precision (a-h,o-z) >*/
/*<    >*/
/*<       dimension work(*) >*/

/*          copy variables from common block kscomm to work array */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 19 July 1996 */

/*<       work(1)  = rdf >*/
	/* Parameter adjustments */
	--work;

	/* Function Body */
	work[1] = kscomm_1.rdf;
/*<       work(2)  = adf >*/
	work[2] = kscomm_1.adf;
/*<       work(3)  = fdl >*/
	work[3] = kscomm_1.fdl;
/*<       work(4)  = fdm >*/
	work[4] = kscomm_1.fdm;
/*<       work(5)  = rho >*/
	work[5] = kscomm_1.rho;
/*<       work(6)  = drho >*/
	work[6] = kscomm_1.drho;
/*<       work(7)  = rhomax >*/
	work[7] = kscomm_1.rhomax;
/*<       work(8)  = fun0 >*/
	work[8] = kscomm_1.fun0;
/*<       work(9)  = slope >*/
	work[9] = kscomm_1.slope;
/*<       work(10) = delx >*/
	work[10] = kscomm_1.delx;
/*<       work(11) = alpha >*/
	work[11] = kscomm_1.alpha;
/*<       work(12) = alpmax >*/
	work[12] = kscomm_1.alpmax;
/*<       work(13) = a1 >*/
	work[13] = kscomm_1.a1;
/*<       work(14) = a2 >*/
	work[14] = kscomm_1.a2;
/*<       work(15) = a3 >*/
	work[15] = kscomm_1.a3;
/*<       work(16) = a4 >*/
	work[16] = kscomm_1.a4;
/*<       work(17) = f1 >*/
	work[17] = kscomm_1.f1;
/*<       work(18) = f2 >*/
	work[18] = kscomm_1.f2;
/*<       work(19) = f3 >*/
	work[19] = kscomm_1.f3;
/*<       work(20) = f4 >*/
	work[20] = kscomm_1.f4;
/*<       work(21) = alim >*/
	work[21] = kscomm_1.alim;
/*<       work(22) = atest >*/
	work[22] = kscomm_1.atest;
/*<       work(23) = ftest >*/
	work[23] = kscomm_1.ftest;
/*<       work(24) = float(ifscl) >*/
	work[24] = (real) kscomm_1.ifscl;
/*<       work(25) = float(ifoff) >*/
	work[25] = (real) kscomm_1.ifoff;
/*<       work(26) = float(isx) >*/
	work[26] = (real) kscomm_1.isx;
/*<       work(27) = float(isx0) >*/
	work[27] = (real) kscomm_1.isx0;
/*<       work(28) = float(isxlb) >*/
	work[28] = (real) kscomm_1.isxlb;
/*<       work(29) = float(isxub) >*/
	work[29] = (real) kscomm_1.isxub;
/*<       work(30) = float(iscl) >*/
	work[30] = (real) kscomm_1.iscl;
/*<       work(31) = float(ig0) >*/
	work[31] = (real) kscomm_1.ig0;
/*<       work(32) = float(idf) >*/
	work[32] = (real) kscomm_1.idf;
/*<       work(33) = float(islp) >*/
	work[33] = (real) kscomm_1.islp;
/*<       work(34) = float(iobj0) >*/
	work[34] = (real) kscomm_1.iobj0;
/*<       work(35) = float(iy) >*/
	work[35] = (real) kscomm_1.iy;
/*<       work(36) = float(ip) >*/
	work[36] = (real) kscomm_1.ip;
/*<       work(37) = float(ih) >*/
	work[37] = (real) kscomm_1.ih;
/*<       work(38) = float(ihess) >*/
	work[38] = (real) kscomm_1.ihess;
/*<       work(39) = float(iside) >*/
	work[39] = (real) kscomm_1.iside;
/*<       work(40) = float(isact) >*/
	work[40] = (real) kscomm_1.isact;
/*<       work(41) = float(idobj) >*/
	work[41] = (real) kscomm_1.idobj;
/*<       work(42) = float(idg) >*/
	work[42] = (real) kscomm_1.idg;
/*<       work(43) = float(itmp1) >*/
	work[43] = (real) kscomm_1.itmp1;
/*<       work(44) = float(itmp2) >*/
	work[44] = (real) kscomm_1.itmp2;
/*<       work(45) = float(inext) >*/
	work[45] = (real) kscomm_1.inext;
/*<       work(46) = float(jnext) >*/
	work[46] = (real) kscomm_1.jnext;
/*<       work(47) = float(jsel) >*/
	work[47] = (real) kscomm_1.jsel;
/*<       work(48) = float(itcnt) >*/
	work[48] = (real) kscomm_1.itcnt;
/*<       work(49) = float(icntr) >*/
	work[49] = (real) kscomm_1.icntr;
/*<       work(50) = float(icnta) >*/
	work[50] = (real) kscomm_1.icnta;
/*<       work(51) = float(isdflg) >*/
	work[51] = (real) kscomm_1.isdflg;
/*<       work(52) = float(isdrst) >*/
	work[52] = (real) kscomm_1.isdrst;
/*<       work(53) = float(ifncl) >*/
	work[53] = (real) kscomm_1.ifncl;
/*<       work(54) = float(nunit) >*/
	work[54] = (real) kscomm_1.nunit;
/*<       work(55) = float(ndv) >*/
	work[55] = (real) kscomm_1.ndv;
/*<       work(56) = float(ncon) >*/
	work[56] = (real) kscomm_1.ncon;
/*<       work(57) = float(nobj) >*/
	work[57] = (real) kscomm_1.nobj;
/*<       work(58) = float(nside) >*/
	work[58] = (real) kscomm_1.nside;
/*<       work(59) = float(nscale) >*/
	work[59] = (real) kscomm_1.nscale;
/*<       work(60) = float(iprnt) >*/
	work[60] = (real) kscomm_1.iprnt;
/*<       work(61) = float(itmax) >*/
	work[61] = (real) kscomm_1.itmax;
/*<       work(62) = float(igrad) >*/
	work[62] = (real) kscomm_1.igrad;
/*<       work(63) = float(limit) >*/
	work[63] = (real) kscomm_1.limit;

/*<       return >*/
	return 0;
/*<       end >*/
} /* kscomp_ */