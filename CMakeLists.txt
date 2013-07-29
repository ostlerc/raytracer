PROJECT( Specter )

FILE( GLOB SRC_FILES *.cc *.h *.c)
ADD_EXECUTABLE( specter ${SRC_FILES} )

SET( CMAKE_CXX_FLAGS_RELEASE "-Wall -fopenmp -g -std=c++11 -pthread -O2" CACHE STRING "Optimized flags" FORCE )
LIST( APPEND CMAKE_CXX_FLAGS "-Wall -O2 -fopenmp -Werror -std=c++11 ${CMAKE_CXX_FLAGS} -g -pthread")

#no warnings for the ply parser c library
SET_SOURCE_FILES_PROPERTIES(rply.c PROPERTIES COMPILE_FLAGS -w)
SET_SOURCE_FILES_PROPERTIES(rply.c PROPERTIES LANGUAGE C)
