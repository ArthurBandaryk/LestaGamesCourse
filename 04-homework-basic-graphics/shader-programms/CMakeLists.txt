cmake_minimum_required(VERSION 3.22)

project(simple-shader-programms)

add_library(engine-lib engine.hxx engine.cxx)
target_link_libraries(engine-lib compiler-flags-lib glog::glog canvas-lib
                      triangle-interpolated-lib SDL3::SDL3-shared)
target_include_directories(
  engine-lib
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../basic-canvas
         ${CMAKE_CURRENT_SOURCE_DIR}/../basic-line
         ${CMAKE_CURRENT_SOURCE_DIR}/../triangle-interpolated)

add_executable(shaders-bin shader-programm.hxx main.cxx)
target_link_libraries(shaders-bin PRIVATE compiler-flags-lib glog::glog
                                          engine-lib)
