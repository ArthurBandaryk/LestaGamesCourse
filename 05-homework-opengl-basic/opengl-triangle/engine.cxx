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
#include <fstream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

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

triangle::triangle(const vertex& v0, const vertex& v1, const vertex& v2) {
  vertices[0] = v0;
  vertices[1] = v1;
  vertices[2] = v2;
}

///////////////////////////////////////////////////////////////////////////////

class engine_using_sdl final : public iengine {
 public:
  engine_using_sdl() = default;
  engine_using_sdl(const engine_using_sdl&) = delete;
  engine_using_sdl(engine_using_sdl&&) = delete;
  engine_using_sdl& operator=(const engine_using_sdl&) = delete;
  engine_using_sdl& operator=(engine_using_sdl&&) = delete;

  void init() override;
  bool process_input(event& event) override;
  void render(
      const triangle& triangle,
      const std::vector<uint32_t>& indices) override;
  void swap_buffers() override;
  void uninit() override;

  std::pair<size_t, size_t> get_screen_resolution() const noexcept;

 private:
  std::optional<keyboard_key> get_keyboard_key_for_event(
      const SDL_Event& sdl_event);

  GLuint load_shader(const GLenum shader_type, const std::string_view shader_path);

  std::string get_shader_code_from_file(const std::string_view path);

  std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>
      m_window{nullptr, nullptr};

  std::unique_ptr<void, int (*)(SDL_GLContext)>
      m_opengl_context{nullptr, nullptr};

  std::size_t m_screen_width{};
  std::size_t m_screen_height{};
  GLuint m_programm_id{};
  GLuint m_vbo{};
  GLuint m_vao{};
};

void engine_using_sdl::init() {
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

  m_opengl_context = std::unique_ptr<void, int (*)(SDL_GLContext)>(
      SDL_GL_CreateContext(m_window.get()),
      SDL_GL_DeleteContext);
  CHECK_NOTNULL(m_opengl_context);

  auto load_opengl_func_pointer = [](const char* func_name) -> void* {
    SDL_FunctionPointer sdl_func_pointer = SDL_GL_GetProcAddress(func_name);
    CHECK_NOTNULL(sdl_func_pointer);
    return reinterpret_cast<void*>(sdl_func_pointer);
  };

  CHECK(gladLoadGLES2Loader(load_opengl_func_pointer));

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

  GLuint vertex_shader_id = load_shader(GL_VERTEX_SHADER, "vertex.shader");
  CHECK(vertex_shader_id);

  GLuint fragment_shader_id = load_shader(GL_FRAGMENT_SHADER, "fragment.shader");
  CHECK(fragment_shader_id);

  m_programm_id = glCreateProgram();
  opengl_check();
  CHECK(m_programm_id);

  glAttachShader(m_programm_id, vertex_shader_id);
  opengl_check();

  glAttachShader(m_programm_id, fragment_shader_id);
  opengl_check();

  glBindAttribLocation(m_programm_id, 0, "a_position");
  opengl_check();

  glBindAttribLocation(m_programm_id, 1, "a_color");
  opengl_check();

  glLinkProgram(m_programm_id);
  opengl_check();

  GLint linked{};
  glGetProgramiv(m_programm_id, GL_LINK_STATUS, &linked);
  opengl_check();

  if (!linked) {
    GLint log_length{};
    glGetProgramiv(m_programm_id, GL_INFO_LOG_LENGTH, &log_length);
    opengl_check();

    if (log_length > 1) {
      std::string log{};
      log.resize(log_length);
      glGetProgramInfoLog(m_programm_id, log_length, nullptr, log.data());
      opengl_check();
      LOG(ERROR) << log;
    }

    glDeleteProgram(m_programm_id);
    opengl_check();
    throw std::runtime_error{"Error on linking shader programm"};
  }

  glValidateProgram(m_programm_id);
  opengl_check();

  GLint is_validated{};
  glGetProgramiv(m_programm_id, GL_VALIDATE_STATUS, &is_validated);
  opengl_check();

  if (!is_validated) {
    GLint log_length{};
    glGetProgramiv(m_programm_id, GL_INFO_LOG_LENGTH, &log_length);
    opengl_check();

    if (log_length > 1) {
      std::string log{};
      log.resize(log_length);
      glGetProgramInfoLog(m_programm_id, log_length, nullptr, log.data());
      opengl_check();
      LOG(ERROR) << log;
    }

    glDeleteProgram(m_programm_id);
    throw std::runtime_error{"Error on validating shader programm"};
  }

  glDeleteShader(vertex_shader_id);
  opengl_check();

  glDeleteShader(fragment_shader_id);
  opengl_check();

  glGenBuffers(1, &m_vbo);
  opengl_check();

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  opengl_check();

  glGenVertexArrays(1, &m_vao);
  opengl_check();
  glBindVertexArray(m_vao);
  opengl_check();

  glUseProgram(m_programm_id);
  opengl_check();

  glEnableVertexAttribArray(0);
  opengl_check();

  glEnableVertexAttribArray(1);
  opengl_check();
}


