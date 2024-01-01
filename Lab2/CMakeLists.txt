cmake_minimum_required(VERSION 2.8)
project(Lab2)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(Lab2 main.cpp
        ECE_PointCharge.cpp
        ECE_PointCharge.h
        ECE_ElectricField.cpp
        ECE_ElectricField.h)
