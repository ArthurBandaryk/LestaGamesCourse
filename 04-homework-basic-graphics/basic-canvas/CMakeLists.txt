cmake_minimum_required(VERSION 3.22)

project(basic-canvas)

add_executable(basic-canvas-bin my-basic-canvas.hxx main.cxx)
target_link_libraries(basic-canvas-bin PRIVATE compiler-flags-lib glog::glog)
