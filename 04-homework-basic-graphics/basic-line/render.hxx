#pragma once

#include "my-basic-canvas.hxx"

//
#include <ostream>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

struct point {
  int32_t x{};
  int32_t y{};
  bool operator==(const point& point);
  friend std::ostream& operator<<(std::ostream& os, const point& point);
};

///////////////////////////////////////////////////////////////////////////////

class irender {
 public:
  virtual ~irender() = default;
  virtual void clear_all_with_color(const color& color) = 0;
  virtual void set_pixel(const point& pixel, const color& color) = 0;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
