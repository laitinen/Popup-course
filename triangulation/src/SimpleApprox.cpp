#include "Common.hpp"
#include "Picture.hpp"
#include "Triangle.hpp"

#include <cctype>
#include <iostream>
#include <vector>

using namespace aicha;

int sqW = 10;
std::vector<Triangle> triangles;

void approximateWithTriangles(const Picture& src, Picture& dst, size_t x1,
                              size_t y1, size_t x2, size_t y2)
{
  size_t rgbDst1[3] = {0,0,0}, rgbDst2[3] = {0,0,0};
  size_t tr1Pixels = 0, tr2Pixels = 0;
  for(size_t x = x1; x < x2; ++x) {
    for(size_t y = y1; y < y2; ++y) {
      Color color = src.color(x,y);
      if(y - y1 >= x - x1) ++tr1Pixels;
      else ++tr2Pixels;
      for(size_t i = 0; i < 3; ++i) {
        if(y - y1 >= x - x1) rgbDst1[i] += color[i];
        else rgbDst2[i] += color[i];
      }
    }
  }
  for(size_t i = 0; i < 3; ++i) {
    if(tr1Pixels > 0) rgbDst1[i] /= tr1Pixels;
    if(tr2Pixels > 0) rgbDst2[i] /= tr2Pixels;
  }
  int v1[] = {x1, y1, x1, y2 - 1, x2 - 1, y2 - 1};
  int v2[] = {x1 + 1, y1, x2-1, y1, x2-1, y2 - 2};
  Triangle t1(v1, Color(rgbDst1[0], rgbDst1[1], rgbDst1[2], 255));
  Triangle t2(v2, Color(rgbDst2[0], rgbDst2[1], rgbDst2[2], 255));
  dst.paintTriangle(t1);
  dst.paintTriangle(t2);
  triangles.push_back(t1);
  triangles.push_back(t2);
}

int main(int argc, char** argv) {
  if(argc < 3) {
    std::cout << "Usage: " << argv[0] << " inputTga outputTga [pixPerSquare]"
              << std::endl;
    return 0;
  }
  if(argc >= 4) sqW = atoi(argv[3]);
  Picture in(argv[1]);
  Picture out(in.width(), in.height());
  for(size_t i = 0; i < in.width(); i += sqW)
    for(size_t j = 0; j < in.height(); j += sqW)
      approximateWithTriangles(in, out, i, j,
                               std::min(i + sqW, in.width()),
                               std::min(j + sqW, in.height()));
  std::cout << in.width() << " " << in.height() << std::endl
            << triangles.size() << std::endl;
  for(size_t i = 0; i < triangles.size(); ++i) {
    for(size_t j = 0; j < 3; ++j) {
      std::cout << triangles[i].vertex(j).x << " "
                << triangles[i].vertex(j).y << " ";
    }
    Color c = triangles[i].color();
    std::cout << (int)c[0] << " " << (int)c[1] << " "
              << (int)c[2] << " " << (int)c[3] << std::endl;
  }
  out.writeToTGA(argv[2]);
}
