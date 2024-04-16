cmake_minimum_required(VERSION 2.8)
project(Glacier)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC
    Glacier/log.cpp
    )

add_library(Glacier SHARED ${LIB_SRC})
#add_library(Glacier STATIC $(LIB_SRC))
#SET_TARGET_PROPERTIES (Glacier_static PROPERTIES OUTPUT_NAME "Glacier")

add_executable(test tests/test.cpp)
add_dependencies(test Glacier)
target_link_libraries(test Glacier)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
