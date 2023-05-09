cmake_minimum_required(VERSION 3.22)

project(opengl-triangle)

list(APPEND SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/vertex.shader"
     "${CMAKE_CURRENT_SOURCE_DIR}/fragment.shader")
file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_library(engine engine.hxx engine.cxx)
target_link_libraries(engine compiler-flags-lib glog::glog SDL3::SDL3-shared
                      glad-lib)

add_executable(opengl-triangle main.cxx)
target_link_libraries(opengl-triangle compiler-flags-lib glog::glog engine)
