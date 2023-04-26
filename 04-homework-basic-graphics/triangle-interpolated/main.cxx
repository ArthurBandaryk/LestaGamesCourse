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

  // 2 triangles which in fact are 2 horizontal lines.
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

  ///////////////////////////////////////////////////////////////////////////////

  render.clear_all_with_color({0, 0, 0});

  // 2 triangles which in fact are 2 vertical lines.
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

  ///////////////////////////////////////////////////////////////////////////////

  render.clear_all_with_color({0, 0, 0});

  // A simple default triangle.
  std::vector<arci::vertex> vertices3{
      {56.f, 67.f, 255.f, 0.f, 0.f},
      {301.f, 201.f, 0.f, 255.f, 0.f},
      {121.f, 367.f, 0.f, 0.f, 255.f},
  };

  std::vector<uint32_t> indices3{0, 1, 2};

  render.render(vertices3, indices3);

  canvas.save_ppm_image("resources/triangle-interpolated.ppm");

  ///////////////////////////////////////////////////////////////////////////////

  render.clear_all_with_color({0, 0, 0});

  // Square consisting from 2 triangles.
  std::vector<arci::vertex> vertices4{
      {0.f, 0.f, 255.f, 0.f, 0.f},
      {399.f, 0.f, 0.f, 255.f, 0.f},
      {0.f, 399.f, 0.f, 0.f, 255.f},
      {399.f, 0.f, 0.f, 255.f, 0.f},
      {399.f, 399.f, 255.f, 0.f, 0.f},
      {0.f, 399.f, 0.f, 0.f, 255.f},
  };

  std::vector<uint32_t> indices4{0, 1, 2, 3, 4, 5};

  render.render(vertices4, indices4);

  canvas.save_ppm_image("resources/square-interpolated.ppm");

  ///////////////////////////////////////////////////////////////////////////////

  render.clear_all_with_color({0, 0, 0});

  std::vector<arci::vertex> vertices5{
      {150.f, 0.f, 0.f, 0.f, 255.f},
      {100.f, 100.f, 0.f, 255.f, 0.f},
      {200.f, 100.f, 0.f, 255.f, 0.f},
      {0.f, 150.f, 255.f, 0.f, 0.f},
      {100.f, 200.f, 255.f, 255.f, 50.f},
      {200.f, 200.f, 255.f, 255.f, 50.f},
      {300.f, 150.f, 255.f, 0.f, 0.f},
      {150.f, 300.f, 0.f, 0.f, 255.f},
  };

  std::vector<uint32_t> indices5{
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

  render.render(vertices5, indices5);

  canvas.save_ppm_image("resources/star-interpolated.ppm");

  return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
