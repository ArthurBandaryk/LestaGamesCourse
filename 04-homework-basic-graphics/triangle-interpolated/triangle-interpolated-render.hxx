#pragma once

#include "line-render.hxx"
#include "my-basic-canvas.hxx"

//
#include <array>
#include <utility>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

struct vertex {
  float x{};
  float y{};
  float r{};
  float g{};
  float b{};
};

///////////////////////////////////////////////////////////////////////////////

class triangle_interpolated_render : public line_render {
 public:
  triangle_interpolated_render(my_canvas& canvas);

  triangle_interpolated_render(const triangle_interpolated_render&) = delete;
  triangle_interpolated_render(triangle_interpolated_render&&) = delete;
  triangle_interpolated_render& operator=(
      const triangle_interpolated_render&) = delete;
  triangle_interpolated_render& operator=(
      triangle_interpolated_render&&) = delete;

  void render(
      const std::vector<vertex>& vertices,
      const std::vector<uint32_t>& indices);

 private:
  std::vector<vertex> get_rasterized_triangle(std::array<vertex, 3>& triangle);
  std::vector<vertex> get_rasterized_horizontal_line(
      const vertex& left,
      const vertex& right);
  std::vector<vertex> get_rasterized_vertical_line(
      const vertex& top,
      const vertex& bottom);
  std::vector<vertex> get_rasterized_half_triangle(
      std::array<vertex, 3>& triangle);
  std::vector<vertex> get_rasterized_default_triangle(
      std::array<vertex, 3>& triangle);
};

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
