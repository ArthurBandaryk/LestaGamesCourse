cmake_minimum_required(VERSION 3.22)

project(triangles-indexed)

add_executable(triangles-indexed-bin triangle-indexed-render.hxx
                                     triangle-indexed-render.cxx main.cxx)
target_link_libraries(triangles-indexed-bin compiler-flags-lib glog::glog
                      canvas-lib line-render-lib helper-lib)
target_include_directories(
  triangles-indexed-bin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../basic-canvas
                                ${CMAKE_CURRENT_SOURCE_DIR}/../basic-line)
