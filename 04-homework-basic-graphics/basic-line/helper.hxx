#pragma once

#include "render.hxx"

//
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

class ialgorithm {
 public:
  virtual ~ialgorithm() = default;
  virtual std::vector<point> get_pixels(
      const point& from,
      const point& to) = 0;
};

class bresenham : public ialgorithm {
 public:
  bresenham() = default;

  bresenham(const bresenham&) = delete;
  bresenham(bresenham&&) = delete;
  bresenham& operator=(const bresenham&) = delete;
  bresenham& operator=(bresenham&&) = delete;

  std::vector<point> get_pixels(const point& from, const point& to) override;

 private:
  std::vector<point> get_horizontal_pixels(
      const point& from,
      const int32_t delta_x);

  std::vector<point> get_vertical_pixels(
      const point& from,
      const int32_t delta_y);

  std::vector<point> get_pixels_for_slope_less_or_equal_than_one(
      const point& from,
      const point& to,
      int32_t delta_x,
      int32_t delta_y);

  std::vector<point> get_pixels_for_slope_greater_than_one(
      const point& from,
      const point& to,
      int32_t delta_x,
      int32_t delta_y);
};

class dda : public ialgorithm {
 public:
  dda() = default;

  dda(const dda&) = delete;
  dda(dda&&) = delete;
  dda& operator=(const dda&) = delete;
  dda& operator=(dda&&) = delete;

  std::vector<point> get_pixels(const point& from, const point& to) override;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
