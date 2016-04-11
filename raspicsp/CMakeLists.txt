cmake_minimum_required(VERSION 2.8)
project(raspicsp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11")
set(SOURCE_FILES main.cpp HAL.cpp HAL.h PIC24.cpp PIC24.h ICSP.cpp ICSP.h devices.h Logger.cpp Logger.h HexFile.cpp HexFile.h)
add_executable(raspicsp ${SOURCE_FILES})