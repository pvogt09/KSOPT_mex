/* grdfun.f -- translated by f2c (version 20100827).
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

/*< 	SUBROUTINE GRDFUN (NDV,NOBJ,NCON,X,OBJ,G,DF,DG) >*/
/* Subroutine */ int grdfun_(integer *ndv, integer *nobj, integer *ncon, 
	doublereal *x, doublereal *obj, doublereal *g, doublereal *df, 
	doublereal *dg)
{
    /* System generated locals */
    integer df_dim1, df_offset, dg_dim1, dg_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j;

/*< 		IMPLICIT DOUBLE PRECISION(A-H,O-Z) >*/
/*< 		DIMENSION X(NDV) >*/
/*< 		DIMENSION OBJ(NOBJ) >*/
/*< 		DIMENSION G(NCON) >*/
/*< 		DIMENSION DF(NOBJ,NDV) >*/
/*< 		DIMENSION DG(NCON,NDV) >*/
/*< 		INTEGER i, j >*/
/* 	integer intent(in) :: ndv */
/* 	integer intent(in) :: nobj */
/* 	integer intent(in) :: ncon */
/* 	double precision dimension(ndv),intent(in),depend(ndv) :: x */
/* 	double precision dimension(nobj),intent(in),depend(nobj) :: obj */
/* 	double precision dimension(ncon),intent(in),depend(ncon) :: g */
/* 	double precision dimension(nobj,ndv),intent(in,out),depend(nobj,ndv) :: df */
/* 	double precision dimension(ncon,ndv),intent(in,out),depend(ncon,ndv) :: dg */
/*< 		DO i = 1, NOBJ >*/
    /* Parameter adjustments */
    --x;
    df_dim1 = *nobj;
    df_offset = 1 + df_dim1;
    df -= df_offset;
    --obj;
    dg_dim1 = *ncon;
    dg_offset = 1 + dg_dim1;
    dg -= dg_offset;
    --g;

    /* Function Body */
    i__1 = *nobj;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*< 			OBJ(i) = 0 >*/
	obj[i__] = 0.;
/*< 			DO j = 1, NDV >*/
	i__2 = *ndv;
	for (j = 1; j <= i__2; ++j) {
/*< 				DF(i, j) =  0 >*/
	    df[i__ + j * df_dim1] = 0.;
/*< 			END DO >*/
	}
/*< 		END DO >*/
    }
/*< 		DO i = 1, NCON >*/
    i__1 = *ncon;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*< 			G(i) = 0 >*/
	g[i__] = 0.;
/*< 			DO j = 1, NDV >*/
	i__2 = *ndv;
	for (j = 1; j <= i__2; ++j) {
/*< 				DG(i, j) =  0 >*/
	    dg[i__ + j * dg_dim1] = 0.;
/*< 			END DO >*/
	}
/*< 		END DO >*/
    }
/*< 		RETURN >*/
    return 0;
/*< 	 END >*/
} /* grdfun_ */

