#include <SDL3/SDL.h>

//
#include <glog/logging.h>

///////////////////////////////////////////////////////////////////////////////

int main(int, char** argv) {
  // Init google logging libary.
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  // SDL initialization.
  CHECK_EQ(SDL_Init(SDL_INIT_VIDEO), 0)
      << "`SDL_Init()` failed with the following error: "
      << SDL_GetError();

  LOG(INFO) << "Current video driver: " << SDL_GetCurrentVideoDriver();

  // Window setup.
  auto* sdl_window = SDL_CreateWindow(
      "Simple sdl loop",
      1024,
      768,
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  CHECK(sdl_window)
      << "`SDL_CreateWindow()` failed with the following error: "
      << SDL_GetError();

  CHECK_EQ(
      SDL_SetWindowPosition(
          sdl_window,
          SDL_WINDOWPOS_CENTERED,
          SDL_WINDOWPOS_CENTERED),
      0)
      << "`SDL_SetWindowPosition()` failed with the following error: "
      << SDL_GetError();

  auto* sdl_surface = SDL_GetWindowSurface(sdl_window);
  CHECK(sdl_surface)
      << "`SDL_GetWindowSurface()` failed with the following error: "
      << SDL_GetError();

  auto* sdl_renderer = SDL_CreateSoftwareRenderer(sdl_surface);
  CHECK(sdl_renderer)
      << "`SDL_CreateSoftwareRenderer()` failed with the following error: "
      << SDL_GetError();

  // Clear the rendering surface with the black color.
  CHECK_EQ(SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF), 0)
      << "`SDL_SetRenderDrawColor()` failed with the following error: "
      << SDL_GetError();

  CHECK_EQ(SDL_RenderClear(sdl_renderer), 0)
      << "`SDL_RenderClear()` failed with the following error: "
      << SDL_GetError();

  bool is_running{true};

  while (is_running) {
    SDL_Event sdl_event{};

    while (SDL_PollEvent(&sdl_event)) {
      switch (sdl_event.type) {
        case SDL_EVENT_QUIT:
          is_running = false;
          break;
        case SDL_EVENT_KEY_UP:
          LOG(INFO) << "Key " << sdl_event.key.keysym.sym
                    << " has been released";
          break;
        case SDL_EVENT_WINDOW_RESIZED:
          LOG(INFO) << "Window has been resized!";
          SDL_DestroyRenderer(sdl_renderer);
          sdl_surface = SDL_GetWindowSurface(sdl_window);
          CHECK(sdl_surface)
              << "`SDL_GetWindowSurface()` failed when window"
              << " has been resized with the following error: "
              << SDL_GetError();

          sdl_renderer = SDL_CreateSoftwareRenderer(sdl_surface);
          CHECK(sdl_renderer)
              << "`SDL_CreateSoftwareRenderer()` failed when window"
              << " has been resized with the following error: "
              << SDL_GetError();

          // Clear the rendering surface with the specified color.
          CHECK_EQ(SDL_SetRenderDrawColor(sdl_renderer, 56, 23, 32, 255), 0)
              << "`SDL_SetRenderDrawColor()` failed when window"
              << " has been resized with the following error: "
              << SDL_GetError();

          CHECK_EQ(SDL_RenderClear(sdl_renderer), 0)
              << "`SDL_RenderClear()` failed when window"
              << " has been resized with the following error: "
              << SDL_GetError();
          break;
      }
    }

    // Copy the window surface to the screen. This is the function you use
    // to reflect any changes to the surface on the screen.
    CHECK_EQ(SDL_UpdateWindowSurface(sdl_window), 0)
        << "`SDL_UpdateWindowSurface()` failed with the following error: "
        << SDL_GetError();
  }

  SDL_DestroyRenderer(sdl_renderer);
  SDL_DestroySurface(sdl_surface);
  SDL_DestroyWindow(sdl_window);

  SDL_Quit();

  return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
