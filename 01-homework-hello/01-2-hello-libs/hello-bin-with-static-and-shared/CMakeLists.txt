cmake_minimum_required(VERSION 3.22)

project(hello-binary VERSION 1.0)

# Option variable for building library if needed (for cmake study purpose).
option(USE_HELLO_LIB "Using my hello lib implementation" ON)

# For optional usage of hello library and version values.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../hello-config/hello-config.hpp.in
               ${CMAKE_BINARY_DIR}/hello-config.hpp @ONLY)

# If option `USE_HELLO_LIB` = ON => build hello library.
if(USE_HELLO_LIB)
  add_subdirectory(hello-libs)
  list(APPEND EXTRA_COMPILER_FLAGS hello-compiler-flags)
  list(APPEND EXTRA_STATIC_LIB hello-static-lib)
  list(APPEND EXTRA_DYNAMIC_LIB hello-dynamic-lib)
endif()

# Steps for static linking.
add_executable(hello-bin-static main.cpp)

# For `hello-config.hpp` file.
target_include_directories(hello-bin-static PRIVATE ${CMAKE_BINARY_DIR})

# Link to static libraries. If option `USE_HELLO_LIB` = OFF => main executable
# will be not linked to static libraries.
target_link_libraries(hello-bin-static PRIVATE ${EXTRA_STATIC_LIB}
                                               ${EXTRA_COMPILER_FLAGS})

# Steps for dynamic linking.
add_executable(hello-bin-dynamic main.cpp)
target_include_directories(hello-bin-dynamic PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(hello-bin-dynamic PRIVATE ${EXTRA_DYNAMIC_LIB}
                                                ${EXTRA_COMPILER_FLAGS})

# For install mode we should definitely specify RPATH because CMake by default
# will strip this path and thus we will be able to see that CMake set runtime
# path to "" empty string. And thus we will not be able to run executable with
# dynamic linking from INSTALL directory.
# https://cmake.org/cmake/help/latest/prop_tgt/INSTALL_RPATH.html
set_target_properties(
  hello-bin-dynamic PROPERTIES INSTALL_RPATH
                               ${CMAKE_CURRENT_LIST_DIR}/../../../bin/install)
set_target_properties(hello-bin-dynamic PROPERTIES INSTALL_RPATH_USE_LINK_PATH
                                                   TRUE)

# Install rules.
install(FILES ${PROJECT_BINARY_DIR}/../../../hello-config.hpp
        DESTINATION ${PROJECT_SOURCE_DIR}/install)
install(
  TARGETS hello-bin-dynamic hello-bin-static
  RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/install
  LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/install
  ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/install)

# Enable testing.
enable_testing()
add_test(NAME check-exit-code-static
         COMMAND ${PROJECT_BINARY_DIR}/hello-bin-static)
add_test(NAME check-exit-code-dynamic
         COMMAND ${PROJECT_BINARY_DIR}/hello-bin-dynamic)
