#include "line-render.hxx"
#include "my-basic-canvas.hxx"

//
#include <glog/logging.h>

//
#include <algorithm>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

int main(int, char** argv) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  constexpr size_t width{400}, height{400};

  struct line {
    arci::point from;
    arci::point to;
    arci::color color;
  };

  std::vector<line> lines{
      // Horizontal lines.
      {{0, 10}, {399, 10}, {0, 255, 255}},
      {{399, 20}, {0, 20}, {0, 255, 255}},
      {{0, 390}, {399, 390}, {0, 255, 255}},
      {{399, 399}, {0, 399}, {0, 255, 255}},

      // Vertical lines.
      {{10, 0}, {10, 399}, {255, 102, 255}},
      {{20, 0}, {20, 399}, {255, 102, 255}},
      {{399, 0}, {399, 399}, {255, 102, 255}},
      {{390, 399}, {390, 0}, {255, 102, 255}},

      // 45 degrees slope.
      {{0, 0}, {399, 399}, {0, 255, 0}},
      {{390, 399}, {0, 9}, {0, 255, 0}},
      {{0, 399}, {399, 0}, {255, 255, 50}},
      {{399, 10}, {10, 399}, {255, 255, 50}},

      // Slope < 1 (dx > dy).
      {{150, 50}, {359, 89}, {255, 0, 0}},
      {{359, 101}, {140, 76}, {255, 0, 0}},
      {{150, 81}, {359, 61}, {255, 0, 0}},
      {{359, 71}, {140, 101}, {255, 0, 0}},

      // Slope > 1 (dy > dx).
      {{59, 250}, {85, 389}, {255, 128, 0}},
      {{105, 389}, {79, 250}, {255, 128, 0}},
      {{59, 389}, {79, 250}, {255, 128, 0}},
      {{89, 250}, {69, 389}, {255, 128, 0}},
  };

  arci::my_canvas canvas{width, height, "P6"};

  canvas.fill_all_with_color({0, 0, 0});

  arci::line_render render(canvas, arci::line_render::algorithm::dda);

  auto draw_lines = [&render, &lines]() {
    std::for_each(
        lines.begin(),
        lines.end(),
        [&render](const line& line) {
          render.render(line.from, line.to, line.color);
        });
  };

  draw_lines();

  canvas.save_ppm_image("dda-algorithm.ppm");

  render.clear_all_with_color({0, 0, 0});
  render.set_algorithm(arci::line_render::algorithm::bresenham);

  draw_lines();

  canvas.save_ppm_image("bresenham-algorithm.ppm");

  return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
