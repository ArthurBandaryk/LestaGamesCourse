#include "my-basic-canvas.hxx"

//
#include <glog/logging.h>

int main(int, char** argv) {
  // Init google logging library.
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  arci::my_canvas canvas{};

  // canvas.load_ppm3_image("resources/simple-p3-image-for-loading.ppm");
  canvas.load_ppm3_image("/home/arc1/LestaGames/LestaGamesCourse/04-homework-basic-graphics/basic-canvas/resources/simple-p3-image-for-loading.ppm");
  std::vector<arci::color> test{
      arci::color{255, 0, 0},
      arci::color{0, 255, 0},
      arci::color{0, 0, 255},
      arci::color{255, 255, 0},
      arci::color{255, 255, 255},
      arci::color{0, 0, 0},
      arci::color{255, 0, 0},
      arci::color{0, 255, 0},
      arci::color{0, 0, 255},
      arci::color{255, 255, 0},
      arci::color{255, 0, 0},
      arci::color{0, 255, 0},
      arci::color{0, 0, 255},
      arci::color{255, 255, 0},
      arci::color{255, 255, 255},
      arci::color{255, 0, 0},
      arci::color{0, 255, 0},
      arci::color{0, 0, 255},
      arci::color{255, 255, 0},
      arci::color{255, 255, 255},
      arci::color{255, 0, 0},
      arci::color{0, 255, 0},
      arci::color{0, 0, 255},
      arci::color{255, 255, 0},
      arci::color{255, 255, 255},
  };

  CHECK_EQ(test, canvas.get_pixels());

  return EXIT_SUCCESS;
}
