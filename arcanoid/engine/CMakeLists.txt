cmake_minimum_required(VERSION 3.22)

project(engine)

include(AddModule)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/shaders")

# Official CMake doc doesn't recommend to use GLOB. Check this:
# https://cmake.org/cmake/help/latest/command/include_directories.html
list(APPEND ENGINE_SOURCES imgui_impl/imgui_impl_opengl.cxx src/engine.cxx
     src/opengl-shader-programm.cxx)

add_library(engine ${ENGINE_SOURCES})

# Prefer to use `target_include_directories()` instead of
# `include_directories()`. Check this:
# https://cmake.org/cmake/help/latest/command/include_directories.html
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# We do not want to propagate internal implemention stuff (imgui_impl,
# stb_image, etc.), so we include the following directories with `PRIVATE`
# keyword.
target_include_directories(
    engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl
                   ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image)
target_link_libraries(engine glm::glm fmt::fmt SDL3::SDL3-shared)

add_module(engine glad ${PROJECT_SOURCE_DIR}/glad)
add_module(engine imgui ${PROJECT_SOURCE_DIR}/external/imgui)
