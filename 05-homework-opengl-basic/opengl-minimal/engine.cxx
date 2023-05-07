#include "engine.hxx"
#include "glad/glad.h"

//
#include <SDL3/SDL.h>

//
#include <glog/logging.h>

//
#include <string_view>
#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <string>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

static std::string debug_msg_to_stderr{};

inline static void opengl_check();

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDebugMessageCallback.xhtml
static void APIENTRY opengl_message_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam);

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
    CHECK_EQ(SDL_Init(SDL_INIT_EVERYTHING), 0)
        << "`SDL_Init()` failed with the following error: "
        << SDL_GetError();

    CHECK_EQ(
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG),
        0)
        << "`SDL_GL_SetAttribute()` for debug failed with the error: "
        << SDL_GetError();

    CHECK_EQ(
        SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE),
        0)
        << "`SDL_GL_SetAttribute()` for profile failed with the error: "
        << SDL_GetError();

    int opengl_major_version{3}, opengl_minor_version{2};

    CHECK_EQ(
        SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_MAJOR_VERSION,
            opengl_major_version),
        0)
        << "`SDL_GL_SetAttribute()` for opengl major"
           " version failed with the error: "
        << SDL_GetError();

    CHECK_EQ(
        SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_MINOR_VERSION,
            opengl_minor_version),
        0)
        << "`SDL_GL_SetAttribute()` for opengl minor"
           " version failed with the error: "
        << SDL_GetError();

    CHECK_EQ(
        SDL_GL_GetAttribute(
            SDL_GL_CONTEXT_MAJOR_VERSION,
            &opengl_major_version),
        0)
        << "`SDL_GL_GetAttribute()` for opengl major"
           " version failed with the error: "
        << SDL_GetError();

    CHECK_EQ(
        SDL_GL_GetAttribute(
            SDL_GL_CONTEXT_MINOR_VERSION,
            &opengl_minor_version),
        0)
        << "`SDL_GL_GetAttribute()` for opengl minor"
           " version failed with the error: "
        << SDL_GetError();

    CHECK(opengl_major_version == 3) << "Invalid opengl major version";
    CHECK(opengl_minor_version == 2) << "Invalid opengl minor version";

    m_screen_width = 800u;
    m_screen_height = 600u;

    // Window setup.
    m_window = std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>(
        SDL_CreateWindow(
            "Basic opengl",
            m_screen_width,
            m_screen_height,
            SDL_WINDOW_OPENGL),
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

    SDL_GLContext opengl_context = SDL_GL_CreateContext(m_window.get());
    CHECK_NOTNULL(opengl_context);

    auto load_opengl_func_pointer = [](const char* func_name) -> void* {
      SDL_FunctionPointer sdl_func_pointer = SDL_GL_GetProcAddress(func_name);
      return reinterpret_cast<void*>(sdl_func_pointer);
    };

    CHECK(gladLoadGLES2Loader(load_opengl_func_pointer));

    debug_msg_to_stderr.resize(GL_MAX_DEBUG_MESSAGE_LENGTH);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_message_callback, nullptr);
    glDebugMessageControl(
        GL_DONT_CARE,
        GL_DONT_CARE,
        GL_DONT_CARE,
        0,
        nullptr,
        GL_TRUE);
  }

  ///////////////////////////////////////////////////////////////////////////////

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
            event.keyboard_info = keyboard_key_for_event->button_released;
          }
          break;

        case SDL_EVENT_KEY_DOWN:
          event.device = event_from_device::keyboard;
          keyboard_key_for_event = get_keyboard_key_for_event(sdl_event);
          if (keyboard_key_for_event) {
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

  ///////////////////////////////////////////////////////////////////////////////

  void render(
      [[maybe_unused]] const std::vector<vertex>& vertices,
      [[maybe_unused]] const std::vector<uint32_t>& indices) override {
    // TODO(arci): use opengl mechanism to draw triangles.
    glClearColor(0.f, 1.f, 1.f, 1.f);
    opengl_check();
    glClear(GL_COLOR_BUFFER_BIT);
    opengl_check();
  }

  ///////////////////////////////////////////////////////////////////////////////

  void uninit() override {
    SDL_Quit();
  }

  void swap_buffers() override {
    SDL_GL_SwapWindow(m_window.get());
  }

  std::pair<size_t, size_t> get_screen_resolution() const noexcept {
    return {m_screen_width, m_screen_height};
  }

  ///////////////////////////////////////////////////////////////////////////////

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

  ///////////////////////////////////////////////////////////////////////////////

  std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>
      m_window{nullptr, nullptr};

  std::size_t m_screen_width{};
  std::size_t m_screen_height{};
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

inline static void opengl_check() {
  const GLenum error = glGetError();

  if (error == GL_NO_ERROR) {
    return;
  }

  switch (error) {
    case GL_INVALID_ENUM:
      LOG(ERROR) << "An unacceptable value is specified"
                 << " for an enumerated argument";
      break;
    case GL_INVALID_VALUE:
      LOG(ERROR) << "A numeric argument is out of range";
      break;
    case GL_INVALID_OPERATION:
      LOG(ERROR) << "The specified operation is not"
                 << " allowed in the current state";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      LOG(ERROR) << "The framebuffer object is not complete";
      break;
    case GL_OUT_OF_MEMORY:
      LOG(ERROR) << "There is not enough memory left to execute the command";
      break;
    case GL_STACK_UNDERFLOW:
      LOG(ERROR) << "An attempt has been made to perform"
                 << " an operation that would cause an internal"
                 << " stack to underflow";
      break;
    case GL_STACK_OVERFLOW:
      LOG(ERROR) << "An attempt has been made to perform"
                 << " an operation that would cause an internal"
                 << " stack to overflow";
      break;
    default:
      LOG(ERROR) << "Undefined opengl error type";
      break;
  }

  std::exit(1);
}

///////////////////////////////////////////////////////////////////////////////

static std::string source_msg_enum_to_string(const GLenum source_msg);
static std::string type_msg_enum_to_string(const GLenum type_msg);
static std::string severity_msg_enum_to_string(const GLenum severity_msg);

static void APIENTRY opengl_message_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    [[maybe_unused]] const void* userParam) {
  CHECK(length < GL_MAX_DEBUG_MESSAGE_LENGTH)
      << "Debug opengl message must be less"
      << " than `GL_MAX_DEBUG_MESSAGE_LENGTH`";

  std::copy(message, message + length, debug_msg_to_stderr.begin());

  LOG(ERROR) << "Message id: " << id;
  LOG(ERROR) << source_msg_enum_to_string(source);
  LOG(ERROR) << type_msg_enum_to_string(type);
  LOG(ERROR) << severity_msg_enum_to_string(severity);
  LOG(ERROR) << debug_msg_to_stderr;
}

