cmake_minimum_required(VERSION 3.22)

# Project configuration

project(
    arcanoid
    VERSION 0.1.0
    DESCRIPTION "A simple 2D game based on the simple custom microengine"
    LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release" )
    message("=== RELEASE MODE ===")
    add_definitions( "-DRELEASE" )
    add_definitions( "-O2" )
else()
    message("=== DEBUG MODE ===")
    add_definitions( "-DDEBUG" )
endif()

# CMake stuff

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

# Check if `fmt` git submodule is initialized.

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../submodules/fmt)
  message(
    FATAL_ERROR
      "Submodule `fmt` is not initialized. "
      "Make sure you've cloned this repo with '--recurse-submodules' option.")
endif()

# Check if `SDL` git submodule is initialized.

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../submodules/sdl)
    message(
        FATAL_ERROR
        "Submodule `SDL` is not initialized. "
        "Make sure you've cloned this repo with '--recurse-submodules' option.")
endif()

# Check if `Glm` git submodule is initialized.

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../submodules/glm)
  message(
        FATAL_ERROR
        "Submodule `glm` is not initialized. "
        "Make sure you've cloned this repo with '--recurse-submodules' option.")
endif()

# Submodules

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../submodules/fmt
                 ${CMAKE_BINARY_DIR}/fmt)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../submodules/sdl
                 ${CMAKE_BINARY_DIR}/sdl)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../submodules/glm
                 ${CMAKE_BINARY_DIR}/glm)

# Game

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/engine/include
    ${PROJECT_SOURCE_DIR}/engine/external/imgui/include)

file(GLOB_RECURSE APP_SOURCES src/*.cxx)

add_executable(arcanoid ${APP_SOURCES})
target_compile_options(arcanoid PRIVATE "$<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wall;-Wextra;-Wpedantic;-Werror>")

include(AddModule)
add_module(arcanoid engine ${PROJECT_SOURCE_DIR}/engine)

# Resources

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/res")
