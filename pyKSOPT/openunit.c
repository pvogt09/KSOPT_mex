/* openunit.f -- translated by f2c (version 20100827).
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

/* Helper subroutine to open files in the Fortran world */
/*<       subroutine openunit(unitnum,filename,filestatus,fileaction,ierror) >*/
/* Subroutine */ int openunit_(integer *unitnum, char *filename, char *
	filestatus, char *fileaction, integer *ierror, ftnlen filename_len, 
	ftnlen filestatus_len, ftnlen fileaction_len)
{
    /* System generated locals */
    olist o__1;

    /* Builtin functions */
    integer f_open(olist *);

/*<       integer unitnum >*/
/* f2py intent(in) unitnum */
/*<       character*(*) filename >*/
/* f2py intent(in) filename */
/*<       character*(*) filestatus >*/
/* f2py intent(in) filestatus */
/*<       character*(*) fileaction >*/
/* f2py intent(in) fileaction */
/*<       integer ierror >*/
/* f2py intent(out) ierror */
/*<    >*/
    o__1.oerr = 1;
    o__1.ounit = *unitnum;
    o__1.ofnmlen = filename_len;
    o__1.ofnm = filename;
    o__1.orl = 0;
    o__1.osta = filestatus;
    o__1.oacc = fileaction;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    *ierror = f_open(&o__1);
/*<       return >*/
    return 0;
/*<       end >*/
} /* openunit_ */

/* Helper routine to flush buffers to files */
/*<       subroutine pyflush(unitnum) >*/
/* Subroutine */ int pyflush_(integer *unitnum)
{
    extern /* Subroutine */ int flush_(integer *);

/*<       integer unitnum >*/
/*<       call flush(unitnum) >*/
    //flush_(unitnum);
/*<       return >*/
    return 0;
/*<       end >*/
} /* pyflush_ */

