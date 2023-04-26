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

  /* top.y = middle.y (the base of triangle is parallel to X-axis).
    ________         /\
    \      /        /  \
     \    /        /    \
      \  /        /      \
       \/        /________\
  */
  if (((top.y == middle.y) && (top.y != bottom.y))
      || ((middle.y == bottom.y) && (top.y != middle.y))) {
    return get_rasterized_half_triangle(triangle);
  }
  // We have a default triangle. So try to split it on 2 triangles,
  // which have bases parallel to X-axis.
  else {
    return get_rasterized_default_triangle(triangle);
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

std::vector<vertex> triangle_interpolated_render::
    get_rasterized_half_triangle(std::array<vertex, 3>& triangle) {
  std::vector<vertex> result{};

  // Sort all vertices by y.
  std::sort(
      triangle.begin(),
      triangle.end(),
      [](const vertex& vertex1, const vertex& vertex2) {
        return vertex1.y < vertex2.y;
      });

  const vertex& top = triangle.at(0);
  const vertex& middle = triangle.at(1);
  const vertex& bottom = triangle.at(2);

  const size_t num_lines =
      std::lround(std::abs(triangle.at(0).y - triangle.at(2).y));

  // TODO(arci): find a better way to grab correct left, right and top triangle
  // vertexes.
  vertex left{}, right{}, top_edge{};
  if (static_cast<int32_t>(std::round(top.y))
      == static_cast<int32_t>(std::round(middle.y))) {
    if (static_cast<int32_t>(std::round(top.x))
        < static_cast<int32_t>(std::round(middle.x))) {
      left = top;
      right = middle;
    } else {
      left = middle;
      right = top;
    }
    top_edge = bottom;
  }

  // TODO(arci): find a better way to grab correct left, right and top triangle
  // vertexes.
  if (static_cast<int32_t>(std::round(middle.y))
      == static_cast<int32_t>(std::round(bottom.y))) {
    if (static_cast<int32_t>(std::round(middle.x))
        < static_cast<int32_t>(std::round(bottom.x))) {
      left = middle;
      right = bottom;
    } else {
      left = bottom;
      right = middle;
    }
    top_edge = top;
  }

  CHECK(num_lines);

  for (size_t i = 0; i <= num_lines; ++i) {
    const float tau = static_cast<float>(i) / num_lines;
    const vertex left_interpolated = get_interpolated_vertex(
        left,
        top_edge,
        tau);
    const vertex right_interpolated = get_interpolated_vertex(
        right,
        top_edge,
        tau);
    const std::vector<vertex> rasterized_horizontal_line =
        get_rasterized_horizontal_line(left_interpolated, right_interpolated);
    result.insert(
        result.end(),
        rasterized_horizontal_line.begin(),
        rasterized_horizontal_line.end());
  }

  return result;
}

std::vector<vertex> triangle_interpolated_render::
    get_rasterized_default_triangle(std::array<vertex, 3>& triangle) {
  std::vector<vertex> result{};

  const vertex& top = triangle.at(0);
  const vertex& middle = triangle.at(1);
  const vertex& bottom = triangle.at(2);

  const point start_biggest_edge{
      static_cast<int32_t>(std::round(top.x)),
      static_cast<int32_t>(std::round(top.y))};

  const point end_biggest_edge{
      static_cast<int32_t>(std::round(bottom.x)),
      static_cast<int32_t>(std::round(bottom.y))};

  const float y_split = middle.y;

  const std::vector<point> biggest_edge =
      get_pixels(start_biggest_edge, end_biggest_edge);

  const auto iter_split = std::find_if(
      biggest_edge.begin(),
      biggest_edge.end(),
      [y_split](const point& p) {
        return p.y == static_cast<int32_t>(std::round(y_split));
      });

  CHECK(iter_split != biggest_edge.end());

  float biggest_edge_length = (start_biggest_edge - end_biggest_edge).length();
  float top_middle_length = (*iter_split - start_biggest_edge).length();
  float tau = top_middle_length / biggest_edge_length;

  vertex split_vertex = get_interpolated_vertex(top, bottom, tau);

  std::array<vertex, 3> triangle_low{
      middle,
      split_vertex,
      bottom,
  };

  std::array<vertex, 3> triangle_high{
      split_vertex,
      top,
      middle,
  };

  const std::vector<vertex> rasterized_part_1 =
      get_rasterized_half_triangle(triangle_low);

  const std::vector<vertex> rasterized_part_2 =
      get_rasterized_half_triangle(triangle_high);

  result.insert(
      result.end(),
      rasterized_part_1.begin(),
      rasterized_part_1.end());

  result.insert(
      result.end(),
      rasterized_part_2.begin(),
      rasterized_part_2.end());

  return result;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