bool engine_using_sdl::process_input(event& event) {
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

void engine_using_sdl::render(
    const triangle& triangle,
    [[maybe_unused]] const std::vector<uint32_t>& indices) {
  glBufferData(
      GL_ARRAY_BUFFER,
      sizeof(triangle.vertices),
      triangle.vertices.data(),
      GL_STATIC_DRAW);
  opengl_check();
  glVertexAttribPointer(
      0,
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(vertex),
      reinterpret_cast<void*>(0));
  opengl_check();

  glVertexAttribPointer(
      1,
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(vertex),
      reinterpret_cast<void*>(3 * sizeof(float)));
  opengl_check();

  glDrawArrays(GL_TRIANGLES, 0, 3);
  opengl_check();
}

void engine_using_sdl::swap_buffers() {
  CHECK(!SDL_GL_SwapWindow(m_window.get()));

  glClearColor(0.f, 1.f, 1.f, 1.f);
  opengl_check();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  opengl_check();
}

void engine_using_sdl::uninit() {
  SDL_Quit();
}

std::pair<size_t, size_t>
engine_using_sdl::get_screen_resolution() const noexcept {
  return {m_screen_width, m_screen_height};
}

std::optional<keyboard_key> engine_using_sdl::get_keyboard_key_for_event(
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

GLuint engine_using_sdl::load_shader(
    const GLenum shader_type,
    const std::string_view shader_path) {
  GLuint shader_id = glCreateShader(shader_type);
  opengl_check();

  CHECK(shader_id) << "Error on creating " << shader_type << " shader";

  const std::string shader_code_string =
      get_shader_code_from_file(shader_path);
  const char* shader_code = shader_code_string.data();

  glShaderSource(shader_id, 1, &shader_code, nullptr);
  opengl_check();

  glCompileShader(shader_id);
  opengl_check();

  GLint shader_compiled{};
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compiled);
  opengl_check();

  if (!shader_compiled) {
    GLint log_length{};
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
    opengl_check();

    if (log_length > 1) {
      std::string log{};
      log.resize(log_length);
      glGetShaderInfoLog(shader_id, log_length, nullptr, log.data());
      opengl_check();
      LOG(ERROR) << log;
    }

    glDeleteShader(shader_id);
    opengl_check();
    return 0;
  }

  return shader_id;
}

std::string engine_using_sdl::get_shader_code_from_file(
    const std::string_view path) {
  std::string shader_code{};

  std::ifstream file{path.data()};

  CHECK(file.is_open()) << "Error on opening " << path;

  file.seekg(0, std::ios_base::end);
  CHECK(file.good())
      << "Error on setting read position to the end of "
      << path << " file";

  std::streamsize bytes_to_read = file.tellg();
  CHECK(bytes_to_read);
  shader_code.resize(bytes_to_read);
  CHECK(file.good()) << "tellg() function failed in " << path;

  file.seekg(0, std::ios_base::beg);
  CHECK(file.good())
      << "Error on setting read position to the begin of "
      << path << " file";

  file.read(shader_code.data(), bytes_to_read);
  CHECK(file.good()) << "Error on reading " << path << " file";

  file.close();
  CHECK(file.good()) << "Error on closing " << path << " file";

  return shader_code;
}

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

  std::string debug_msg_to_stderr{};
  debug_msg_to_stderr.resize(GL_MAX_DEBUG_MESSAGE_LENGTH);
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
