cmake_minimum_required(VERSION 3.22)

project(basic-triangle)

add_executable(triangle-bin triangle-render.cxx triangle-render.hxx main.cxx)
target_link_libraries(triangle-bin compiler-flags-lib glog::glog canvas-lib
                      line-render-lib helper-lib)
target_include_directories(
  triangle-bin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../basic-canvas
                       ${CMAKE_CURRENT_SOURCE_DIR}/../basic-line)
