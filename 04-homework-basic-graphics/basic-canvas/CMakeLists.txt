cmake_minimum_required(VERSION 3.22)

project(basic-canvas)

list(APPEND IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/resources/arci-for-load.ppm"
     "${CMAKE_CURRENT_SOURCE_DIR}/resources/ppm3-image-for-loading-1.ppm"
     "${CMAKE_CURRENT_SOURCE_DIR}/resources/ppm3-image-for-loading-2.ppm")
file(COPY ${IMAGES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_library(canvas-lib my-basic-canvas.hxx my-basic-canvas.cxx)
target_link_libraries(canvas-lib compiler-flags-lib glog::glog)

add_executable(basic-canvas-bin main.cxx)
target_link_libraries(basic-canvas-bin PRIVATE compiler-flags-lib canvas-lib
                                               glog::glog)
