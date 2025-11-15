cmake_minimum_required(VERSION 3.25)
project(src)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/drivers drivers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sensors sensors)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/apps apps)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common common)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE
    drivers
    sensors
    #apps
    #common
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/        # everything from under src
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bin)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

MESSAGE(STATUS "Done configuring ${PROJECT_NAME}")