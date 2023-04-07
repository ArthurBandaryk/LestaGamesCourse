cmake_minimum_required(VERSION 3.22)

project(linking-sdl)

# Check if `glog` git submodule is initialized. Check this:
# https://github.com/google/glog
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../submodules/glog)
  message(
    FATAL_ERROR
      "Submodule `glog` is not initialized. "
      "Make sure you've cloned this repo with '--recurse-submodules' option.")
endif()

# Check if `SDL` git submodule is initialized.
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../submodules/sdl)
  message(
    FATAL_ERROR
      "Submodule `SDL` is not initialized. "
      "Make sure you've cloned this repo with '--recurse-submodules' option.")
endif()

# We do here set CMAKE_INSTALL_PREFIX because `glog` cmake install everything to
# default directory (/usr/local/lib).
set(CMAKE_INSTALL_PREFIX
    ${CMAKE_BINARY_DIR}/../bin/install
    CACHE PATH "Set path for install mode" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../submodules/glog
                 ${CMAKE_BINARY_DIR}/glog)

# Set clang, gcc compilers.
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")

# I do not want SDL tests by default.
set(SDL_TEST OFF)

option(BUILD_SDL_STATIC "Get static SDL lib" OFF)

if(BUILD_SDL_STATIC)
  # Enable static build for SDL since by default it will be built only
  # dynamically.
  set(BUILD_SHARED_LIBS OFF)
  set(SDL_SHARED OFF)
  set(SDL_STATIC ON)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../submodules/sdl
                 ${CMAKE_BINARY_DIR}/sdl)

if(BUILD_SDL_STATIC)
  add_executable(sdl-bin-static main.cxx)
  target_link_libraries(sdl-bin-static glog::glog SDL3::SDL3-static)
  target_compile_features(sdl-bin-static PRIVATE cxx_std_17)
  target_compile_options(
    sdl-bin-static
    PRIVATE "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra>>")
  set_target_properties(
    sdl-bin-static PROPERTIES INSTALL_RPATH
                              "${CMAKE_CURRENT_LIST_DIR}/../bin/install/lib")

  # Install rules.
  install(TARGETS sdl-bin-static
          DESTINATION ${PROJECT_SOURCE_DIR}/../bin/install)

  # Enable testing.
  enable_testing()
  add_test(NAME check-exit-code-static COMMAND sdl-bin-static)
endif()

if(NOT BUILD_SDL_STATIC)
  add_executable(sdl-bin-shared main.cxx)
  target_link_libraries(sdl-bin-shared glog::glog SDL3::SDL3-shared)
  target_compile_features(sdl-bin-shared PRIVATE cxx_std_17)
  target_compile_options(
    sdl-bin-shared
    PRIVATE "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra>>")
  set_target_properties(
    sdl-bin-shared
    PROPERTIES
      INSTALL_RPATH
      "${CMAKE_CURRENT_LIST_DIR}/../bin/install/lib;${CMAKE_CURRENT_LIST_DIR}/../build/sdl"
  )

  install(TARGETS sdl-bin-shared
          DESTINATION ${PROJECT_SOURCE_DIR}/../bin/install)

  enable_testing()
  add_test(NAME check-exit-code-dynamic COMMAND sdl-bin-shared)
endif()
