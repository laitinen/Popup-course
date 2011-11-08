#ifndef PICTURE_HPP_
#define PICTURE_HPP_

#include "Common.hpp"
#include "Triangle.hpp"

#include <vector>

namespace aicha {

struct Edge;

class Picture {
 public:
  Picture(int w, int h);
  Picture(const char *tgaFname);

  Picture(const Picture& other);

  ~Picture();

  Picture& operator=(const Picture& other);

  void paintColor(int x, int y, const Color& color);

  void paintTriangle(const Triangle& triangle);

  void writeToTGA(const char* fname) const;

 private:
  size_t m_w, m_h;
  byte *m_pic;

  int scanLine(Edge& edge, const Color& color, std::vector<int>& lineEnds,
               bool rightSide, int startIndex);
};

} //namespace aicha

#endif
