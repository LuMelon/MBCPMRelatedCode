SOLVER_OBJ = Adam.o

# NOTE:
# gfortran is required to compile the original bt fortran code
#

# solver objects
Adam.o: ${SOLVER_DIR}/Adam.hpp ${SOLVER_DIR}/Adam.cpp
	${CXX} ${CFLAGS} -c ${SOLVER_DIR}/Adam.cpp
