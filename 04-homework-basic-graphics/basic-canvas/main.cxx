#include "my-basic-canvas.hxx"

//
#include <glog/logging.h>

//
#include <string_view>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

int main(int, char** argv) {
  // Init google logging library.
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  arci::my_canvas canvas{400, 400, "P3"};

  constexpr std::string_view blue_ppm3_image{"resources/saved-ppm3-blue-image.ppm"};

  constexpr arci::color blue{0, 102, 102};

  canvas.fill_all_with_color(blue);

  const std::vector<arci::color> test = canvas.get_pixels();
  CHECK_EQ(test.size(), 400u * 400u);

  canvas.save_ppm_image(blue_ppm3_image);
  canvas.load_ppm_image(blue_ppm3_image);
  CHECK_EQ(400u, canvas.get_width());
  CHECK_EQ(400u, canvas.get_height());
  CHECK_EQ(test, canvas.get_pixels());

  ///////////////////////////////////////////////////////////////////////////////

  constexpr std::string_view ppm3_img_to_load_1{
      "resources/ppm3-image-for-loading-1.ppm"};
  constexpr std::string_view ppm3_img_to_save_1{
      "resources/saved-ppm3-image-1.ppm"};

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

  canvas.load_ppm_image(ppm3_img_to_load_1);
  CHECK_EQ(5u, canvas.get_width());
  CHECK_EQ(5u, canvas.get_height());
  CHECK_EQ(test_ppm3_1, canvas.get_pixels());
  canvas.save_ppm_image(ppm3_img_to_save_1);
  canvas.load_ppm_image(ppm3_img_to_save_1);
  CHECK_EQ(5u, canvas.get_width());
  CHECK_EQ(5u, canvas.get_height());
  CHECK_EQ(test_ppm3_1, canvas.get_pixels());

  ///////////////////////////////////////////////////////////////////////////////

  constexpr std::string_view ppm3_img_to_load_2{
      "resources/ppm3-image-for-loading-2.ppm"};
  constexpr std::string_view ppm3_img_to_save_2{
      "resources/saved-ppm3-image-2.ppm"};

  std::vector<arci::color> test_ppm3_2{
      {255, 0, 0},
      {0, 255, 0},
      {0, 0, 255},
      {255, 255, 0},
      {255, 255, 255},
      {0, 0, 0},
  };

  canvas.load_ppm_image(ppm3_img_to_load_2);
  CHECK_EQ(3u, canvas.get_width());
  CHECK_EQ(2u, canvas.get_height());
  CHECK_EQ(test_ppm3_2, canvas.get_pixels());
  canvas.save_ppm_image(ppm3_img_to_save_2);
  canvas.load_ppm_image(ppm3_img_to_save_2);
  CHECK_EQ(3u, canvas.get_width());
  CHECK_EQ(2u, canvas.get_height());
  CHECK_EQ(test_ppm3_2, canvas.get_pixels());

  ///////////////////////////////////////////////////////////////////////////////

  constexpr std::string_view ppm6_img_to_load{"resources/arci-for-load.ppm"};
  constexpr std::string_view ppm6_img_to_save{"resources/saved-ppm6-arci.ppm"};
  canvas.load_ppm_image(ppm6_img_to_load);
  CHECK_EQ(2560u, canvas.get_width());
  CHECK_EQ(2560u, canvas.get_height());
  const std::vector<arci::color> test_arci = canvas.get_pixels();
  CHECK_EQ(test_arci.size(), 2560u * 2560u);

  canvas.save_ppm_image(ppm6_img_to_save);
  canvas.load_ppm_image(ppm6_img_to_save);
  CHECK_EQ(2560u, canvas.get_width());
  CHECK_EQ(2560u, canvas.get_height());
  CHECK_EQ(test_arci.size(), 2560u * 2560u);
  CHECK_EQ(test_arci, canvas.get_pixels());

  return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
