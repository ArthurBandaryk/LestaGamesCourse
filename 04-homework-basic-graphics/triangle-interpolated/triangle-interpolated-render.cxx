#include "triangle-interpolated-render.hxx"

//
#include <glog/logging.h>

//
#include <algorithm>
#include <cmath>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

static float get_interpolated_value(
    const float value1,
    const float value2,
    const float tau) {
  return value1 + (value2 - value1) * tau;
}

static vertex get_interpolated_vertex(
    const vertex& v0,
    const vertex& v1,
    const float tau) {
  return vertex{
      get_interpolated_value(v0.x, v1.x, tau),
      get_interpolated_value(v0.y, v1.y, tau),
      get_interpolated_value(v0.r, v1.r, tau),
      get_interpolated_value(v0.g, v1.g, tau),
      get_interpolated_value(v0.b, v1.b, tau),
  };
}

///////////////////////////////////////////////////////////////////////////////

triangle_interpolated_render::triangle_interpolated_render(my_canvas& canvas)
  : line_render(canvas) {}

void triangle_interpolated_render::render(
    const std::vector<vertex>& vertices,
    const std::vector<uint32_t>& indices) {
  CHECK(indices.size() % 3 == 0) << "Triangle consists of 3 vertices";

  const size_t num_indices = indices.size();

  for (size_t i = 0; i < num_indices; i += 3) {
    std::array<vertex, 3> triangle{
        vertices.at(indices.at(i)),
        vertices.at(indices.at(i + 1)),
        vertices.at(indices.at(i + 2)),
    };

    const std::vector<vertex> rasterized_vertices =
        get_rasterized_triangle(triangle);

    for (const vertex& vertex : rasterized_vertices) {
      const point p{
          static_cast<int32_t>(std::lround(vertex.x)),
          static_cast<int32_t>(std::lround(vertex.y)),
      };
      const color c{
          static_cast<unsigned char>(std::lround(vertex.r)),
          static_cast<unsigned char>(std::lround(vertex.g)),
          static_cast<unsigned char>(std::lround(vertex.b)),
      };
      set_pixel(p, c);
    }
  }
}

std::vector<vertex> triangle_interpolated_render::get_rasterized_triangle(
    std::array<vertex, 3>& triangle) {
  std::vector<vertex> result{};
  // Sort vertices from top to bottom.
  std::sort(
      triangle.begin(),
      triangle.end(),
      [](const vertex& vertex1, const vertex& vertex2) {
        return vertex1.y < vertex2.y;
      });

  const point top{
      static_cast<int32_t>(std::lround(triangle.at(0).x)),
      static_cast<int32_t>(std::lround(triangle.at(0).y)),
  };
  const point middle{
      static_cast<int32_t>(std::lround(triangle.at(1).x)),
      static_cast<int32_t>(std::lround(triangle.at(1).y)),
  };
  const point bottom{
      static_cast<int32_t>(std::lround(triangle.at(2).x)),
      static_cast<int32_t>(std::lround(triangle.at(2).y)),
  };

  // Triangle is a horizontal line.
  if (top.y == bottom.y) {
    // Sort all vertices by x.
    std::sort(
        triangle.begin(),
        triangle.end(),
        [](const vertex& vertex1, const vertex& vertex2) {
          return vertex1.x < vertex2.x;
        });

    std::vector<vertex> part1 =
        get_rasterized_horizontal_line(triangle.at(0), triangle.at(1));
    std::vector<vertex> part2 =
        get_rasterized_horizontal_line(triangle.at(1), triangle.at(2));

    result.insert(result.end(), part1.begin(), part1.end());
    result.insert(result.end(), part2.begin(), part2.end());
    return result;
  }

  // Triangle is a vertical line.
  if ((top.x == bottom.x) && (top.x == middle.x)) {
    std::vector<vertex> part1 =
        get_rasterized_vertical_line(triangle.at(0), triangle.at(1));
    std::vector<vertex> part2 =
        get_rasterized_vertical_line(triangle.at(1), triangle.at(2));

    result.insert(result.end(), part1.begin(), part1.end());
    result.insert(result.end(), part2.begin(), part2.end());
    return result;
  }

  return result;
}

std::vector<vertex> triangle_interpolated_render::
    get_rasterized_horizontal_line(
        const vertex& left,
        const vertex& right) {
  std::vector<vertex> result{};

  const size_t num_vertices = std::lround(std::abs(left.x - right.x)) + 1;

  for (size_t i = 0; i <= num_vertices; ++i) {
    const float tau = static_cast<float>(i) / num_vertices;
    const vertex v = get_interpolated_vertex(left, right, tau);
    result.push_back(v);
  }

  return result;
}

std::vector<vertex> triangle_interpolated_render::
    get_rasterized_vertical_line(
        const vertex& top,
        const vertex& bottom) {
  std::vector<vertex> result{};

  const size_t num_vertices = std::lround(std::abs(top.y - bottom.y)) + 1;

  for (size_t i = 0; i <= num_vertices; ++i) {
    const float tau = static_cast<float>(i) / num_vertices;
    const vertex v = get_interpolated_vertex(top, bottom, tau);
    result.push_back(v);
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
