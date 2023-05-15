cmake_minimum_required(VERSION 3.22)

project(opengl-morphing)

list(
  APPEND
  SHADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/triangle-vertex.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/triangle-fragment.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/star-vertex.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/star-fragment.shader")
file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_library(engine-morphing opengl-debug.hxx opengl-shader-programm.hxx
                            opengl-shader-programm.cxx engine.hxx engine.cxx)
target_link_libraries(engine-morphing compiler-flags-lib glog::glog
                      SDL3::SDL3-shared glad-lib)

add_executable(morphing-bin input-data.hxx main.cxx)
target_link_libraries(morphing-bin compiler-flags-lib glog::glog
                      engine-morphing)
