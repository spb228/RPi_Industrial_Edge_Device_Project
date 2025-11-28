cmake_minimum_required(VERSION 3.25)
project(utilities)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ring_buffer/ring_buffer.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(${PROJECT_NAME} PRIVATE inc)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/inc
)

MESSAGE(STATUS "Done configuring ${PROJECT_NAME}")
