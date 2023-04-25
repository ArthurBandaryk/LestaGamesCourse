#pragma once

#include "line-render.hxx"

//
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

class triangle_indexed_render : public line_render {
 public:
  triangle_indexed_render(my_canvas& canvas);

  triangle_indexed_render(const triangle_indexed_render&) = delete;
  triangle_indexed_render(triangle_indexed_render&&) = delete;
  triangle_indexed_render& operator=(const triangle_indexed_render&) = delete;
  triangle_indexed_render& operator=(triangle_indexed_render&&) = delete;

  void render(
      const std::vector<point>& vertices,
      const std::vector<uint32_t>& indices,
      const color& color);
};

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
