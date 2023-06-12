cmake_minimum_required(VERSION 3.22)

project(imgui-via-engine-api)

list(APPEND SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/texture-vertex.shader"
     "${CMAKE_CURRENT_SOURCE_DIR}/texture-fragment.shader")
file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/worm.png")
file(COPY ${IMAGES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND IMGUI_FONTS
     "${CMAKE_CURRENT_SOURCE_DIR}/imgui/fonts/Roboto-Medium.ttf")
file(COPY ${IMGUI_FONTS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/worm.png")
file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND MUSIC_WAVS "music.wav" "hit.wav")
file(COPY ${MUSIC_WAVS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(
  APPEND
  IMGUI_HEADERS
  "imgui/imconfig.h"
  "imgui/imgui_internal.h"
  "imgui/imgui.h"
  "imgui/imstb_rectpack.h"
  "imgui/imstb_textedit.h"
  "imgui/imstb_truetype.h"
  "imgui/imgui_impl_opengl.hxx")

list(
  APPEND
  IMGUI_SOURCES
  "imgui/imgui_demo.cpp"
  "imgui/imgui_draw.cpp"
  "imgui/imgui_tables.cpp"
  "imgui/imgui_widgets.cpp"
  "imgui/imgui.cpp"
  "imgui/imgui_impl_opengl.cpp")

# Check if `glog` git submodule is initialized.
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

# Check if `SDL` git submodule is initialized.
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../submodules/glm)
  message(
    FATAL_ERROR
      "Submodule `glm` is not initialized. "
      "Make sure you've cloned this repo with '--recurse-submodules' option.")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../submodules/glog
                 ${CMAKE_BINARY_DIR}/glog)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../submodules/sdl
                 ${CMAKE_BINARY_DIR}/sdl)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../submodules/glm
                 ${CMAKE_BINARY_DIR}/glm)

# Set clang, gcc compilers.
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")

# Create library for compiler flags in order to propagate compiler options to
# other dependents.
add_library(compiler-flags-lib INTERFACE)
target_compile_features(compiler-flags-lib INTERFACE cxx_std_17)
target_compile_options(
  compiler-flags-lib
  INTERFACE "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra>>")

add_library(glad-lib glad/include/glad/glad.h glad/include/KHR/khrplatform.h
                     glad/src/glad.c)
target_include_directories(glad-lib
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

add_library(imgui-lib STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_link_libraries(imgui-lib PRIVATE glad-lib glog::glog SDL3::SDL3-shared)
target_include_directories(imgui-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui
                                            ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET imgui-lib PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(engine opengl-debug.hxx opengl-shader-programm.hxx
                   opengl-shader-programm.cxx engine.hxx engine.cxx picopng.hxx)
target_link_libraries(
  engine
  compiler-flags-lib
  glm::glm
  glog::glog
  SDL3::SDL3-shared
  glad-lib
  imgui-lib)

add_executable(imgui main.cxx)
target_link_libraries(imgui compiler-flags-lib glm::glm glog::glog engine)
