# SPDX-FileCopyrightText: 2017-2018 Christian Sailer
# SPDX-FileCopyrightText: 2017-2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.15.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS deployment version")
project(salatest)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
endif()

set (FORCE_GLOBAL_COMPILE_WARNINGS OFF)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(COMPILE_WARNINGS -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_compile_options(/EHsc /W4)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:strict")
endif()


# policy for target sources - we don't expect any old CMakes
cmake_policy(SET CMP0076 NEW)

include_directories(".")

# Allows enabling M_PI for MSVC
add_compile_definitions(_USE_MATH_DEFINES)

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE APP_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE APP_COMMIT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# generate version_defs.hpp
include_directories(${CMAKE_BINARY_DIR})
configure_file("${CMAKE_SOURCE_DIR}/version_defs.hpp.in" "${CMAKE_BINARY_DIR}/version_defs.hpp" @ONLY)

string(TIMESTAMP APP_DATE %Y-%m-%d)

#add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
#add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)

set(modules_core "" CACHE INTERNAL "modules_core" FORCE)
set(MODULES_GUI FALSE)
set(MODULES_CLI FALSE)
set(MODULES_CLI_TEST FALSE)
set(MODULES_CORE TRUE)
set(MODULES_CORE_TEST FALSE)

add_subdirectory(salalib)
add_subdirectory(modules) # only the core modules are loaded here
add_subdirectory(genlibTest)
add_subdirectory(salaTest)
add_subdirectory(dmcli)
add_subdirectory(dmcliTest)
add_subdirectory(moduleTest)

