cmake_minimum_required(VERSION 3.22)

project(triangle-interpolated)

add_executable(
  triangle-interpolated-bin triangle-interpolated-render.hxx
                            triangle-interpolated-render.cxx main.cxx)
target_link_libraries(triangle-interpolated-bin compiler-flags-lib glog::glog
                      canvas-lib line-render-lib helper-lib)
target_include_directories(
  triangle-interpolated-bin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../basic-canvas
                                    ${CMAKE_CURRENT_SOURCE_DIR}/../basic-line)
