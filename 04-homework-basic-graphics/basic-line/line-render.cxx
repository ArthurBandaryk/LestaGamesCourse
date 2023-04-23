#include "line-render.hxx"

//
#include <glog/logging.h>

//
#include <algorithm>
#include <cmath>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

line_render::line_render(my_canvas& canvas, algorithm type)
  : m_canvas{canvas},
    m_algorithm{type} {}

void line_render::set_pixel(const pixel_point& pixel, const color& color) {
  m_canvas.set_color_for_pixel(pixel.x, pixel.y, color);
}

void line_render::render_line(
    const pixel_point& from,
    const pixel_point& to,
    const color& color) {
  std::vector<pixel_point> pixels{};

  switch (m_algorithm) {
    case algorithm::dda:
      pixels = get_pixels_for_line_dda(from, to);
      break;
    case algorithm::bresenham:
      pixels = get_pixels_for_line_bresenham(from, to);
      break;
    case algorithm::vu:
      pixels = get_pixels_for_line_vu(from, to);
      break;
  }

  if (!pixels.empty()) {
    std::for_each(
        pixels.begin(),
        pixels.end(),
        [this, &color](const pixel_point& point) {
          set_pixel(point, color);
        });
  }
}

std::vector<pixel_point> line_render::get_pixels_for_line_dda(
    const pixel_point& from,
    const pixel_point& to) {
  CHECK(from.x >= 0 && static_cast<size_t>(from.x) < m_canvas.get_width());
  CHECK(from.y >= 0 && static_cast<size_t>(from.y) < m_canvas.get_height());

  CHECK(to.x >= 0 && static_cast<size_t>(to.x) < m_canvas.get_width());
  CHECK(to.y >= 0 && static_cast<size_t>(to.y) < m_canvas.get_height());

  std::vector<pixel_point> result{};

  const int32_t delta_x = from.x - to.x;
  const int32_t delta_y = from.y - to.y;

  const size_t steps = std::max(std::abs(delta_x), std::abs(delta_y));
  const int32_t mult_x = (delta_x > 0) ? -1 : 1;
  const int32_t mult_y = (delta_y > 0) ? -1 : 1;
  const float x_increment = static_cast<float>(std::abs(delta_x)) / steps * mult_x;
  const float y_increment = static_cast<float>(std::abs(delta_y)) / steps * mult_y;

  float x_cur = from.x;
  float y_cur = from.y;

  for (size_t i = 0; i < steps; ++i) {
    result.push_back(
        {static_cast<int32_t>(std::lround(x_cur)),
         static_cast<int32_t>(std::lround(y_cur))});
    x_cur += x_increment;
    y_cur += y_increment;
  }

  return result;
}

std::vector<pixel_point> line_render::get_pixels_for_line_bresenham(
    const pixel_point& from,
    const pixel_point& to) {
  // TODO(arci): implement bresenham algorithm.
  return {};
}

std::vector<pixel_point> line_render::get_pixels_for_line_vu(
    const pixel_point& from,
    const pixel_point& to) {
  // TODO(arci): implement vu algorithm.
  return {};
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
