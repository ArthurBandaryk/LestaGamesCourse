#pragma once

#include "triangle-interpolated-render.hxx"

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

struct uniform {
  float u0{};
  float u1{};
  float u2{};
  float u3{};
};

///////////////////////////////////////////////////////////////////////////////

class ishader_programm {
 public:
  virtual ~ishader_programm() = default;
  virtual vertex get_vertex_transformed(const vertex& v_in) const = 0;
  virtual color get_color_transformed(
      [[maybe_unused]] const vertex& v_in) const = 0;
  void set_uniform(const uniform& uniform) noexcept {
    m_uniform = uniform;
  }
  uniform get_uniform() const noexcept {
    return m_uniform;
  }

 protected:
  uniform m_uniform{};
};

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
