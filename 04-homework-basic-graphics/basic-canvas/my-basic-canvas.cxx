#include "my-basic-canvas.hxx"

//
#include <glog/logging.h>

//
#include <string_view>
#include <algorithm>
#include <cctype> // For isspace().
#include <fstream>
#include <string>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

static_assert(
    sizeof(color) == 3,
    "Struct 'arci::color' should have 24 bits for size");

bool operator==(const color& color1, const color& color2) {
  return color1.r == color2.r
      && color1.g == color2.g
      && color1.b == color2.b;
}

std::ostream& operator<<(std::ostream& os, const color& color) {
  os << "r = " << static_cast<int>(color.r)
     << "\tg = " << static_cast<int>(color.g)
     << "\tb = " << static_cast<int>(color.b);
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<color>& pixels) {
  std::for_each(
      pixels.begin(),
      pixels.end(),
      [&os, i = 0](const color& c) mutable {
        os << "[" << i << "]: r = " << static_cast<int>(c.r)
           << "\tg = " << static_cast<int>(c.g)
           << "\tb = " << static_cast<int>(c.b)
           << "\n";
        ++i;
      });

  return os;
}

///////////////////////////////////////////////////////////////////////////////

my_canvas::my_canvas(const size_t width, const size_t height)
  : m_width{width},
    m_height{height} {
  m_pixels.reserve(width * height);
}

std::vector<color> my_canvas::get_pixels() const {
  return m_pixels;
}

void my_canvas::load_ppm3_image(const std::string_view name_image) {
  std::ifstream file{};

  file.open(name_image.data(), std::ios_base::binary);

  CHECK(file.is_open()) << "Error on opening '"
                        << name_image << "' for reading";

  std::string ppm_format{}, max_color_value{};
  size_t width{}, height{};

  file >> ppm_format;

  CHECK(!ppm_format.empty()) << "Invalid ppm format for '"
                             << name_image << "'";

  CHECK_STREQ("P3", ppm_format.c_str())
      << "Function 'load_ppm3_image' supports only 'P3' format";

  LOG(INFO) << "Format read from '" << name_image << "' is: " << ppm_format;

  char symbol = file.get();
  CHECK(file.good());

  std::string comments{};

  // Reading all comments.
  while (symbol == '#' || isspace(static_cast<unsigned char>(symbol))) {
    std::string comment{};
    std::getline(file, comment, '\n');
    comments += comment + "\n";
    symbol = file.get();
    CHECK(file.good());
  }

  if (!comments.empty()) {
    const std::streampos pos = file.tellg() - static_cast<std::streamoff>(1);
    file.seekg(pos);
    LOG(INFO) << comments;
  }

  file >> width;
  file >> height;
  file >> max_color_value;

  LOG(INFO) << "Width: " << width;
  LOG(INFO) << "Height: " << height;
  LOG(INFO) << "Max color value is: " << max_color_value;

  m_pixels.resize(width * height);

  CHECK_EQ(m_pixels.size(), width * height);

  std::for_each(
      m_pixels.begin(),
      m_pixels.end(),
      [&file, r = 0, g = 0, b = 0](color& c) mutable {
        file >> r >> g >> b;
        c.r = static_cast<unsigned char>(r);
        c.g = static_cast<unsigned char>(g);
        c.b = static_cast<unsigned char>(b);
      });

  file.close();
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
