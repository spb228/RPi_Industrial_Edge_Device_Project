cmake_minimum_required(VERSION 3.25)
project(RPi_Industrial_Edge_Device_Project C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

#add_subdirectory(${CMAKE_SOURCE_DIR}/kernel kernel)
#add_subdirectory(${CMAKE_SOURCE_DIR}/inc inc)
add_subdirectory(${CMAKE_SOURCE_DIR}/src src)

MESSAGE(STATUS "Done configuring entire CMake project")
