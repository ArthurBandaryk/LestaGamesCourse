#include "line-render.hxx"
#include "my-basic-canvas.hxx"

//
#include <glog/logging.h>

int main(int, char** argv) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  arci::my_canvas canvas{400, 400, "P6"};

  canvas.fill_all_with_color({0, 0, 0});

  arci::line_render render(canvas, arci::line_render::algorithm::dda);

  // Horizontal lines.
  render.render_line({0, 10}, {399, 10}, {0, 255, 255});
  render.render_line({399, 20}, {0, 20}, {0, 255, 255});
  render.render_line({0, 390}, {399, 390}, {0, 255, 255});
  render.render_line({399, 399}, {0, 399}, {0, 255, 255});

  // Vertical lines.
  render.render_line({10, 0}, {10, 399}, {255, 102, 255});
  render.render_line({20, 0}, {20, 399}, {255, 102, 255});
  render.render_line({399, 0}, {399, 399}, {255, 102, 255});
  render.render_line({390, 399}, {390, 0}, {255, 102, 255});

  // 45 degrees slope.
  render.render_line({0, 0}, {399, 399}, {0, 255, 0});
  render.render_line({390, 399}, {0, 9}, {0, 255, 0});
  render.render_line({0, 399}, {399, 0}, {255, 255, 50});
  render.render_line({399, 10}, {10, 399}, {255, 255, 50});

  // Slope < 1 (dx > dy).
  render.render_line({150, 50}, {359, 89}, {255, 0, 0});
  render.render_line({359, 101}, {140, 76}, {255, 0, 0});
  render.render_line({150, 81}, {359, 61}, {255, 0, 0});
  render.render_line({359, 71}, {140, 101}, {255, 0, 0});

  // Slope > 1 (dy > dx).
  render.render_line({59, 250}, {85, 389}, {255, 128, 0});
  render.render_line({105, 389}, {79, 250}, {255, 128, 0});
  render.render_line({59, 389}, {79, 250}, {255, 128, 0});
  render.render_line({89, 250}, {69, 389}, {255, 128, 0});

  canvas.save_ppm_image("resources/dda-algorithm.ppm");

  return EXIT_SUCCESS;
}
