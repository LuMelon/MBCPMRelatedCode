SOLVER_OBJ = Adgrad.o

# NOTE:
# gfortran is required to compile the original bt fortran code
#

# solver objects
Adgrad.o: ${SOLVER_DIR}/Adgrad.hpp ${SOLVER_DIR}/Adgrad.cpp
	${CXX} ${CFLAGS} -c ${SOLVER_DIR}/Adgrad.cpp
