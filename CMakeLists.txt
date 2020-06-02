cmake_minimum_required(VERSION 3.15)
project(ex3c C)

set(CMAKE_C_STANDARD 99)

add_executable(ex3c RBTree.c RBTree.h Structs.h Structs.c utilities/RButilities.c
        utilities/RButilities.h tests.c)