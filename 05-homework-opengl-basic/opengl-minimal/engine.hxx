#pragma once

//
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

class iengine;

iengine* engine_create();

///////////////////////////////////////////////////////////////////////////////

void engine_destroy(iengine* engine);

///////////////////////////////////////////////////////////////////////////////

enum class event_from_device {
  keyboard,
  mouse,
};

///////////////////////////////////////////////////////////////////////////////

struct mouse_event {
  float x;
  float y;
};

///////////////////////////////////////////////////////////////////////////////

enum class keyboard_event {
  left_button_pressed,
  left_button_released,
  right_button_pressed,
  right_button_released,
  up_button_pressed,
  up_button_released,
  down_button_pressed,
  down_button_released,
  space_button_pressed,
  space_button_released,
  escape_button_pressed,
  escape_button_released,
};

///////////////////////////////////////////////////////////////////////////////

struct event {
  event_from_device device;
  std::optional<mouse_event> mouse_info;
  std::optional<keyboard_event> keyboard_info;
  bool is_quitting{false};
};

///////////////////////////////////////////////////////////////////////////////

struct vertex {
  float x{};
  float y{};
};

///////////////////////////////////////////////////////////////////////////////

class iengine {
 public:
  virtual ~iengine() = default;
  virtual void init() = 0;
  virtual bool process_input(event& event) = 0;
  virtual void render(
      const std::vector<vertex>& vertices,
      const std::vector<uint32_t>& indices) = 0;
  virtual void uninit() = 0;
  virtual void swap_buffers() = 0;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
