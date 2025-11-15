cmake_minimum_required(VERSION 3.25)
project(tests)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

include_directories(${gtest_SOURCE_DIR}/include)

add_library(mock_syscalls mock_syscalls.cpp)

target_include_directories(mock_syscalls PUBLIC
    ${gtest_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/drivers/SPI/spi_driver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test_spi.cpp
)

# target_sources(${PROJECT_NAME} PRIVATE
#     ${CMAKE_SOURCE_DIR}/src/drivers/SPI/spi_driver.c
# )

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/drivers
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/inc
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    gtest_main
    mock_syscalls
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
