cmake_minimum_required(VERSION 3.21)
project(ConcurrencyAssignment C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_executable(bank
        main.c main.h
        test.c test.h)
