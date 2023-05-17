cmake_minimum_required(VERSION 3.22)

project(math-on-cpu)

list(
  APPEND
  SHADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/triangle-vertex.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/triangle-fragment.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/texture-vertex.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/texture-fragment.shader")
list(APPEND IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/ball.png")
file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${IMAGES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_library(engine-math-on-cpu opengl-debug.hxx opengl-shader-programm.hxx
                               opengl-shader-programm.cxx engine.hxx engine.cxx)
target_link_libraries(engine-math-on-cpu compiler-flags-lib glog::glog
                      SDL3::SDL3-shared glad-lib)

add_executable(math-on-cpu-bin main.cxx)
target_link_libraries(math-on-cpu-bin compiler-flags-lib glog::glog
                      engine-math-on-cpu glm::glm)
