cmake_minimum_required(VERSION 3.25)
project(src)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/drivers drivers)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE
    drivers
    #sensors
    #utils
) 

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

MESSAGE (STATUS "Done configuring ${PROJECT_NAME}")