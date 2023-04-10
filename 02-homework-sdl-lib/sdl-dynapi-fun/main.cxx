#include <glog/logging.h>

//
#include <SDL.h>

//
#include <cstdlib> // For std::getenv().

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const SDL_version& version) {
  os << static_cast<int>(version.major) << "."
     << static_cast<int>(version.minor) << "."
     << static_cast<int>(version.patch);
  return os;
}

///////////////////////////////////////////////////////////////////////////////

int main(int, char** argv) {
  // Init google logging library.
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  // NOTE: to demonstrate the force of `SDL_DYNAMIC_API` before
  // running this programm we should do exporting for
  // `SDL_DYNAMIC_API` environment variable. In my case it would
  // be the following chain of commands in terminal:
  // export SDL_DYNAMIC_API=/home/arc1/TestingSDL2/SDL-release-2.26.5/build/libSDL2-2.0.so &&
  // ./path/to/executable
  // Useful link:
  // https://github.com/libsdl-org/SDL/blob/main/docs/README-dynapi.md
  const char* sdl_dynamic_api_env = std::getenv("SDL_DYNAMIC_API");

  CHECK_NOTNULL(sdl_dynamic_api_env);

  LOG(INFO) << sdl_dynamic_api_env;

  SDL_version compiled_sdl_version{}, linked_sdl_version{};

  SDL_VERSION(&compiled_sdl_version);

  // SDL_GetVersion has no `return` value for SDL2(void).
  // SDL3 supports returning value.
  SDL_GetVersion(&linked_sdl_version);

  LOG(INFO) << "SDL compiled version: " << compiled_sdl_version;

  LOG(INFO) << "SDL linked version: " << linked_sdl_version;

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
