cmake_minimum_required(VERSION 3.25)
project(drivers)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SPI/spi_driver.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SPI/
)

MESSAGE (STATUS "Done configuring ${PROJECT_NAME}")