cmake_minimum_required(VERSION 3.25)
project(inc)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

set(INC_LIB_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/common_def.h
)

add_library(${PROJECT_NAME} STATIC 
    ${INC_LIB_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

MESSAGE (STATUS "Done configuring ${PROJECT_NAME}")
