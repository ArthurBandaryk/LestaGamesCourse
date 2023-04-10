cmake_minimum_required(VERSION 3.22)

project(sdl-dynapi)

# Set clang, gcc compilers.
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")

add_executable(sdl-bin-dynapi main.cxx)
target_link_libraries(
  sdl-bin-dynapi glog::glog
  /home/arc1/SDL2-26-3/SDL-release-2.26.3/build/libSDL2.a
  /home/arc1/SDL2-26-3/SDL-release-2.26.3/build/libSDL2main.a)
target_compile_features(sdl-bin-dynapi PRIVATE cxx_std_17)
target_compile_options(
  sdl-bin-dynapi PRIVATE "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra>>")
target_include_directories(
  sdl-bin-dynapi PRIVATE /home/arc1/SDL2-26-3/SDL-release-2.26.3/include)
