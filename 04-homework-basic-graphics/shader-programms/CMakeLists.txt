cmake_minimum_required(VERSION 3.22)

project(simple-shader-programms)

add_library(engine-lib engine.hxx engine.cxx)
target_link_libraries(engine-lib PRIVATE compiler-flags-lib glog::glog
                                         SDL3::SDL3-shared)

add_executable(shaders-bin main.cxx)
target_link_libraries(shaders-bin PRIVATE compiler-flags-lib glog::glog
                                          engine-lib)
target_include_directories(
  shaders-bin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../triangle-interpolated)
