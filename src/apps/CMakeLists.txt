cmake_minimum_required(VERSION 3.25)
project(apps)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vib_sensor_acq/vib_sensor_acq.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    inc
    drivers
    sensors
    utilities
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..          # src dir to access drivers, sensors
    ${CMAKE_SOURCE_DIR}/inc
)

MESSAGE(STATUS "Done configuring ${PROJECT_NAME}")
