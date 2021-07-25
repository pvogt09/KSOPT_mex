# For making f2c.lib (here called vcf2c.lib) with Microsoft Visual C++ .
# Invoke with "nmake -f makefile.vc" .

# To get signed zeros in write statements on IEEE-arithmetic systems,
# add -DSIGNED_ZEROS to the CFLAGS assignment below and add signbit.obj
# to the objects in the "w =" list below.

.SUFFIXES: .c .obj
CC = cc
ifndef MAKE_CFLAGS
	MAKE_CFLAGS = 
endif
CFLAGS = $(MAKE_CFLAGS) -O -fPIC

%.obj:
	test -f $*.c && $(CC) -c $(CFLAGS) $*.c || test -f $*.cpp && $(CC) -c $(CFLAGS) $*.cpp
	ld -r -x -o $*.xxx $*.o
	mv $*.xxx $*.obj

w = \
	closeunit.obj \
	grdfun.obj \
	ks.obj \
	ksando.obj \
	kscomg.obj \
	kscomp.obj \
	ksales.obj \
	kscube.obj \
	ksd.obj \
	ksdfp.obj \
	ksdfun.obj \
	ksfun.obj \
	ksgeterrormessage.obj \
	ksgrad.obj \
	kshess.obj \
	kshmul.obj \
	ksinfo.obj \
	ksinit.obj \
	ksmain.obj \
	ksoned.obj \
	ksopt.obj \
	ksprnt.obj \
	ksqmin.obj \
	ksquad.obj \
	ksscal.obj \
	ksside.obj \
	ksunsc.obj \
	ksvprd.obj \
	ksxlim.obj \
	objfun.obj \
	openunit.obj

wpp = \
	ksopt_optimizecpp.obj

all: ksopt.a

#f2ccopy:
#	xcopy "D:\Programme\f2c\libf2c\*" /exclude:makefile.vc
#	xcopy "D:\Programme\f2c\libf2c\makefile.vc" "makefilef2c.vc"
#	
#f2c: f2ccopy
#	nmake -f makefilef2c.vc

obj: $w $(wpp)

#cppobj: ksopt_optimizecpp.obj
#	$(CC) -c $(CFLAGS) ksopt_optimizecpp.cpp
#	ld -r -x -o ksopt_optimizecpp.xxx ksopt_optimizecpp.o
#	mv ksopt_optimizecpp.xxx ksopt_optimizecpp.obj

ksopt.a: obj cppobj
	ar r ksopt.a $?
 
