cmake_minimum_required(VERSION 3.22)

project(hello-world-tasks)

# Creating simple binary.
add_subdirectory(01-1-simple-hello)

# Creating static and shared library and linking them to the main executable.
add_subdirectory(01-2-hello-libs)
