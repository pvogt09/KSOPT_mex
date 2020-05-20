/* ksinfo.f -- translated by f2c (version 20100827).
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

/*<       subroutine ksinfo (iout,rout) >*/
/* Subroutine */ int ksinfo_(integer *iout, doublereal *rout)
{
/*<       implicit double precision (a-h,o-z) >*/
/*<    >*/
/*<       dimension iout(15),rout(25) >*/

/*          This user called routine returns the current values of */
/*          several common block variables to satisfy the curiosity */
/*          of the user.  This routine may be called at any time after */
/*          routine ksinit is executed. */

/*          author   - Gregory A. Wrenn */
/*          location - Lockheed Engineering and Sciences Co. */
/*                     144 Research Drive */
/*                     Hampton, Va. 23666 */

/*          last modification - 30 August 1991 */

/*<       iout(1)  = inext >*/
    /* Parameter adjustments */
    --rout;
    --iout;

    /* Function Body */
    iout[1] = kscomm_1.inext;
/*<       iout(2)  = jnext >*/
    iout[2] = kscomm_1.jnext;
/*<       iout(3)  = jsel >*/
    iout[3] = kscomm_1.jsel;
/*<       iout(4)  = itcnt >*/
    iout[4] = kscomm_1.itcnt;
/*<       iout(5)  = icntr >*/
    iout[5] = kscomm_1.icntr;
/*<       iout(6)  = icnta >*/
    iout[6] = kscomm_1.icnta;
/*<       iout(7)  = isdflg >*/
    iout[7] = kscomm_1.isdflg;
/*<       iout(8)  = isdrst >*/
    iout[8] = kscomm_1.isdrst;
/*<       iout(9)  = ifncl >*/
    iout[9] = kscomm_1.ifncl;
/*<       iout(10) = limit >*/
    iout[10] = kscomm_1.limit;
/*<       iout(11) = -1 >*/
    iout[11] = -1;
/*<       iout(12) = -1 >*/
    iout[12] = -1;
/*<       iout(13) = -1 >*/
    iout[13] = -1;
/*<       iout(14) = -1 >*/
    iout[14] = -1;
/*<       iout(15) = -1 >*/
    iout[15] = -1;

/*<       rout(1)  = rho >*/
    rout[1] = kscomm_1.rho;
/*<       rout(2)  = drho >*/
    rout[2] = kscomm_1.drho;
/*<       rout(3)  = fun0 >*/
    rout[3] = kscomm_1.fun0;
/*<       rout(4)  = slope >*/
    rout[4] = kscomm_1.slope;
/*<       rout(5)  = delx >*/
    rout[5] = kscomm_1.delx;
/*<       rout(6)  = alpha >*/
    rout[6] = kscomm_1.alpha;
/*<       rout(7)  = alpmax >*/
    rout[7] = kscomm_1.alpmax;
/*<       rout(8)  = a1 >*/
    rout[8] = kscomm_1.a1;
/*<       rout(9)  = a2 >*/
    rout[9] = kscomm_1.a2;
/*<       rout(10) = a3 >*/
    rout[10] = kscomm_1.a3;
/*<       rout(11) = a4 >*/
    rout[11] = kscomm_1.a4;
/*<       rout(12) = f1 >*/
    rout[12] = kscomm_1.f1;
/*<       rout(13) = f2 >*/
    rout[13] = kscomm_1.f2;
/*<       rout(14) = f3 >*/
    rout[14] = kscomm_1.f3;
/*<       rout(15) = f4 >*/
    rout[15] = kscomm_1.f4;
/*<       rout(16) = alim >*/
    rout[16] = kscomm_1.alim;
/*<       rout(17) = atest >*/
    rout[17] = kscomm_1.atest;
/*<       rout(18) = ftest >*/
    rout[18] = kscomm_1.ftest;
/*<       rout(19) = -1.0 >*/
    rout[19] = -1.f;
/*<       rout(20) = -1.0 >*/
    rout[20] = -1.f;
/*<       rout(21) = -1.0 >*/
    rout[21] = -1.f;
/*<       rout(22) = -1.0 >*/
    rout[22] = -1.f;
/*<       rout(23) = -1.0 >*/
    rout[23] = -1.f;
/*<       rout(24) = -1.0 >*/
    rout[24] = -1.f;
/*<       rout(25) = -1.0 >*/
    rout[25] = -1.f;

/*<       return >*/
    return 0;
/*<       end >*/
} /* ksinfo_ */

