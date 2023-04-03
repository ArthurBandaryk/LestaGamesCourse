cmake_minimum_required(VERSION 3.22)

project(simple-hello-binary)

# Set clang, gcc and msvc compilers.
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")
set(msvc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

add_executable(simple-hello main.cpp)
target_compile_features(simple-hello PRIVATE cxx_std_17)
target_compile_options(
  simple-hello PRIVATE "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra>>"
                       "$<${msvc_like_cxx}:$<BUILD_INTERFACE:-W3;>>")

# Set install rules.
install(TARGETS simple-hello
        DESTINATION ${PROJECT_SOURCE_DIR}/../../bin/install)

# Enable testing.
enable_testing()
add_test(NAME check_exit_code COMMAND simple-hello)
