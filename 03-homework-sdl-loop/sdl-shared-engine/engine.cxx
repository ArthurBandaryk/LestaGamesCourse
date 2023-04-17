#include "engine.hxx"

//
#include <SDL3/SDL.h>

//
#include <glog/logging.h>

//
#include <string_view>
#include <algorithm>
#include <array>
#include <optional>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

struct key {
  SDL_KeyCode key_code;
  event_type key_pressed;
  event_type key_released;
  std::string_view key_name{};
};

///////////////////////////////////////////////////////////////////////////////

const std::array<key, 6> keys{
    key{
        SDLK_LEFT,
        event_type::left_button_pressed,
        event_type::left_button_released,
        "left",
    },
    key{
        SDLK_RIGHT,
        event_type::right_button_pressed,
        event_type::right_button_released,
        "right",
    },
    key{
        SDLK_UP,
        event_type::up_button_pressed,
        event_type::up_button_released,
        "up",
    },
    key{
        SDLK_DOWN,
        event_type::down_button_pressed,
        event_type::down_button_released,
        "down",
    },
    key{
        SDLK_SPACE,
        event_type::space_button_pressed,
        event_type::space_button_released,
        "space",
    },
    key{
        SDLK_ESCAPE,
        event_type::escape_button_pressed,
        event_type::escape_button_released,
        "escape",
    },
};

// TODO(arci): add mouse events.

///////////////////////////////////////////////////////////////////////////////

class engine_using_sdl final : public iengine {
 public:
  engine_using_sdl() = default;
  engine_using_sdl(const engine_using_sdl&) = delete;
  engine_using_sdl(engine_using_sdl&&) = delete;
  engine_using_sdl& operator=(const engine_using_sdl&) = delete;
  engine_using_sdl& operator=(engine_using_sdl&&) = delete;

  bool testSDLVersion() {
    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    SDL_Log("Compiled version SDL: %u.%u.%u\n", compiled.major, compiled.minor, compiled.patch);
    SDL_Log("Linked version SDL: %u.%u.%u\n", linked.major, linked.minor, linked.patch);

    return SDL_COMPILEDVERSION == SDL_VERSIONNUM(linked.major, linked.minor, linked.patch);
  }

  void init() override {
    // SDL initialization.
    CHECK(testSDLVersion()) << "warning: SDL2 compiled and linked version mismatch";

    CHECK_EQ(SDL_Init(SDL_INIT_VIDEO), 0)
        << "`SDL_Init()` failed with the following error: "
        << SDL_GetError();

    LOG(INFO) << "Current video driver: " << SDL_GetCurrentVideoDriver();

    // Window setup.
    m_window = SDL_CreateWindow(
        "Static sdl engine",
        1024,
        768,
        SDL_WINDOW_OPENGL);

    CHECK(m_window)
        << "`SDL_CreateWindow()` failed with the following error: "
        << SDL_GetError();

    CHECK_EQ(
        SDL_SetWindowPosition(
            m_window,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED),
        0)
        << "`SDL_SetWindowPosition()` failed with the following error: "
        << SDL_GetError();
  }

  bool process_input(event_type& event) override {
    SDL_Event sdl_event{};
    std::optional<key> key_for_event{};

    if (SDL_PollEvent(&sdl_event)) {
      switch (sdl_event.type) {
        case SDL_EVENT_QUIT:
          event = event_type::exit;
          return true;
          break;

        case SDL_EVENT_KEY_UP:
          key_for_event = get_keyboard_key_for_event(sdl_event);
          if (key_for_event) {
            LOG(INFO) << "Button '" << key_for_event->key_name
                      << "' has been released";
            event = key_for_event->key_released;
          }
          return true;
          break;

        case SDL_EVENT_KEY_DOWN:
          key_for_event = get_keyboard_key_for_event(sdl_event);
          if (key_for_event) {
            LOG(INFO) << "Button '" << key_for_event->key_name
                      << "' has been pressed";
            event = key_for_event->key_pressed;
          }
          return true;
          break;
      }
    }

    return false;
  }

  void render() override {
    // For now do nothing.
  }

  void uninit() override {
    CHECK(m_window) << "Invalid 'm_window' state on uninit engine call";
    SDL_DestroyWindow(m_window);
    SDL_Quit();
  }

 private:
  std::optional<key> get_keyboard_key_for_event(
      const SDL_Event& sdl_event) {
    const auto iter = std::find_if(
        keys.begin(),
        keys.end(),
        [&sdl_event](const key& k) {
          return sdl_event.key.keysym.sym == k.key_code;
        });

    if (iter != keys.end()) {
      return std::make_optional(*iter);
    }

    return {};
  }

  SDL_Window* m_window{nullptr};
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
