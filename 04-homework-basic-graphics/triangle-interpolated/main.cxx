#include "triangle-interpolated-render.hxx"

//
#include <glog/logging.h>

//
#include <vector>

///////////////////////////////////////////////////////////////////////////////

int main(int, char** argv) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  arci::my_canvas canvas{400, 400, "P6"};

  canvas.fill_all_with_color({0, 0, 0});

  std::vector<arci::vertex> vertices1{
      {0.f, 0.f, 255.f, 0.f, 0.f},
      {200.f, 0.f, 0.f, 255.f, 0.f},
      {399.f, 0.f, 0.f, 0.f, 255.f},
      {399.f, 20.f, 255.f, 0.f, 0.f},
      {200.f, 20.f, 0.f, 255.f, 0.f},
      {0.f, 20.f, 0.f, 0.f, 255.f},
  };

  std::vector<uint32_t> indices1{0, 1, 2, 3, 4, 5};

  arci::triangle_interpolated_render render{canvas};

  render.render(vertices1, indices1);

  canvas.save_ppm_image("resources/horizontal-interpolated-lines.ppm");

  render.clear_all_with_color({0, 0, 0});

  std::vector<arci::vertex> vertices2{
      {200.f, 0.f, 255.f, 0.f, 0.f},
      {200.f, 200.f, 0.f, 255.f, 0.f},
      {200.f, 399.f, 0.f, 0.f, 255.f},
      {220.f, 399.f, 255.f, 0.f, 0.f},
      {220.f, 200.f, 0.f, 255.f, 0.f},
      {220.f, 0.f, 0.f, 0.f, 255.f},
  };

  std::vector<uint32_t> indices2{0, 1, 2, 3, 4, 5};

  render.render(vertices2, indices2);

  canvas.save_ppm_image("resources/vertical-interpolated-lines.ppm");

  return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
