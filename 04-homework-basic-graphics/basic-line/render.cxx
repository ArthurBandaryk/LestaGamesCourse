#include "render.hxx"

//
#include <cmath>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

bool point::operator==(const point& point) {
  return x == point.x && y == point.y;
}

std::ostream& operator<<(std::ostream& os, const point& point) {
  os << "(x, y): " << point.x << ", " << point.y;
  return os;
}

float point::length() const {
  return std::sqrt(x * x + y * y);
}

point operator-(const point& point1, const point& point2) {
  return point{point1.x - point2.x, point1.y - point2.y};
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
