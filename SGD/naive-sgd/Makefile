# Set to 1 if COIN Clp is installed and you would like to use L1 norm regularizer
COIN_CLP_AVAILABLE=0

BOOST_DIR=../externalpackages/
UTILITIES_DIR=../utilities
DATA_DIR=../data
MODEL_DIR=../model
LOSS_DIR=../loss
SOLVER_DIR=../solver

include ${LOSS_DIR}/loss.mk
include ${DATA_DIR}/data.mk
include ${SOLVER_DIR}/solver.mk
include ${UTILITIES_DIR}/utilities.mk
include ${MODEL_DIR}/model.mk

TRAINER = naive-sgd-train

FACTORY_SRC = ${LOSS_DIR}/lossfactory.hpp ${DATA_DIR}/datafactory.hpp\
	      ${SOLVER_DIR}/solverfactory.hpp 


INCLUDE_DIR = -I${DATA_DIR} -I${MODEL_DIR} -I${UTILITIES_DIR} -I${LOSS_DIR}\
	      -I${SOLVER_DIR} -I${BOOST_DIR}

ALL_OBJECTS = ${UTILITIES_OBJ} ${DATA_OBJ} ${MODEL_OBJ} ${LOSS_OBJ} ${SOLVER_OBJ}

CXX = g++

### for debugging ###
CFLAGS = -Wall -O0 -g -DNDEBUG  ${INCLUDE_DIR}

### for release ###
#CFLAGS = -Wall -O3 -DNDEBUG ${INCLUDE_DIR}

LDFLAGS = -lglog 

ifeq (${COIN_CLP_AVAILABLE},1)
	CFLAGS += ${COIN_CFLAGS} -DHAVE_L1N1_INNER_SOLVER
	LDFLAGS += ${COIN_LDFLAGS}
endif


.PHONY: all

all: naive-sgd-train naive-sgd-predict

# executable
naive-sgd-train: naive-sgd-train.cpp ${ALL_OBJECTS} ${FACTORY_SRC}
	${CXX} ${CFLAGS} ${LDFLAGS} naive-sgd-train.cpp -o ${TRAINER} ${ALL_OBJECTS}

naive-sgd-predict: naive-sgd-predict.cpp ${ALL_OBJECTS} ${FACTORY_SRC}
	${CXX} ${CFLAGS} ${LDFLAGS} naive-sgd-predict.cpp -o naive-sgd-predict ${ALL_OBJECTS}


clean:
	rm *.o naive-sgd-train naive-sgd-predict;

