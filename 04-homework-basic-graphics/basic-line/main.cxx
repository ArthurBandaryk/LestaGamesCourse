#include "render.hxx"

//
#include <glog/logging.h>

int main(int, char** argv) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  return EXIT_SUCCESS;
}
