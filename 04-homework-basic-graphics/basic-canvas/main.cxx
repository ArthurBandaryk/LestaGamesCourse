#include "my-basic-canvas.hxx"

//
#include <glog/logging.h>

int main(int, char** argv) {
  // Init google logging library.
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  LOG(INFO) << "Hello world";
  return EXIT_SUCCESS;
}
