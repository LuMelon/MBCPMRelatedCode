UTILITIES_OBJ = sml.o common.o configuration.o sgdexception.o timer.o

# utility objects
sml.o: ${UTILITIES_DIR}/sml.hpp ${UTILITIES_DIR}/sml.cpp
	${CXX} ${CFLAGS} -c ${UTILITIES_DIR}/sml.cpp

common.o: ${UTILITIES_DIR}/common.hpp ${UTILITIES_DIR}/common.cpp
	${CXX} ${CFLAGS} -c ${UTILITIES_DIR}/common.cpp

configuration.o: ${UTILITIES_DIR}/configuration.hpp ${UTILITIES_DIR}/configuration.cpp
	${CXX} ${CFLAGS} -c ${UTILITIES_DIR}/configuration.cpp

sgdexception.o: ${UTILITIES_DIR}/sgdexception.hpp ${UTILITIES_DIR}/sgdexception.cpp
	${CXX} ${CFLAGS} -c ${UTILITIES_DIR}/sgdexception.cpp

timer.o: ${UTILITIES_DIR}/timer.hpp ${UTILITIES_DIR}/timer.cpp
	${CXX} ${CFLAGS} -c ${UTILITIES_DIR}/timer.cpp
