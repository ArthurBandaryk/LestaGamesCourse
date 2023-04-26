#include "triangle-render.hxx"

//
#include <glog/logging.h>

//
#include <algorithm>
#include <array>
#include <memory>
#include <utility>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

triangle_render::triangle_render(my_canvas& canvas)
  : line_render(canvas) {}

void triangle_render::render(
    const std::vector<point>& vertices,
    const size_t num_vertices,
    const color& color) {
  CHECK(num_vertices % 3 == 0) << "Triangle consists of 3 vertices";

  std::vector<point> all_pixels{};

  auto get_pixels_for_triangle =
      [this](const std::array<std::pair<point, point>, 3>& triangle_edges) {
        std::vector<point> result{};

        std::for_each(
            triangle_edges.begin(),
            triangle_edges.end(),
            [&result, this](const std::pair<point, point>& vertex) {
              std::vector<point> all_pixels_for_edge =
                  get_pixels(vertex.first, vertex.second);
              result.insert(
                  result.end(),
                  all_pixels_for_edge.begin(),
                  all_pixels_for_edge.end());
            });

        return result;
      };

  for (size_t i = 0; i < num_vertices; i += 3) {
    std::pair<point, point> v0{vertices.at(i), vertices.at(i + 1)};
    std::pair<point, point> v1{vertices.at(i + 1), vertices.at(i + 2)};
    std::pair<point, point> v2{vertices.at(i + 2), vertices.at(i)};
    const std::array<std::pair<point, point>, 3> triangle{v0, v1, v2};
    const auto all_pixels_for_triangle = get_pixels_for_triangle(triangle);
    all_pixels.insert(
        all_pixels.end(),
        all_pixels_for_triangle.begin(),
        all_pixels_for_triangle.end());
  }

  std::for_each(
      all_pixels.begin(),
      all_pixels.end(),
      [this, &color](const point& point) {
        set_pixel(point, color);
      });
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
