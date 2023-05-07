cmake_minimum_required(VERSION 3.22)

project(opengl-clear-window)

add_library(engine-lib engine.hxx engine.cxx)
target_link_libraries(engine-lib compiler-flags-lib glog::glog
                      SDL3::SDL3-shared glad-lib)

add_executable(opengl-colored-window main.cxx)
target_link_libraries(opengl-colored-window compiler-flags-lib glog::glog
                      engine-lib)
