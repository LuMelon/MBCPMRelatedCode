SOLVER_OBJ = Momentum.o

# NOTE:
# gfortran is required to compile the original bt fortran code
#

# solver objects
Momentum.o: ${SOLVER_DIR}/Momentum.hpp ${SOLVER_DIR}/Momentum.cpp
	${CXX} ${CFLAGS} -c ${SOLVER_DIR}/Momentum.cpp
