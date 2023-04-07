#include <SDL3/SDL.h> // For SDL_GetError().
#include <SDL3/SDL_version.h>

//
#include <glog/logging.h>

//
#include <iostream>

///////////////////////////////////////////////////////////////////////////////

// All glog logs will be written to stderr.
#define ARCI_LOGGING_TO_STDERR

std::ostream& operator<<(std::ostream& os, const SDL_version& version) {
  os << static_cast<int>(version.major) << "."
     << static_cast<int>(version.minor) << "."
     << static_cast<int>(version.patch);
  return os;
}

bool operator==(
    const SDL_version& compiled_version,
    const SDL_version& linked_version) {
  return compiled_version.major == linked_version.major
      && compiled_version.minor == linked_version.minor
      && compiled_version.patch == linked_version.patch;
}

///////////////////////////////////////////////////////////////////////////////

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

  SDL_version compiled_sdl_version{}, linked_sdl_version{};

  SDL_VERSION(&compiled_sdl_version);

  CHECK_EQ(SDL_GetVersion(&linked_sdl_version), 0)
      << "SDL_GetVersion() failed with the following error: "
      << SDL_GetError();

  std::cout << "SDL compiled version: "
            << compiled_sdl_version
            << std::endl;

  std::cout << "SDL linked version: "
            << linked_sdl_version
            << std::endl;

  CHECK_EQ(compiled_sdl_version, linked_sdl_version)
      << "Oops, SDL compiled version and link version differ";

  return std::cout.fail()
      ? EXIT_FAILURE
      : EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
