cmake_minimum_required(VERSION 3.22)
project(imgui)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/backends
    )

file(GLOB_RECURSE IMGUI_SOURCES src/*.cpp)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_link_libraries(imgui PRIVATE SDL3::SDL3-shared)

set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)
