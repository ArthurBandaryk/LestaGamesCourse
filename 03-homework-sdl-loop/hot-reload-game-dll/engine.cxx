#include "engine.hxx"

//
#include <SDL3/SDL.h>

//
#include <glog/logging.h>

//
#include <string_view>
#include <algorithm>
#include <array>
#include <filesystem>
#include <memory>
#include <optional>
#include <thread>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

struct keyboard_key {
  SDL_KeyCode key_code;
  keyboard_event button_pressed;
  keyboard_event button_released;
  std::string_view key_name{};
};

///////////////////////////////////////////////////////////////////////////////

const std::array<keyboard_key, 6> keys{
    keyboard_key{
        SDLK_LEFT,
        keyboard_event::left_button_pressed,
        keyboard_event::left_button_released,
        "left",
    },
    keyboard_key{
        SDLK_RIGHT,
        keyboard_event::right_button_pressed,
        keyboard_event::right_button_released,
        "right",
    },
    keyboard_key{
        SDLK_UP,
        keyboard_event::up_button_pressed,
        keyboard_event::up_button_released,
        "up",
    },
    keyboard_key{
        SDLK_DOWN,
        keyboard_event::down_button_pressed,
        keyboard_event::down_button_released,
        "down",
    },
    keyboard_key{
        SDLK_SPACE,
        keyboard_event::space_button_pressed,
        keyboard_event::space_button_released,
        "space",
    },
    keyboard_key{
        SDLK_ESCAPE,
        keyboard_event::escape_button_pressed,
        keyboard_event::escape_button_released,
        "escape",
    },
};

///////////////////////////////////////////////////////////////////////////////

class engine_using_sdl final : public iengine {
 public:
  engine_using_sdl() = default;
  engine_using_sdl(const engine_using_sdl&) = delete;
  engine_using_sdl(engine_using_sdl&&) = delete;
  engine_using_sdl& operator=(const engine_using_sdl&) = delete;
  engine_using_sdl& operator=(engine_using_sdl&&) = delete;

  void init() override {
    // SDL initialization.
    CHECK_EQ(SDL_Init(SDL_INIT_VIDEO), 0)
        << "`SDL_Init()` failed with the following error: "
        << SDL_GetError();

    LOG(INFO) << "Current video driver: " << SDL_GetCurrentVideoDriver();

    // Window setup.
    m_window = std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>(
        SDL_CreateWindow("Aim target shooting", 1024, 768, SDL_WINDOW_OPENGL),
        SDL_DestroyWindow);

    CHECK(m_window)
        << "`SDL_CreateWindow()` failed with the following error: "
        << SDL_GetError();

    CHECK_EQ(
        SDL_SetWindowPosition(
            m_window.get(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED),
        0)
        << "`SDL_SetWindowPosition()` failed with the following error: "
        << SDL_GetError();

    m_renderer = std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer*)>(
        SDL_CreateRenderer(
            m_window.get(),
            "opengl",
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        SDL_DestroyRenderer);

    CHECK_NOTNULL(m_renderer);
  }

  bool process_input(event& event) override {
    bool continue_event_processing{true};
    SDL_Event sdl_event{};
    std::optional<keyboard_key> keyboard_key_for_event{};

    if (SDL_PollEvent(&sdl_event)) {
      switch (sdl_event.type) {
        case SDL_EVENT_QUIT:
          event.is_quitting = true;
          break;

        case SDL_EVENT_KEY_UP:
          event.device = event_from_device::keyboard;
          keyboard_key_for_event = get_keyboard_key_for_event(sdl_event);
          if (keyboard_key_for_event) {
            LOG(INFO) << "Button '" << keyboard_key_for_event->key_name
                      << "' has been released";
            event.keyboard_info = keyboard_key_for_event->button_released;
          }
          break;

        case SDL_EVENT_KEY_DOWN:
          event.device = event_from_device::keyboard;
          keyboard_key_for_event = get_keyboard_key_for_event(sdl_event);
          if (keyboard_key_for_event) {
            LOG(INFO) << "Button '" << keyboard_key_for_event->key_name
                      << "' has been pressed";
            event.keyboard_info = keyboard_key_for_event->button_pressed;
          }
          break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
          event.device = event_from_device::mouse;
          event.mouse_info = mouse_event{sdl_event.button.x, sdl_event.button.y};
          break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
          // For now do nothing.
          break;
      }
    } else {
      continue_event_processing = false;
    }

    return continue_event_processing;
  }

  void render_rectangle(
      float left,
      float top,
      float width,
      float height,
      unsigned char r,
      unsigned char g,
      unsigned char b,
      unsigned char a) override {
    const SDL_FRect rect{left, top, width, height};

    // Set color for background.
    CHECK(!SDL_SetRenderDrawColor(m_renderer.get(), 0, 0, 0, 255))
        << "`SDL_SetRenderDrawColor` for background"
        << " failed with the following error: "
        << SDL_GetError();

    CHECK(!SDL_RenderClear(m_renderer.get()))
        << "`SDL_RenderClear` failed with the following error: "
        << SDL_GetError();

    // Set color for target.
    CHECK(!SDL_SetRenderDrawColor(m_renderer.get(), r, g, b, a))
        << "`SDL_SetRenderDrawColor` for target"
        << " failed with the following error: "
        << SDL_GetError();

    CHECK(!SDL_RenderFillRect(m_renderer.get(), &rect))
        << "`SDL_RenderFillRect` failed with the following error: "
        << SDL_GetError();

    CHECK(!SDL_RenderPresent(m_renderer.get()))
        << "`SDL_RenderPresent` failed with the following error: "
        << SDL_GetError();
  }

  void uninit() override {
    SDL_Quit();
  }

 private:
  std::optional<keyboard_key> get_keyboard_key_for_event(
      const SDL_Event& sdl_event) {
    const auto iter = std::find_if(
        keys.begin(),
        keys.end(),
        [&sdl_event](const keyboard_key& k) {
          return sdl_event.key.keysym.sym == k.key_code;
        });

    if (iter != keys.end()) {
      return std::make_optional(*iter);
    }

    return {};
  }

  std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer*)>
      m_renderer{nullptr, nullptr};

