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
  line_render(my_canvas&, const algorithm type = algorithm::bresenham);

  line_render(const line_render&) = delete;
  line_render(line_render&&) = delete;
  line_render& operator=(const line_render&) = delete;
  line_render& operator=(line_render&&) = delete;

  void set_algorithm(const algorithm type);
  void set_pixel(const point& pixel, const color& color) override;
  void clear_all_with_color(const color& color) override;
  void render(
      const point& from,
      const point& to,
      const color& color);

  std::vector<point> get_pixels(const point& from, const point& to) const;

 protected:
  my_canvas& m_canvas;
  algorithm m_algorithm{};
};

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
