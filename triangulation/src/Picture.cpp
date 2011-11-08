#include "Picture.hpp"
#include "Common.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>

namespace aicha {

Picture::Picture(int w, int h) : m_w(w), m_h(h) {
  m_pic = new byte[m_w*m_h*3];
  for(size_t i = 0; i < m_w*m_h*3; ++i) {
    m_pic[i] = 255;
  }
}

Picture::Picture(const Picture& other) : m_w(other.m_w), m_h(other.m_h) {
  m_pic = new byte[m_w*m_h*3];
  std::copy(other.m_pic, other.m_pic + m_w*m_h*3, m_pic);
}

Picture::~Picture() { delete [] m_pic; }

Picture& Picture::operator=(const Picture& other) {
  Picture tmp(other);
  std::swap(m_pic, tmp.m_pic);
  std::swap(m_w, tmp.m_w);
  std::swap(m_h, tmp.m_h);
  return *this;
}

void Picture::paintColor(int x, int y, const Color& color) {
  if( x < 0 || (size_t)x >= m_w || y < 0 || (size_t)y >= m_h ) return;
  byte *pixel = (m_pic + 3*m_w*y + 3*x);
  double alpha = color.normalizedAlpha();
  for(size_t i = 0; i < 3; ++i) {
    pixel[i] = (byte) ((1 - alpha)*pixel[i] + alpha*color[i]);
  }
}

void Picture::writeToTGA(const char* fname) {
  std::ofstream out(fname);
  if(!out) {
    std::cerr << "Couldn't open file " << fname << std::endl;
    return;
  }
  byte header[] = {0, 0,
                   2, //image format == uncompressed RGB
                   0, 0,
                   0, 0,
                   0,
                   0, 0, // x-coordinate of bottom left corner
                   0, 0, // y-coordinate of bottom left corner
                   m_w & 0x00FF, (m_w & 0xFF00) >> 8, // width
                   m_h & 0x00FF, (m_h & 0xFF00) >> 8, // height
                   24, //bits per pixel
                   0};
  for(size_t i = 0; i < sizeof(header); ++i) {
    out.put(header[i]);
  }
  for(size_t i = 0; i < m_w*m_h*3; i += 3) {
    out.put(m_pic[i + 2]);
    out.put(m_pic[i + 1]);
    out.put(m_pic[i]);
  }
}

/** Struct for easing the drawing of triangle. */
struct Edge {
 public:
  Edge() { }

  Edge(const ivec2& V1, const ivec2& V2) {
    if(V1.y > V2.y) {
      v1 = V2; v2 = V1;
    } else {
      v1 = V1; v2 = V2;
    }
  }

  bool operator<(const Edge& e) const {
    return height() < e.height();
  }

  size_t height() const { return v2.y-v1.y + 1; }
  
  ivec2 v1, v2;

};


int Picture::scanLine(Edge& edge, const Color& color,
                      std::vector<int>& lineEnds,  bool rightSide, int i)
{
  if(edge.v1.y == edge.v2.y) {
    if(rightSide && edge.v1.x < edge.v2.x) std::swap(edge.v1, edge.v2);
    else if(!rightSide && edge.v1.x > edge.v2.x) std::swap(edge.v1, edge.v2);
  }

  int dx = abs(edge.v1.x - edge.v2.x), dy = edge.height()-1;
  int sx = (edge.v1.x < edge.v2.x)?1:-1;
  int err = dx - dy;
  int x1 = edge.v1.x, y1 = edge.v1.y, x2 = edge.v2.x, y2 = edge.v2.y;

  for(;;) {
    if(rightSide) {
      for(int x = x1; x >= lineEnds[i]; --x) paintColor(x, y1, color);
      lineEnds[i] = x1 + 1;
    } else{
      for(int x = x1; x <= lineEnds[i]; ++x) paintColor(x, y1, color);
      lineEnds[i] = x1 - 1;
    }
    if(x1 == x2 && y1 == y2) break;
    int e2 = 2*err;

    if(e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      ++y1;
      ++i;
    }
  }

  return i;
}


void Picture::paintTriangle(const Triangle& triangle) {
  Edge edges[3];
  for(int i = 0; i < 3; ++i) {
    edges[i] = Edge(triangle.vertex(i), triangle.vertex((i+1)%3));
  }
  std::sort(edges, edges+3);
  size_t height = edges[2].height();
  std::vector<int> lineEndpoints;
  lineEndpoints.resize(height);

  // Now apply Bresenham's line rasterization algorithm for lines!
  // First the heightest line is rasterized.
  // Decide whether the last remaining point is on the left or the right side
  // of the longest edge by using the properties of cross-product.
  ivec2 remPoint = edges[0].v1;
  if(remPoint == edges[2].v1 || remPoint == edges[2].v2) remPoint = edges[0].v2;

  ivec2 longestVec = edges[2].v2 - edges[2].v1;
  ivec2 remVec = remPoint - edges[2].v1;
  bool onRightSide = (longestVec.x*remVec.y - longestVec.y*remVec.x)  < 0;

  int dx = abs(edges[2].v1.x - edges[2].v2.x), dy = height-1;
  int sx = (edges[2].v1.x < edges[2].v2.x)?1:-1;
  int i = 0;
  int err = dx - dy;
  int x1 = edges[2].v1.x, y1 = edges[2].v1.y, x2 = edges[2].v2.x, y2 = edges[2].v2.y;

  // The idea is to fill vector lineEndpoints to tell which is the farthest
  // point to color at the same y-coordinate, when handling the last two
  // edges. 
  lineEndpoints[i] = x1;

  for(;;) {
    if(x1 == x2 && y1 == y2) break;
    int e2 = 2*err;
    int nx = x1, ny = y1;

    if(e2 > -dy) {
      err -= dy;
      nx += sx;
    }
    if (e2 < dx) {
      err += dx;
      ++i;
      ++ny;
    }
    if(ny != y1) lineEndpoints[i] = nx;
    else if(nx > x1 && !onRightSide) lineEndpoints[i] = nx;
    else if(nx < x1 && onRightSide) lineEndpoints[i] = nx;
    x1 = nx, y1 = ny;
  }
  assert((size_t)i == height-1);
  if(edges[1].v1.y < edges[0].v1.y) std::swap(edges[1], edges[0]);
  i = 0;
  i = scanLine(edges[0], triangle.color(), lineEndpoints, onRightSide, i);
  scanLine(edges[1], triangle.color(), lineEndpoints, onRightSide, i);
}

} //namespace aicha


