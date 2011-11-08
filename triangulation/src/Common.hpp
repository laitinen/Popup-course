#ifndef DEFINITIONS_HPP_
#define DEFINITIONS_HPP_

#include <cstdlib> //For size_t

namespace aicha {

typedef unsigned char byte;

struct ivec2 {
  int x, y;
};

bool operator==(const ivec2& v1, const ivec2& v2);

ivec2 operator-(const ivec2& v1, const ivec2& v2);

class Color {
 public:
  template <typename T>
  Color(T *rgba) {
    for(size_t i = 0; i < 4; ++i) m_rgba[i] = static_cast<byte>(rgba[i]);
  }

  Color(int r, int g, int b, int a);
  
  double normalizedAlpha() const;

  byte& operator[](int i);
  const byte& operator[](int i) const;

 private:
  byte m_rgba[4];
};

} //namespace aicha

#endif
