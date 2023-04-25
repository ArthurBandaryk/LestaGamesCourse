#include "helper.hxx"

//
#include <cmath>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

std::vector<point> bresenham::get_pixels(const point& from, const point& to) {
  std::vector<point> result{};

  int32_t delta_x = to.x - from.x;
  int32_t delta_y = to.y - from.y;

  // dy = 0.
  if (!delta_y) {
    return get_horizontal_pixels(from, delta_x);
  }

  // dx = 0.
  if (!delta_x) {
    return get_vertical_pixels(from, delta_y);
  }

  if (std::abs(to.y - from.y) <= std::abs(to.x - from.x)) {
    return get_pixels_for_slope_less_or_equal_than_one(
        from,
        to,
        delta_x,
        delta_y);
  } else {
    return get_pixels_for_slope_greater_than_one(
        from,
        to,
        delta_x,
        delta_y);
  }

  return {};
}

std::vector<point> bresenham::get_horizontal_pixels(
    const point& from,
    const int32_t delta_x) {
  std::vector<point> result{};

  int32_t x_cur = from.x;
  int32_t y_cur = from.y;

  for (int i = 0; i <= std::abs(delta_x); ++i) {
    result.push_back({x_cur, y_cur});
    if (delta_x > 0) {
      ++x_cur;
    } else {
      --x_cur;
    }
  }

  return result;
}

std::vector<point> bresenham::get_vertical_pixels(
    const point& from,
    const int32_t delta_y) {
  std::vector<point> result{};

  int32_t x_cur = from.x;
  int32_t y_cur = from.y;

  for (int i = 0; i <= std::abs(delta_y); ++i) {
    result.push_back({x_cur, y_cur});
    if (delta_y > 0) {
      ++y_cur;
    } else {
      --y_cur;
    }
  }

  return result;
}

std::vector<point> bresenham::get_pixels_for_slope_less_or_equal_than_one(
    const point& from,
    const point& to,
    int32_t delta_x,
    int32_t delta_y) {
  std::vector<point> result{};

  int32_t x_cur = from.x;
  int32_t y_cur = from.y;

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
    result.push_back({x_cur, y_cur});

    if (decision_current > 0) {
      y_cur += y_increment;
      decision_current -= 2 * delta_x;
    }

    decision_current += 2 * delta_y;
    ++x_cur;
  }

  return result;
}

std::vector<point> bresenham::get_pixels_for_slope_greater_than_one(
    const point& from,
    const point& to,
    int32_t delta_x,
    int32_t delta_y) {
  std::vector<point> result{};

  int32_t x_cur = from.x;
  int32_t y_cur = from.y;

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
    result.push_back({x_cur, y_cur});

    if (decision_current > 0) {
      x_cur += x_increment;
      decision_current -= 2 * delta_y;
    }

    decision_current += 2 * delta_x;
    ++y_cur;
  }

  return result;
}

std::vector<point> dda::get_pixels(const point& from, const point& to) {
  std::vector<point> result{};

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

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