  std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>
      m_window{nullptr, nullptr};
};

///////////////////////////////////////////////////////////////////////////////

class engine_instance {
 public:
  engine_instance() = delete;
  engine_instance(const engine_instance&) = delete;
  engine_instance(engine_instance&&) = delete;

  static iengine* get_instance() {
    CHECK(!m_is_existing) << "Engine has been already created";
    m_is_existing = true;
    return m_engine;
  }

 private:
  static iengine* m_engine;
  static bool m_is_existing;
};

iengine* engine_instance::m_engine = new engine_using_sdl{};
bool engine_instance::m_is_existing = false;

///////////////////////////////////////////////////////////////////////////////

iengine* engine_create() {
  return engine_instance::get_instance();
}

///////////////////////////////////////////////////////////////////////////////

void engine_destroy(iengine* engine) {
  CHECK(engine) << "Invalid engine state on engine destroy";
  delete engine;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////

arci::igame* hot_reload(
    const std::string_view game_name_dll,
    void*& old_handle,
    arci::igame* old_game,
    arci::iengine* engine);

int main(int, char** argv) {
  // Initialize google logging library.
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  // Create engine.
  std::unique_ptr<arci::iengine, void (*)(arci::iengine*)> engine{
      arci::engine_create(),
      arci::engine_destroy};

  engine->init();

  void* game_handle{nullptr};

  std::string_view game_name_dll{"./libgame-shared.so"};

  arci::igame* game = hot_reload(
      game_name_dll,
      game_handle,
      nullptr,
      engine.get());

  auto time_during_loading_game =
      std::filesystem::last_write_time(game_name_dll.data());

  CHECK_NOTNULL(game_handle);

  bool loop_continue{true};

  while (loop_continue) {
    // Try hot reload.
    auto current_time = std::filesystem::last_write_time(game_name_dll);

    if (current_time != time_during_loading_game) {
      // We should wait a little bit in case system is still overwriting
      // game dll.
      std::filesystem::file_time_type time_still_write;
      while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        time_still_write = std::filesystem::last_write_time(game_name_dll);

        // System defenitelly finished writing dll, just break.
        if (time_still_write == current_time) {
          break;
        }

        current_time = time_still_write;
      }

      // Now we are able to reload game.
      game = hot_reload(game_name_dll, game_handle, game, engine.get());

      time_during_loading_game = time_still_write;
    }

    arci::event event{};

    while (engine->process_input(event)) {
      if (event.is_quitting) {
        loop_continue = false;
        break;
      } else {
        game->process_event(event);
      }
    }

    if (game->is_game_over()) {
      break;
    }

    game->update();

    if (game->is_game_over()) {
      break;
    }

    game->render();
  }

  engine->uninit();

  return EXIT_SUCCESS;
}

// Hot reload mechanism.
arci::igame* hot_reload(
    const std::string_view game_dll_name,
    void*& old_game_handle,
    arci::igame* old_game,
    arci::iengine* engine) {
  const std::string_view copy_game_dll_name = "./libgame-shared-copy.so";

  if (old_game_handle) {
    // Before reloading game dll we should destroy old game in order
    // to not have memory leak.
    auto sdl_func_ptr =
        SDL_LoadFunction(old_game_handle, "destroy_game");

    CHECK_NOTNULL(sdl_func_ptr);

    using func_ptr_destroy_game = void (*)(arci::igame*);

    auto destroy_game = reinterpret_cast<func_ptr_destroy_game>(sdl_func_ptr);

    destroy_game(old_game);

    SDL_UnloadObject(old_game_handle);
  }

  CHECK(std::filesystem::exists(game_dll_name.data()));

  if (std::filesystem::exists(copy_game_dll_name.data())) {
    CHECK(std::filesystem::remove(copy_game_dll_name.data()));
  }

  try {
    std::filesystem::copy(game_dll_name, copy_game_dll_name);
  } catch (std::filesystem::filesystem_error& exception) {
    LOG(ERROR) << "Failed when copying `libgame-shared.so` dll: "
               << exception.what();

    exit(EXIT_FAILURE);
  }

  void* game_handle = SDL_LoadObject(copy_game_dll_name.data());

  CHECK_NOTNULL(game_handle);

  old_game_handle = game_handle;

  using func_ptr_create_game = decltype(&arci::create_game);

  SDL_FunctionPointer sdl_func_ptr =
      SDL_LoadFunction(game_handle, "create_game");

  CHECK_NOTNULL(sdl_func_ptr);

  auto create_game_ptr = reinterpret_cast<func_ptr_create_game>(sdl_func_ptr);

  return create_game_ptr(engine);
}

///////////////////////////////////////////////////////////////////////////////
