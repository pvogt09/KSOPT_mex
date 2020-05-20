	SUBROUTINE OBJFUN (NDV,NOBJ,NCON,X,OBJ,G)
		IMPLICIT DOUBLE PRECISION(A-H,O-Z)
		DIMENSION X(NDV)
		DIMENSION OBJ(NOBJ)
		DIMENSION G(NCON)
		
		INTEGER i
		
C	integer intent(in) :: ndv
C	integer intent(in) :: nobj
C	integer intent(in) :: ncon
C	double precision dimension(ndv),intent(in),depend(ndv) :: x
C	double precision dimension(nobj),intent(in,out),depend(nobj) :: obj
C	double precision dimension(ncon),intent(in,out),depend(ncon) :: g

		DO i = 1, NOBJ
			OBJ(i) = 0
		END DO

		DO i = 1, NCON
			G(i) = 0
		END DO
		RETURN
	END
	END