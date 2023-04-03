cmake_minimum_required(VERSION 3.22)

project(hello-libs)

# Setting variables for compiler warnings (different compilers might have
# different names for warning options).
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")
set(msvc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

# Create library for compiler flags in order to propagate compiler options to
# other dependents.
add_library(hello-compiler-flags INTERFACE)
target_compile_features(hello-compiler-flags INTERFACE cxx_std_17)
target_compile_options(
  hello-compiler-flags
  INTERFACE "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra>>"
            "$<${msvc_like_cxx}:$<BUILD_INTERFACE:-W3;>>")

list(APPEND SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/hello.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/src/hello.cpp)

add_library(hello-static-lib STATIC ${SOURCE_FILES})
target_link_libraries(hello-static-lib hello-compiler-flags)
target_include_directories(hello-static-lib
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(hello-dynamic-lib SHARED ${SOURCE_FILES})
target_link_libraries(hello-dynamic-lib hello-compiler-flags)
target_include_directories(hello-dynamic-lib
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Install rules.
install(
  TARGETS hello-static-lib hello-dynamic-lib hello-compiler-flags
  RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../../bin/install
  LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../../bin/install
  ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../../bin/install)
