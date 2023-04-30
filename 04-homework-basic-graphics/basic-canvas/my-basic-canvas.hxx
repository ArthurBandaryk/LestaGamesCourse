#pragma once

#include <string_view>
#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)
struct color {
  unsigned char r{};
  unsigned char g{};
  unsigned char b{};
  friend bool operator==(const color& color1, const color& color2);
  friend std::ostream& operator<<(std::ostream& os, const color& color);
};
#pragma pack(pop)

std::ostream& operator<<(std::ostream& os, const std::vector<color>& pixels);

///////////////////////////////////////////////////////////////////////////////

class my_canvas final {
 public:
  ~my_canvas() = default;
  my_canvas() = default;
  my_canvas(
      const size_t width,
      const size_t height,
      const std::string& format);
  my_canvas(const my_canvas& other) = default;
  my_canvas(my_canvas&& other) = default;
  my_canvas& operator=(const my_canvas& other) = default;
  my_canvas& operator=(my_canvas&& other) = default;

  std::vector<color>& get_pixels() noexcept;
  void fill_all_with_color(const color& color);
  void set_color_for_pixel(
      const size_t i,
      const size_t j,
      const color& color);

  void set_resolution(const std::pair<size_t, size_t>& resolution);

  size_t get_width() const noexcept;
  size_t get_height() const noexcept;
  std::pair<size_t, size_t> get_resolution() const noexcept;

  void load_ppm_image(const std::string_view name_image);
  void save_ppm_image(const std::string_view name_image) const;

 private:
  std::vector<color> m_pixels{};

  // P3 or P6.
  std::string m_ppm_format{"P6"};

  size_t m_width{};
  size_t m_height{};
};

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
