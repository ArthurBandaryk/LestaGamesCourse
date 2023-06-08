cmake_minimum_required(VERSION 3.22)

project(test-sound)

add_executable(test-sound main.cxx)
target_link_libraries(test-sound compiler-flags-lib glog::glog
                      SDL3::SDL3-shared)
