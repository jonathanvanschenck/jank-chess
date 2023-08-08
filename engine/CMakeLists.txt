cmake_minimum_required(VERSION 3.10)

# Project
project(JankChess VERSION 0.1.0 LANGUAGES CXX)
set(PROJECT_DESCRIPTION "A c++ chess engine for fun and profit!")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)

# Project options
option(JC_BUILD_PERFT "Build perft" ON)
option(JC_BUILD_DEV "Build dev" ON)

add_subdirectory(include)
add_subdirectory(src)

add_library(jankchess SHARED ${JC_SOURCES})

add_subdirectory(apps)
add_subdirectory(engines)

target_include_directories(jankchess
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/>)
