cmake_minimum_required(VERSION 3.22)

project(triangle-interpolated)

add_library(triangle-interpolated-lib triangle-interpolated-render.hxx
                                      triangle-interpolated-render.cxx)
target_link_libraries(
  triangle-interpolated-lib PUBLIC compiler-flags-lib glog::glog canvas-lib
                                   line-render-lib helper-lib)
target_include_directories(
  triangle-interpolated-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../basic-canvas
                                   ${CMAKE_CURRENT_SOURCE_DIR}/../basic-line)

add_executable(triangle-interpolated-bin main.cxx)
target_link_libraries(triangle-interpolated-bin
                      PRIVATE triangle-interpolated-lib)
