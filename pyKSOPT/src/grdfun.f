	SUBROUTINE GRDFUN (NDV,NOBJ,NCON,X,OBJ,G,DF,DG)
		IMPLICIT DOUBLE PRECISION(A-H,O-Z)
		DIMENSION X(NDV)
		DIMENSION OBJ(NOBJ)
		DIMENSION G(NCON)
		DIMENSION DF(NOBJ,NDV)
		DIMENSION DG(NCON,NDV)
		
		INTEGER i, j
		
C	integer intent(in) :: ndv
C	integer intent(in) :: nobj
C	integer intent(in) :: ncon
C	double precision dimension(ndv),intent(in),depend(ndv) :: x
C	double precision dimension(nobj),intent(in),depend(nobj) :: obj
C	double precision dimension(ncon),intent(in),depend(ncon) :: g
C	double precision dimension(nobj,ndv),intent(in,out),depend(nobj,ndv) :: df
C	double precision dimension(ncon,ndv),intent(in,out),depend(ncon,ndv) :: dg

		DO i = 1, NOBJ
			OBJ(i) = 0
			DO j = 1, NDV
				DF(i, j) =  0
			END DO
		END DO

		DO i = 1, NCON
			G(i) = 0
			DO j = 1, NDV
				DG(i, j) =  0
			END DO
		END DO
		RETURN
	 END
	 END