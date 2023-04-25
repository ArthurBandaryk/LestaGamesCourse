#include "triangle-indexed-render.hxx"
#include "helper.hxx"

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

triangle_indexed_render::triangle_indexed_render(my_canvas& canvas)
  : line_render(canvas) {}

void triangle_indexed_render::render(
    const std::vector<point>& vertices,
    const std::vector<uint32_t>& indices,
    const color& color) {
  CHECK(indices.size() % 3 == 0) << "Triangle consists of 3 vertices";

  std::vector<point> all_pixels{};

  std::unique_ptr<ialgorithm> algorithm{nullptr};

  switch (m_algorithm) {
    case algorithm::bresenham:
      algorithm = std::make_unique<bresenham>();
      break;
    case algorithm::dda:
      algorithm = std::make_unique<dda>();
      break;
    default:
      break;
  }

  auto get_pixels_for_triangle =
      [&algorithm](const std::array<std::pair<point, point>, 3>& triangle_edges) {
        std::vector<point> result{};

        std::for_each(
            triangle_edges.begin(),
            triangle_edges.end(),
            [&result, &algorithm](const std::pair<point, point>& vertex) {
              std::vector<point> all_pixels_for_edge =
                  algorithm->get_pixels(vertex.first, vertex.second);
              result.insert(
                  result.end(),
                  all_pixels_for_edge.begin(),
                  all_pixels_for_edge.end());
            });

        return result;
      };

  const size_t num_indices = indices.size();

  for (size_t i = 0; i < num_indices; i += 3) {
    std::pair<point, point> v0{
        vertices.at(indices.at(i)),
        vertices.at(indices.at(i + 1)),
    };

    std::pair<point, point> v1{
        vertices.at(indices.at(i + 1)),
        vertices.at(indices.at(i + 2)),
    };

    std::pair<point, point> v2{
        vertices.at(indices.at(i + 2)),
        vertices.at(indices.at(i)),
    };

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
