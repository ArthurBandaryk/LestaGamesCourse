#include <glog/logging.h>
#include <iostream>

// All glog logs will be written to stderr.
#define ARCI_LOGGING_TO_STDERR

int main(int, char** argv) {
// IMPORTANT NOTE: this global flag should be defined before
// `InitGoogleLogging`. If this flag is specified, logfiles
// are written into the directory specified instead of the
// default logging directory:
// (tmp/<program name>.<hostname>.<user name>.log.<severity level>...)
// Check this link:
// https://github.com/google/glog#setting-flags
#ifdef ARCI_LOGGING_TO_FILE
  FLAGS_log_dir = ".";
#endif

#ifdef ARCI_LOGGING_TO_STDERR
  FLAGS_logtostderr = 1;
#endif

  // Initialize Google's logging library.
  google::InitGoogleLogging(argv[0]);

  LOG(INFO) << "Hello log from `glog` library";

  std::cout << "Hello, SDL!" << std::endl;

  return std::cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
}
