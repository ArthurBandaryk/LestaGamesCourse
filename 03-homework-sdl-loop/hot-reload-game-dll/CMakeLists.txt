cmake_minimum_required(VERSION 3.22)

project(hot-reload-dll)

# Check if `glog` git submodule is initialized.
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/glog)
  message(
    FATAL_ERROR
      "Submodule `glog` is not initialized. "
      "Make sure you've cloned this repo with '--recurse-submodules' option.")
endif()

# Check if `SDL` git submodule is initialized.
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/sdl)
  message(
    FATAL_ERROR
      "Submodule `SDL` is not initialized. "
      "Make sure you've cloned this repo with '--recurse-submodules' option.")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/glog
                 ${CMAKE_BINARY_DIR}/glog)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/sdl
                 ${CMAKE_BINARY_DIR}/sdl)

# Set clang, gcc compilers.
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")

# Create library for compiler flags in order to propagate compiler options to
# other dependents.
add_library(compiler-flags-lib INTERFACE)
target_compile_features(compiler-flags-lib INTERFACE cxx_std_17)
target_compile_options(
  compiler-flags-lib
  INTERFACE "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra>>")

add_executable(engine-bin engine.hxx engine.cxx)
target_link_libraries(engine-bin PRIVATE compiler-flags-lib glog::glog
                                         SDL3::SDL3-shared)

# Specify if the executable exports symbols for loadable modules.
# https://cmake.org/cmake/help/latest/prop_tgt/ENABLE_EXPORTS.html
set_target_properties(engine-bin PROPERTIES ENABLE_EXPORTS 1)

add_library(game-shared SHARED game.cxx)
target_link_libraries(game-shared PRIVATE compiler-flags-lib engine-bin
                                          glog::glog)
