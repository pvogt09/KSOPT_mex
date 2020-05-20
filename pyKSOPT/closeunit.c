/* closeunit.f -- translated by f2c (version 20100827).
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

/* Helper subroutine to close files in the Fortran world */
/*<       subroutine closeunit(unitnum) >*/
/* Subroutine */ int closeunit_(integer *unitnum)
{
    /* System generated locals */
    cllist cl__1;

    /* Builtin functions */
    integer f_clos(cllist *);

/*<       integer unitnum >*/
/* f2py intent(in) unitnum */
/*<       close(unitnum) >*/
    cl__1.cerr = 0;
    cl__1.cunit = *unitnum;
    cl__1.csta = 0;
    f_clos(&cl__1);
/*<       return >*/
    return 0;
/*<       end >*/
} /* closeunit_ */

