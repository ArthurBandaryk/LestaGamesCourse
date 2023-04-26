#include "line-render.hxx"
#include "helper.hxx"

//
#include <glog/logging.h>

//
#include <algorithm>
#include <cmath>
#include <memory>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

line_render::line_render(my_canvas& canvas, const algorithm type)
  : m_canvas{canvas},
    m_algorithm{type} {}

void line_render::set_pixel(const point& pixel, const color& color) {
  m_canvas.set_color_for_pixel(pixel.x, pixel.y, color);
}

void line_render::clear_all_with_color(const color& color) {
  m_canvas.fill_all_with_color(color);
}

void line_render::set_algorithm(const algorithm type) {
  m_algorithm = type;
}

void line_render::render(
    const point& from,
    const point& to,
    const color& color) {
  std::vector<point> pixels = get_pixels(from, to);

  std::for_each(
      pixels.begin(),
      pixels.end(),
      [this, &color](const point& point) {
        set_pixel(point, color);
      });
}

std::vector<point> line_render::get_pixels(
    const point& from,
    const point& to) const {
  // Algorithm for drawing lines (bresenham, dda).
  std::unique_ptr<ialgorithm> algorithm{nullptr};

  switch (m_algorithm) {
    case algorithm::dda:
      algorithm = std::make_unique<dda>();
      break;
    case algorithm::bresenham:
      algorithm = std::make_unique<bresenham>();
      break;
    default:
      break;
  }

  CHECK_NOTNULL(algorithm);
  return algorithm->get_pixels(from, to);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
