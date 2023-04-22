cmake_minimum_required(VERSION 3.22)

project(basic-canvas)

add_library(canvas-lib my-basic-canvas.hxx my-basic-canvas.cxx)
target_link_libraries(canvas-lib compiler-flags-lib glog::glog)

add_executable(basic-canvas-bin main.cxx)
target_link_libraries(basic-canvas-bin PRIVATE compiler-flags-lib canvas-lib
                                               glog::glog)
