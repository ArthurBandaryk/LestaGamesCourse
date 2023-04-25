cmake_minimum_required(VERSION 3.22)

project(basic-line)

add_library(render-lib render.hxx render.cxx)
target_link_libraries(render-lib compiler-flags-lib canvas-lib)
target_include_directories(render-lib
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../basic-canvas)

add_library(helper-lib helper.hxx helper.cxx)
target_link_libraries(helper-lib compiler-flags-lib render-lib)
target_include_directories(helper-lib
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../basic-canvas)

add_library(line-render-lib line-render.hxx line-render.cxx)
target_link_libraries(line-render-lib compiler-flags-lib glog::glog render-lib
                      helper-lib)
target_include_directories(line-render-lib
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../basic-canvas)

add_executable(basic-line-bin main.cxx)
target_link_libraries(basic-line-bin compiler-flags-lib glog::glog canvas-lib
                      line-render-lib)
target_include_directories(basic-line-bin
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../basic-canvas)
