#pragma once

#include "render.hxx"

//
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

class line_render : public irender {
 public:
  enum class algorithm {
    dda,
    bresenham,
  };

  line_render() = delete;
  line_render(const line_render&) = delete;
  line_render(line_render&&) = default;
  line_render& operator=(const line_render&) = delete;
  line_render& operator=(line_render&&) = default;
  line_render(my_canvas&, const algorithm type = algorithm::bresenham);

  void set_algorithm(const algorithm type);
  void set_pixel(const pixel_point& pixel, const color& color) override;
  void clear_all_with_color(const color& color) override;
  void render_line(
      const pixel_point& from,
      const pixel_point& to,
      const color& color);

 private:
  void check_point(const pixel_point& point) const;

  // DDA algorithm for grabbing pixels to render line.
  // Remember: since this algo uses floats - it's slow.
  std::vector<pixel_point> get_pixels_for_line_dda(
      const pixel_point& from,
      const pixel_point& to);

  // Bresenham algorithm for grabbing pixels to render line.
  std::vector<pixel_point> get_pixels_for_line_bresenham(
      const pixel_point& from,
      const pixel_point& to);

  my_canvas& m_canvas;
  algorithm m_algorithm{};
};

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
