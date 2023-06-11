cmake_minimum_required(VERSION 3.22)

project(engine)

include(AddModule)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/shaders")

include_directories(
    ${PROJECT_SOURCE_DIR}/external/imgui
    ${PROJECT_SOURCE_DIR}/external/stb_image
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/imgui_impl)

file(GLOB_RECURSE ENGINE_SOURCES src/*.cxx imgui_impl/*.cxx)

add_library(engine ${ENGINE_SOURCES})

target_link_libraries(
  engine
  glm::glm
  fmt::fmt
  SDL3::SDL3-shared)

  add_module(engine glad ${PROJECT_SOURCE_DIR}/glad)
  add_module(engine imgui ${PROJECT_SOURCE_DIR}/external/imgui)
