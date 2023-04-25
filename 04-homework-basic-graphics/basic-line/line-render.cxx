#include "line-render.hxx"

//
#include <glog/logging.h>

//
#include <algorithm>
#include <cmath>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

line_render::line_render(my_canvas& canvas, const algorithm type)
  : m_canvas{canvas},
    m_algorithm{type} {}

void line_render::set_pixel(const pixel_point& pixel, const color& color) {
  m_canvas.set_color_for_pixel(pixel.x, pixel.y, color);
}

void line_render::clear_all_with_color(const color& color) {
  m_canvas.fill_all_with_color(color);
}

void line_render::set_algorithm(const algorithm type) {
  m_algorithm = type;
}

void line_render::check_point(const pixel_point& point) const {
  CHECK(point.x >= 0 && static_cast<size_t>(point.x) < m_canvas.get_width());
  CHECK(point.y >= 0 && static_cast<size_t>(point.y) < m_canvas.get_height());
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
    default:
      break;
  }

  std::for_each(
      pixels.begin(),
      pixels.end(),
      [this, &color](const pixel_point& point) {
        set_pixel(point, color);
      });
}

std::vector<pixel_point> line_render::get_pixels_for_line_dda(
    const pixel_point& from,
    const pixel_point& to) {
  check_point(from);
  check_point(to);

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
  check_point(from);
  check_point(to);

  int32_t delta_x = to.x - from.x;
  int32_t delta_y = to.y - from.y;

  int32_t x_cur = from.x;
  int32_t y_cur = from.y;

  // Horizontal line.
  auto get_horizontal_pixels = [&, this]() {
    std::vector<pixel_point> result{};

    for (int i = 0; i <= std::abs(delta_x); ++i) {
      check_point({x_cur, y_cur});
      result.push_back({x_cur, y_cur});
      if (delta_x > 0) {
        ++x_cur;
      } else {
        --x_cur;
      }
    }

    return result;
  };

  // Vertical line.
  auto get_vertical_pixels = [&, this]() {
    std::vector<pixel_point> result{};

    for (int i = 0; i <= std::abs(delta_y); ++i) {
      check_point({x_cur, y_cur});
      result.push_back({x_cur, y_cur});
      if (delta_y > 0) {
        ++y_cur;
      } else {
        --y_cur;
      }
    }

    return result;
  };

  if (!delta_y) {
    return get_horizontal_pixels();
  }

  if (!delta_x) {
    return get_vertical_pixels();
  }

  // dy < dx.
  auto get_pixels_for_slope_less_than_one = [&, this]() {
    std::vector<pixel_point> result{};
    int32_t y_increment = 1;
    int32_t x_end = to.x;

    if (from.x > to.x) {
      delta_x = -delta_x;
      delta_y = -delta_y;
      x_cur = to.x;
      y_cur = to.y;
      x_end = from.x;
    }

    if (delta_y < 0) {
      delta_y = -delta_y;
      y_increment = -1;
    }

    int32_t decision_current = 2 * delta_y - delta_x;

    while (x_cur <= x_end) {
      check_point({x_cur, y_cur});
      result.push_back({x_cur, y_cur});

      if (decision_current > 0) {
        y_cur += y_increment;
        decision_current -= 2 * delta_x;
      }

      decision_current += 2 * delta_y;
      ++x_cur;
    }

    return result;
  };

  // dx < dy.
  auto get_pixels_for_slope_greater_than_one = [&, this]() {
    std::vector<pixel_point> result{};
    int32_t x_increment = 1;
    int32_t y_end = to.y;

    if (from.y > to.y) {
      delta_x = -delta_x;
      delta_y = -delta_y;
      x_cur = to.x;
      y_cur = to.y;
      y_end = from.y;
    }

    if (delta_x < 0) {
      delta_x = -delta_x;
      x_increment = -1;
    }

    int32_t decision_current = 2 * delta_x - delta_y;

    while (y_cur <= y_end) {
      check_point({x_cur, y_cur});
      result.push_back({x_cur, y_cur});

      if (decision_current > 0) {
        x_cur += x_increment;
        decision_current -= 2 * delta_y;
      }

      decision_current += 2 * delta_x;
      ++y_cur;
    }

    return result;
  };

  if (std::abs(to.y - from.y) <= std::abs(to.x - from.x)) {
    return get_pixels_for_slope_less_than_one();
  } else {
    return get_pixels_for_slope_greater_than_one();
  }

  return {};
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
