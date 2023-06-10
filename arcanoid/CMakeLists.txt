cmake_minimum_required(VERSION 3.22)

project(imgui)

list(
  APPEND
  SHADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/engine/shaders/triangle-vertex.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/engine/shaders/triangle-fragment.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/engine/shaders/texture-vertex.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/engine/shaders/texture-fragment.shader")
file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/resources/worm.png")
file(COPY ${IMAGES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND IMGUI_FONTS
     "${CMAKE_CURRENT_SOURCE_DIR}/engine/imgui/fonts/Roboto-Medium.ttf")
file(COPY ${IMGUI_FONTS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND MUSIC_WAVS "resources/sounds/music.wav" "resources/sounds/hit.wav")
file(COPY ${MUSIC_WAVS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(
  APPEND
  IMGUI_HEADERS
  "engine/imgui/imconfig.h"
  "engine/imgui/imgui_internal.h"
  "engine/imgui/imgui.h"
  "engine/imgui/imstb_rectpack.h"
  "engine/imgui/imstb_textedit.h"
  "engine/imgui/imstb_truetype.h"
  "engine/imgui/backends/imgui_impl_sdl3.h"
  "engine/imgui/backends/imgui_impl_opengl3.h"
  "engine/imgui/backends/imgui_impl_opengl3_loader.h")

list(
  APPEND
  IMGUI_SOURCES
  "engine/imgui/imgui_demo.cpp"
  "engine/imgui/imgui_draw.cpp"
  "engine/imgui/imgui_tables.cpp"
  "engine/imgui/imgui_widgets.cpp"
  "engine/imgui/imgui.cpp"
  "engine/imgui/backends/imgui_impl_sdl3.cpp"
  "engine/imgui/backends/imgui_impl_opengl3.cpp")

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

add_library(imgui-lib STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_link_libraries(imgui-lib PRIVATE SDL3::SDL3-shared)
target_include_directories(imgui-lib
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/engine/imgui)
set_property(TARGET imgui-lib PROPERTY POSITION_INDEPENDENT_CODE ON)

# Set clang, gcc compilers.
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")

# Create library for compiler flags in order to propagate compiler options to
# other dependents.
add_library(compiler-flags-lib INTERFACE)
target_compile_features(compiler-flags-lib INTERFACE cxx_std_17)
target_compile_options(
  compiler-flags-lib
  INTERFACE "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra>>")

add_library(
  glad-lib engine/glad/include/glad/glad.h
           engine/glad/include/KHR/khrplatform.h engine/glad/src/glad.c)
target_include_directories(
  glad-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/engine/glad/include)

add_library(
  engine
  engine/opengl-debug.hxx engine/opengl-shader-programm.hxx
  engine/opengl-shader-programm.cxx engine/engine.hxx engine/engine.cxx
  engine/picopng.hxx)
target_link_libraries(
  engine
  compiler-flags-lib
  glm::glm
  glog::glog
  SDL3::SDL3-shared
  glad-lib
  imgui-lib)

add_executable(arcanoid game/game.cxx)
target_link_libraries(arcanoid compiler-flags-lib glm::glm glog::glog engine)
target_include_directories(arcanoid PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine)
