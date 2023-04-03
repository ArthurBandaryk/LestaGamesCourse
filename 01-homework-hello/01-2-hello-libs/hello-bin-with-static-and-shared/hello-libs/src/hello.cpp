#include "hello.hpp"
#include <iostream>

namespace arci {

int hello(const int version_major, const int version_minor) {
  std::cout << "Hello world from lib. Programm version: "
            << version_major << "." << version_minor
            << std::endl;
  return std::cout.good() ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace arci
