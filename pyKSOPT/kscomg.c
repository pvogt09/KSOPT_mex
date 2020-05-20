/* kscomg.f -- translated by f2c (version 20100827).
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

/*<       subroutine kscomg (work) >*/
/* Subroutine */ int kscomg_(doublereal *work)
{
/*<       implicit double precision (a-h,o-z) >*/
/*<    >*/
/*<       dimension work(*) >*/

/*          copy variables from work array to common block kscomm */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 19 July 1996 */

/*<       rdf    = work(1) >*/
    /* Parameter adjustments */
    --work;

    /* Function Body */
    kscomm_1.rdf = work[1];
/*<       adf    = work(2) >*/
    kscomm_1.adf = work[2];
/*<       fdl    = work(3) >*/
    kscomm_1.fdl = work[3];
/*<       fdm    = work(4) >*/
    kscomm_1.fdm = work[4];
/*<       rho    = work(5) >*/
    kscomm_1.rho = work[5];
/*<       drho   = work(6) >*/
    kscomm_1.drho = work[6];
/*<       rhomax = work(7) >*/
    kscomm_1.rhomax = work[7];
/*<       fun0   = work(8) >*/
    kscomm_1.fun0 = work[8];
/*<       slope  = work(9) >*/
    kscomm_1.slope = work[9];
/*<       delx   = work(10) >*/
    kscomm_1.delx = work[10];
/*<       alpha  = work(11) >*/
    kscomm_1.alpha = work[11];
/*<       alpmax = work(12) >*/
    kscomm_1.alpmax = work[12];
/*<       a1     = work(13) >*/
    kscomm_1.a1 = work[13];
/*<       a2     = work(14) >*/
    kscomm_1.a2 = work[14];
/*<       a3     = work(15) >*/
    kscomm_1.a3 = work[15];
/*<       a4     = work(16) >*/
    kscomm_1.a4 = work[16];
/*<       f1     = work(17) >*/
    kscomm_1.f1 = work[17];
/*<       f2     = work(18) >*/
    kscomm_1.f2 = work[18];
/*<       f3     = work(19) >*/
    kscomm_1.f3 = work[19];
/*<       f4     = work(20) >*/
    kscomm_1.f4 = work[20];
/*<       alim   = work(21) >*/
    kscomm_1.alim = work[21];
/*<       atest  = work(22) >*/
    kscomm_1.atest = work[22];
/*<       ftest  = work(23) >*/
    kscomm_1.ftest = work[23];
/*<       ifscl  = int(work(24)) >*/
    kscomm_1.ifscl = (integer) work[24];
/*<       ifoff  = int(work(25)) >*/
    kscomm_1.ifoff = (integer) work[25];
/*<       isx    = int(work(26)) >*/
    kscomm_1.isx = (integer) work[26];
/*<       isx0   = int(work(27)) >*/
    kscomm_1.isx0 = (integer) work[27];
/*<       isxlb  = int(work(28)) >*/
    kscomm_1.isxlb = (integer) work[28];
/*<       isxub  = int(work(29)) >*/
    kscomm_1.isxub = (integer) work[29];
/*<       iscl   = int(work(30)) >*/
    kscomm_1.iscl = (integer) work[30];
/*<       ig0    = int(work(31)) >*/
    kscomm_1.ig0 = (integer) work[31];
/*<       idf    = int(work(32)) >*/
    kscomm_1.idf = (integer) work[32];
/*<       islp   = int(work(33)) >*/
    kscomm_1.islp = (integer) work[33];
/*<       iobj0  = int(work(34)) >*/
    kscomm_1.iobj0 = (integer) work[34];
/*<       iy     = int(work(35)) >*/
    kscomm_1.iy = (integer) work[35];
/*<       ip     = int(work(36)) >*/
    kscomm_1.ip = (integer) work[36];
/*<       ih     = int(work(37)) >*/
    kscomm_1.ih = (integer) work[37];
/*<       ihess  = int(work(38)) >*/
    kscomm_1.ihess = (integer) work[38];
/*<       iside  = int(work(39)) >*/
    kscomm_1.iside = (integer) work[39];
/*<       isact  = int(work(40)) >*/
    kscomm_1.isact = (integer) work[40];
/*<       idobj  = int(work(41)) >*/
    kscomm_1.idobj = (integer) work[41];
/*<       idg    = int(work(42)) >*/
    kscomm_1.idg = (integer) work[42];
/*<       itmp1  = int(work(43)) >*/
    kscomm_1.itmp1 = (integer) work[43];
/*<       itmp2  = int(work(44)) >*/
    kscomm_1.itmp2 = (integer) work[44];
/*<       inext  = int(work(45)) >*/
    kscomm_1.inext = (integer) work[45];
/*<       jnext  = int(work(46)) >*/
    kscomm_1.jnext = (integer) work[46];
/*<       jsel   = int(work(47)) >*/
    kscomm_1.jsel = (integer) work[47];
/*<       itcnt  = int(work(48)) >*/
    kscomm_1.itcnt = (integer) work[48];
/*<       icntr  = int(work(49)) >*/
    kscomm_1.icntr = (integer) work[49];
/*<       icnta  = int(work(50)) >*/
    kscomm_1.icnta = (integer) work[50];
/*<       isdflg = int(work(51)) >*/
    kscomm_1.isdflg = (integer) work[51];
/*<       isdrst = int(work(52)) >*/
    kscomm_1.isdrst = (integer) work[52];
/*<       ifncl  = int(work(53)) >*/
    kscomm_1.ifncl = (integer) work[53];
/*<       nunit  = int(work(54)) >*/
    kscomm_1.nunit = (integer) work[54];
/*<       ndv    = int(work(55)) >*/
    kscomm_1.ndv = (integer) work[55];
/*<       ncon   = int(work(56)) >*/
    kscomm_1.ncon = (integer) work[56];
/*<       nobj   = int(work(57)) >*/
    kscomm_1.nobj = (integer) work[57];
/*<       nside  = int(work(58)) >*/
    kscomm_1.nside = (integer) work[58];
/*<       nscale = int(work(59)) >*/
    kscomm_1.nscale = (integer) work[59];
/*<       iprnt  = int(work(60)) >*/
    kscomm_1.iprnt = (integer) work[60];
/*<       itmax  = int(work(61)) >*/
    kscomm_1.itmax = (integer) work[61];
/*<       igrad  = int(work(62)) >*/
    kscomm_1.igrad = (integer) work[62];
/*<       limit  = int(work(63)) >*/
    kscomm_1.limit = (integer) work[63];

/*<       return >*/
    return 0;
/*<       end >*/
} /* kscomg_ */

