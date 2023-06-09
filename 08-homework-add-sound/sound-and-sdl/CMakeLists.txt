cmake_minimum_required(VERSION 3.22)

project(test-sound)

list(APPEND MUSIC_WAVS "music.wav" "hit.wav")
file(COPY ${MUSIC_WAVS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(test-sound main.cxx)
target_link_libraries(test-sound compiler-flags-lib glog::glog
                      SDL3::SDL3-shared)
