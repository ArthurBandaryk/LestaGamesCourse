#include "my-basic-canvas.hxx"
#include "triangle-render.hxx"

//
#include <glog/logging.h>

int main(int, char** argv) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  constexpr size_t k_width{400}, k_height{400};

  arci::my_canvas canvas{k_width, k_height, "P6"};

  canvas.fill_all_with_color({0, 0, 0});

  arci::triangle_render render{canvas};

  const std::vector<arci::point> vertices1{
      {0, 0},
      {k_width - 1, k_height - 1},
      {0, k_height - 1},
  };

  render.render(vertices1, 3, {255, 255, 50});

  canvas.save_ppm_image("basic-triangle.ppm");

  render.clear_all_with_color({0, 0, 0});

  const std::vector<arci::point> vertices2{
      // 1.
      {0, 150},
      {100, 100},
      {100, 200},

      // 2.
      {100, 100},
      {150, 0},
      {200, 100},

      // 3.
      {100, 200},
      {100, 100},
      {200, 200},

      // 4.
      {100, 100},
      {200, 100},
      {200, 200},

      // 5.
      {200, 100},
      {300, 150},
      {200, 200},

      // 6.
      {100, 200},
      {200, 200},
      {150, 300},
  };

  render.render(vertices2, 18, {0, 255, 0});

  canvas.save_ppm_image("figure.ppm");

  return 0;
}
