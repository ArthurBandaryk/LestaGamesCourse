cmake_minimum_required(VERSION 3.22)

project(sdl-simple-loop)

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

# Set clang, gcc compilers.
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/sdl
                 ${CMAKE_BINARY_DIR}/sdl)

add_executable(sdl-bin-simple-loop main.cxx)
target_link_libraries(sdl-bin-simple-loop glog::glog SDL3::SDL3-shared)
target_compile_features(sdl-bin-simple-loop PRIVATE cxx_std_17)
target_compile_options(
  sdl-bin-simple-loop
  PRIVATE "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra>>")
