#include "Common.hpp"

namespace aicha {

bool operator==(const ivec2& v1, const ivec2& v2) {
  return v1.x == v2.x && v1.y == v2.y;
}

ivec2 operator-(const ivec2& v1, const ivec2& v2) {
  ivec2 v = {v1.x - v2.x, v1.y - v2.y};
  return v;
}

Color::Color(int r, int g, int b, int a) {
  m_rgba[0] = r; m_rgba[1] = g; m_rgba[2] = b; m_rgba[3] = a;
}

double Color::normalizedAlpha() const { return m_rgba[3]/255.0; }

byte& Color::operator[](int i) { return m_rgba[i]; }
const byte& Color::operator[](int i) const { return m_rgba[i]; }

} //namespace aicha
