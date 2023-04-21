#include "my-basic-canvas.hxx"

//
#include <glog/logging.h>

int main(int, char** argv) {
  // Init google logging library.
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  arci::my_canvas canvas{};

  canvas.load_ppm3_image("resources/simple-p3-image-for-loading-1.ppm");
  std::vector<arci::color> test_ppm3_1{
      {255, 0, 0},
      {0, 255, 0},
      {0, 0, 255},
      {255, 255, 0},
      {255, 255, 255},
      {0, 0, 0},
      {255, 0, 0},
      {0, 255, 0},
      {0, 0, 255},
      {255, 255, 0},
      {255, 0, 0},
      {0, 255, 0},
      {0, 0, 255},
      {255, 255, 0},
      {255, 255, 255},
      {255, 0, 0},
      {0, 255, 0},
      {0, 0, 255},
      {255, 255, 0},
      {255, 255, 255},
      {255, 0, 0},
      {0, 255, 0},
      {0, 0, 255},
      {255, 255, 0},
      {255, 255, 255},
  };

  CHECK_EQ(test_ppm3_1, canvas.get_pixels());

  std::vector<arci::color> test_ppm3_2{
      {255, 0, 0},
      {0, 255, 0},
      {0, 0, 255},
      {255, 255, 0},
      {255, 255, 255},
      {0, 0, 0},
  };

  canvas.load_ppm3_image("resources/simple-p3-image-for-loading-2.ppm");
  CHECK_EQ(test_ppm3_2, canvas.get_pixels());

  return EXIT_SUCCESS;
}
