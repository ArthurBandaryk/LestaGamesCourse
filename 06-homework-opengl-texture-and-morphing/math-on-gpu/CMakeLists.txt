cmake_minimum_required(VERSION 3.22)

project(math-on-cpu)

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

add_library(engine-math-on-gpu opengl-debug.hxx opengl-shader-programm.hxx
                               opengl-shader-programm.cxx engine.hxx engine.cxx)
target_link_libraries(engine-math-on-gpu compiler-flags-lib glm::glm glog::glog
                      SDL3::SDL3-shared glad-lib)

add_executable(math-on-gpu-bin main.cxx)
target_link_libraries(math-on-gpu-bin compiler-flags-lib glm::glm glog::glog
                      engine-math-on-gpu)