static std::string source_msg_enum_to_string(const GLenum source_msg) {
  std::string result{"Message source: "};

  switch (source_msg) {
    case GL_DEBUG_SOURCE_API:
      result += "calls to the OpenGL API\n";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      result += "calls to a window-system API\n";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      result += "a compiler for a shading language\n";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      result += "a third party application associated with OpenGL\n";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      result += "a source application associated with OpenGL\n";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      result += "some other source\n";
      break;
    default:
      result += "unknown source\n";
      break;
  }

  return result;
}

static std::string type_msg_enum_to_string(const GLenum type_msg) {
  std::string result{"Message type: "};

  switch (type_msg) {
    case GL_DEBUG_TYPE_ERROR:
      result += "an error, typically from the API\n";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      result += "some behavior marked deprecated has been used\n";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      result += "something has invoked undefined behavior\n";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      result += "some functionality the user relies upon is not portable\n";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      result += "code has triggered possible performance issues\n";
      break;
    case GL_DEBUG_TYPE_MARKER:
      result += "command stream annotation\n";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      result += "group pushing\n";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      result += "group popping\n";
      break;
    case GL_DEBUG_TYPE_OTHER:
      result += "some other type\n";
      break;
    default:
      result += "unknown type\n";
      break;
  }

  return result;
}

static std::string severity_msg_enum_to_string(const GLenum severity_msg) {
  std::string result{"Message severity: "};

  switch (severity_msg) {
    case GL_DEBUG_SEVERITY_HIGH:
      result +=
          "HIGH. All OpenGL Errors, shader compilation/linking errors,"
          " or highly-dangerous undefined behavior\n";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      result +=
          "MEDIUM. Major performance warnings, "
          "shader compilation/linking warnings, "
          "or the use of deprecated functionality\n";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      result +=
          "LOW. Redundant state change performance warning, "
          "or unimportant undefined behavior\n";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      result +=
          "NOTIFICATION. Anything that isn't an error or performance issue\n";
      break;
    default:
      result += "UNKNOWN\n";
      break;
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
