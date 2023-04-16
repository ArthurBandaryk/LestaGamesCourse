#pragma once

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

class iengine;

iengine* engine_create();

///////////////////////////////////////////////////////////////////////////////

void engine_destroy(iengine* engine);

///////////////////////////////////////////////////////////////////////////////

enum class event_type {
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
  left_mouse_button_pressed,
  left_mouse_button_released,
  exit,
};

///////////////////////////////////////////////////////////////////////////////

class iengine {
 public:
  virtual ~iengine() = default;
  virtual void init() = 0;
  virtual bool process_input(event_type& event) = 0;
  virtual void render() = 0;
  virtual void uninit() = 0;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
