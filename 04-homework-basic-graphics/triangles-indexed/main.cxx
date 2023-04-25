#include "my-basic-canvas.hxx"
#include "triangle-indexed-render.hxx"

//
#include <glog/logging.h>

//
#include <vector>

///////////////////////////////////////////////////////////////////////////////

int main(int, char** argv) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  std::vector<arci::point> vertices{
      {150, 0},
      {100, 100},
      {200, 100},
      {0, 150},
      {100, 200},
      {200, 200},
      {300, 150},
      {150, 300},
  };

  std::vector<uint32_t> indices{
      // Left triangle.
      3,
      1,
      4,
      // Upper triangle.
      0,
      2,
      1,
      // Middle low triangle.
      1,
      5,
      4,
      // Middle high triangle.
      1,
      2,
      5,
      // Right triangle.
      2,
      6,
      5,
      // Low triangle.
      4,
      5,
      7,
  };

  arci::my_canvas canvas{400, 400, "P6"};

  canvas.fill_all_with_color({0, 0, 0});

  arci::triangle_indexed_render render{canvas};

  render.render(vertices, indices, {255, 255, 50});

  canvas.save_ppm_image("resources/triangles-indexed.ppm");

  return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
