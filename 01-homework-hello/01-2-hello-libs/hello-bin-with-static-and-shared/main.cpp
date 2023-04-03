#include "hello-config.hpp"
#ifdef USE_HELLO_LIB
#include "hello.hpp"
#else
#include <iostream>
#endif

int main(int, char**) {
#ifdef USE_HELLO_LIB
  constexpr int version_major{PROGRAMM_VERSION_MAJOR},
      version_minor{PROGRAMM_VERSION_MINOR};

  return arci::hello(version_major, version_minor);
#else
  std::cout << "Simple `hello world`" << std::endl;
  return std::cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
#endif
}
