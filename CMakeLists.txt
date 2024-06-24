cmake_minimum_required(VERSION 3.29)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(awoonya)

include(cmake/cflags.cmake)

find_package(dpp CONFIG REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE)

target_link_libraries(${PROJECT_NAME} dpp::dpp)
