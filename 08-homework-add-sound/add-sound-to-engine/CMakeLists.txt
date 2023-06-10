cmake_minimum_required(VERSION 3.22)

project(add-sound-to-engine)

list(
  APPEND
  SHADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/triangle-vertex.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/triangle-fragment.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/texture-vertex.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/texture-fragment.shader")
list(APPEND IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/worm.png")
file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
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
  "imgui/backends/imgui_impl_sdl3.h"
  "imgui/backends/imgui_impl_opengl3.h"
  "imgui/backends/imgui_impl_opengl3_loader.h")

list(
  APPEND
  IMGUI_SOURCES
  "imgui/imgui_demo.cpp"
  "imgui/imgui_draw.cpp"
  "imgui/imgui_tables.cpp"
  "imgui/imgui_widgets.cpp"
  "imgui/imgui.cpp"
  "imgui/backends/imgui_impl_sdl3.cpp"
  "imgui/backends/imgui_impl_opengl3.cpp")

add_library(imgui-lib STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_link_libraries(imgui-lib PRIVATE SDL3::SDL3-shared)
target_include_directories(imgui-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set_property(TARGET imgui-lib PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(glad-lib glad/include/glad/glad.h glad/include/KHR/khrplatform.h
                     glad/src/glad.c)
target_include_directories(glad-lib
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

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

add_executable(sound main.cxx)
target_link_libraries(sound compiler-flags-lib glm::glm glog::glog engine)
