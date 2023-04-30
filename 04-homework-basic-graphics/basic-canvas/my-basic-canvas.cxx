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
        os << "[" << i << "]: "
           << c
           << "\n";
        ++i;
      });

  return os;
}

///////////////////////////////////////////////////////////////////////////////

my_canvas::my_canvas(
    const size_t width,
    const size_t height,
    const std::string& format)
  : m_ppm_format{format},
    m_width{width},
    m_height{height} {
  m_pixels.resize(width * height);
}

std::vector<color>& my_canvas::get_pixels() noexcept {
  return m_pixels;
}

void my_canvas::fill_all_with_color(const color& some_color) {
  std::for_each(m_pixels.begin(), m_pixels.end(), [&some_color](color& c) {
    c = some_color;
  });
}

void my_canvas::set_color_for_pixel(
    const size_t i,
    const size_t j,
    const color& color) {
  const size_t index = m_width * j + i;
  CHECK(index < m_pixels.size())
      << "Incorrect index for setting color";

  m_pixels[index] = color;
}

void my_canvas::set_resolution(const std::pair<size_t, size_t>& resolution) {
  m_width = resolution.first;
  m_height = resolution.second;
  m_pixels.resize(m_width * m_height);
}

size_t my_canvas::get_width() const noexcept {
  return m_width;
}

size_t my_canvas::get_height() const noexcept {
  return m_height;
}

std::pair<size_t, size_t> my_canvas::get_resolution() const noexcept {
  return std::make_pair(get_width(), get_height());
}

void my_canvas::load_ppm_image(const std::string_view name_image) {
  std::ifstream file{};

  file.exceptions(std::ios_base::failbit);
  file.open(name_image.data());

  CHECK(file.is_open()) << "Error on opening '"
                        << name_image << "' for reading";

  uint16_t max_color_value{};

  file >> m_ppm_format;

  CHECK(!m_ppm_format.empty()) << "Invalid ppm format for '"
                               << name_image << "'";

  CHECK("P3" == m_ppm_format || "P6" == m_ppm_format)
      << "Function 'load_ppm_image' supports only 'P3' and 'P6' formats";

  LOG(INFO) << "Format read from '" << name_image << "' is: " << m_ppm_format;

  char first_symbol_on_new_line = file.get();

  // Extracts all whitespace symbols. As a result, we should get the
  // position for symbol which is a beginning for a comment('#') or
  // int value for triplet column number.
  auto extracting_whitespaces = [&file, &first_symbol_on_new_line]() {
    while (isspace(static_cast<unsigned char>(first_symbol_on_new_line))) {
      first_symbol_on_new_line = file.get();
    }
  };

  extracting_whitespaces();

  std::string comments{};

  // Reading all comments.
  while (first_symbol_on_new_line == '#') {
    std::string comment{};
    std::getline(file, comment, '\n');
    CHECK(file.good());
    comments += comment + "\n";
    first_symbol_on_new_line = file.get();
    CHECK(file.good());
    extracting_whitespaces();
  }

  // We've consumed important symbol for reading. So set current file
  // position to the previous symbol.
  const std::streampos pos = file.tellg() - static_cast<std::streamoff>(1);
  file.seekg(pos);

  if (!comments.empty()) {
    LOG(INFO) << comments;
  }

  char white_space{};

  file >> m_width
      >> m_height
      >> max_color_value
      >> std::noskipws
      >> white_space;

  CHECK(isspace(static_cast<unsigned char>(white_space)))
      << "After max color value should be a whitespace symbol";

  LOG(INFO) << "Width: " << m_width;
  LOG(INFO) << "Height: " << m_height;
  LOG(INFO) << "Max color value is: " << max_color_value;

  m_pixels.resize(m_width * m_height);

  CHECK_EQ(m_pixels.size(), m_width * m_height);

  if (m_ppm_format == "P3") {
    std::for_each(
        m_pixels.begin(),
        m_pixels.end(),
        [&file, r = 0, g = 0, b = 0](color& c) mutable {
          char white_space{};
          file >> r >> white_space;
          CHECK(isspace(static_cast<unsigned char>(white_space)));
          file >> g >> white_space;
          CHECK(isspace(static_cast<unsigned char>(white_space)));
          file >> b >> white_space;
          CHECK(isspace(static_cast<unsigned char>(white_space)));
          c.r = static_cast<unsigned char>(r);
          c.g = static_cast<unsigned char>(g);
          c.b = static_cast<unsigned char>(b);
        });
  } else {
    const std::streamsize bytes_to_read = sizeof(color) * m_pixels.size();
    file.read(reinterpret_cast<char*>(m_pixels.data()), bytes_to_read);
  }

  file.close();
}

void my_canvas::save_ppm_image(const std::string_view name_file) const {
  std::ofstream file{};

  file.exceptions(std::ios_base::failbit);
  file.open(name_file.data(), std::ios_base::trunc);

  CHECK(file.is_open())
      << "Error on opening '" << name_file << "' for writing";

  CHECK("P3" == m_ppm_format || "P6" == m_ppm_format)
      << "Function 'save_ppm_image' supports only 'P3' and 'P6' formats";

  uint16_t max_color_value{255};

  file << m_ppm_format << "\n";
  file << m_width << " " << m_height << "\n";
  file << max_color_value << "\n";

  if (m_ppm_format == "P3") {
    std::for_each(m_pixels.begin(), m_pixels.end(), [&file](const color& c) {
      file << static_cast<int>(c.r) << " "
           << static_cast<int>(c.g) << " "
           << static_cast<int>(c.b) << "\n";
    });
  } else {
    std::streamsize bytes_to_write = sizeof(color) * m_pixels.size();
    file.write(reinterpret_cast<const char*>(m_pixels.data()), bytes_to_write);
  }

  file.close();
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
