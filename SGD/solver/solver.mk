SOLVER_OBJ = SGD.o

# NOTE:
# gfortran is required to compile the original bt fortran code
#

# solver objects
SGD.o: ${SOLVER_DIR}/SGD.hpp ${SOLVER_DIR}/SGD.cpp
	${CXX} ${CFLAGS} -c ${SOLVER_DIR}/SGD.cpp
