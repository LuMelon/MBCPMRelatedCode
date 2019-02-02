
BOOST_DIR=../externalpackages/
UTILITIES_DIR=../utilities
DATA_DIR=../data
MODEL_DIR=../model
LOSS_DIR=../loss
SOLVER_DIR=../solver
INNER_SOLVER_DIR=../solver/MBCPMinnersolver

include ${LOSS_DIR}/loss.mk
include ${DATA_DIR}/data.mk
include ${SOLVER_DIR}/solver.mk
include ${UTILITIES_DIR}/utilities.mk
include ${MODEL_DIR}/model.mk

TRAINER = mbcpm-train

FACTORY_SRC = ${LOSS_DIR}/lossfactory.hpp ${DATA_DIR}/datafactory.hpp\
	      ${SOLVER_DIR}/solverfactory.hpp ${INNER_SOLVER_DIR}/mbcpminnersolverfactory.hpp


INCLUDE_DIR = -I${DATA_DIR} -I${MODEL_DIR} -I${UTILITIES_DIR} -I${LOSS_DIR}\
	      -I${SOLVER_DIR} -I${INNER_SOLVER_DIR} -I${BOOST_DIR}

ALL_OBJECTS = ${UTILITIES_OBJ} ${DATA_OBJ} ${MODEL_OBJ} ${LOSS_OBJ} ${SOLVER_OBJ} ${INNER_SOLVER_OBJ}

CXX = g++

### for debugging ###
CFLAGS = -Wall -O0 -g -DNDEBUG  ${INCLUDE_DIR}

### for release ###
#CFLAGS = -Wall -O3 -DNDEBUG ${INCLUDE_DIR}

LDFLAGS = -lglog 

.PHONY: all

all: mbcpm-train mbcpm-predict

# executable
mbcpm-train: mbcpm-train.cpp ${ALL_OBJECTS} ${FACTORY_SRC}
	${CXX} ${CFLAGS} ${LDFLAGS} mbcpm-train.cpp -o ${TRAINER} ${ALL_OBJECTS}

mbcpm-predict: mbcpm-predict.cpp ${ALL_OBJECTS} ${FACTORY_SRC}
	${CXX} ${CFLAGS} ${LDFLAGS} mbcpm-predict.cpp -o mbcpm-predict ${ALL_OBJECTS}


clean:
	rm *.o *-train mbcpm-predict;

