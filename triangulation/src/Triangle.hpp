#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include "Common.hpp"

namespace aicha {

class Triangle {
 public:
  Triangle(int *vertices) : m_color(255, 255, 255, 0) {
    for(size_t i = 0; i < 3; ++i) {
      m_vertices[i].x = vertices[2*i];
      m_vertices[i].y = vertices[2*i + 1];
    }
  }

  Triangle(int *vertices, const Color& color) : m_color(color) {
    for(size_t i = 0; i < 3; ++i) {
      m_vertices[i].x = vertices[2*i];
      m_vertices[i].y = vertices[2*i + 1];
    }
  }

  Triangle(ivec2* vecs, const Color& color)
      : m_color(color)
  {
    for(int i = 0; i < 3; ++i) m_vertices[i] = vecs[i];
  }

  Color& color() { return m_color; }
  const Color& color() const { return m_color; }

  const ivec2& vertex(size_t i) const { return m_vertices[i]; }

 private:
  ivec2 m_vertices[3];
  Color m_color;
};

} //namespace aicha

#endif
