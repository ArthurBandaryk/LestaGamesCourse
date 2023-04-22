cmake_minimum_required(VERSION 3.22)

project(basic-line)

add_executable(basic-line-bin render.hxx line-render.hxx render.cxx
                              line-render.cxx main.cxx)
target_link_libraries(basic-line-bin compiler-flags-lib canvas-lib glog::glog)
target_include_directories(basic-line-bin
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../basic-canvas)
