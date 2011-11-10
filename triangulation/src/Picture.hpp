#ifndef PICTURE_HPP_
#define PICTURE_HPP_

#include "Common.hpp"
#include "Triangle.hpp"

#include <vector>

namespace aicha {

struct Edge;

class Picture {
 public:
  Picture(size_t w, size_t h);
  Picture(const char *tgaFname);
  Picture(const Picture& other);

  ~Picture();

  Picture& operator=(const Picture& other);

  void paintColor(int x, int y, const Color& color);

  void paintTriangle(const Triangle& triangle);

  Color getAverageColor(const ivec2& v1, const ivec2& v2, const ivec2& v3) const;

  void writeToTGA(const char* fname) const;

  size_t width() const;

  size_t height() const;
  
  Color color(int x, int y) const;

  double distance(const Picture& pic) const;
  
  bool insidePicture(int x, int y) const;

  void clearColor();

 private:
  size_t m_w, m_h;
  byte *m_pic;

  int scanLine(Edge& edge, const Color& color, std::vector<int>& lineEnds,
               bool rightSide, int startIndex);
  int scanLine(Edge& edge, std::vector<int>& lineEnds,  bool rightSide,
               int i, size_t* rgb, size_t& pix) const;
  bool gatherEndPoints(Edge* edges, std::vector<int>& endPoints) const;
};

} //namespace aicha

#endif
