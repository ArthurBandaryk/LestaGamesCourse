#pragma once

#include "line-render.hxx"

//
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

class triangle_render : public line_render {
 public:
  triangle_render(my_canvas& canvas);

  triangle_render(const triangle_render&) = delete;
  triangle_render(triangle_render&&) = delete;
  triangle_render& operator=(const triangle_render&) = delete;
  triangle_render& operator=(triangle_render&&) = delete;

  void render(
      const std::vector<point>& vertices,
      const size_t num_vertices,
      const color& color);
};

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
