# Set minimum cmake version.
cmake_minimum_required(VERSION 3.22)

project(gamedev-tasks)

# First homework task consisting from two parts: simple hello binary and
# creating static/shared libs.
add_subdirectory(01-homework-hello)

# Second homework task: build and link sdl(static and dynamic).
add_subdirectory(02-homework-sdl-lib)
