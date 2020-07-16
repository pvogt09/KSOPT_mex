/* objfun.f -- translated by f2c (version 20100827).
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

/*< 	SUBROUTINE OBJFUN (NDV,NOBJ,NCON,X,OBJ,G) >*/
/* Subroutine */ int objfun_(integer *ndv, integer *nobj, integer *ncon,
	doublereal *x, doublereal *obj, doublereal *g)
{
	/* System generated locals */
	integer i__1;

	/* Local variables */
	static integer i__;

/*< 		IMPLICIT DOUBLE PRECISION(A-H,O-Z) >*/
/*< 		DIMENSION X(NDV) >*/
/*< 		DIMENSION OBJ(NOBJ) >*/
/*< 		DIMENSION G(NCON) >*/
/*< 		INTEGER i >*/
/* 	integer intent(in) :: ndv */
/* 	integer intent(in) :: nobj */
/* 	integer intent(in) :: ncon */
/* 	double precision dimension(ndv),intent(in),depend(ndv) :: x */
/* 	double precision dimension(nobj),intent(in,out),depend(nobj) :: obj */
/* 	double precision dimension(ncon),intent(in,out),depend(ncon) :: g */
/*< 		DO i = 1, NOBJ >*/
	/* Parameter adjustments */
	--x;
	--obj;
	--g;

	/* Function Body */
	i__1 = *nobj;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*< 			OBJ(i) = 0 >*/
	obj[i__] = 0.;
/*< 		END DO >*/
	}
/*< 		DO i = 1, NCON >*/
	i__1 = *ncon;
	for (i__ = 1; i__ <= i__1; ++i__) {
/*< 			G(i) = 0 >*/
	g[i__] = 0.;
/*< 		END DO >*/
	}
/*< 		RETURN >*/
	return 0;
/*< 	END >*/
} /* objfun_ */