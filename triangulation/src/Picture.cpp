#include "Picture.hpp"
#include "Common.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>

namespace aicha {

Picture::Picture(size_t w, size_t h) : m_w(w), m_h(h) {
  m_pic = new byte[m_w*m_h*3];
  for(size_t i = 0; i < m_w*m_h*3; ++i) {
    m_pic[i] = 255;
  }
}

Picture::Picture(const Picture& other) : m_w(other.m_w), m_h(other.m_h) {
  m_pic = new byte[m_w*m_h*3];
  std::copy(other.m_pic, other.m_pic + m_w*m_h*3, m_pic);
}

Picture::Picture(const char *tgaFname) {
  std::ifstream in(tgaFname);
  if(!in) {
    std::cerr << "Couldn't open file " << tgaFname << std::endl;
    return;
  }
  byte header[18];
  for(size_t i = 0; i < sizeof(header); ++i) {
    header[i] = in.get();
  }
  if(header[2] != 2 || header[16] != 24 || header[8] != 0 ||
     header[9] != 0 || header[10] != 0|| header[11] != 0)
  {
    std::cerr << "Unsupported TGA-format. Use uncompressed RGB's." << std::endl;
    return;
  }
  m_w = header[12] | (header[13] << 8);
  m_h = header[14] | (header[15] << 8);
  m_pic = new byte[m_w*m_h*3];
  for(size_t i = 0; i < m_w*m_h*3; i += 3) {
    m_pic[i+2] = in.get();
    m_pic[i+1] = in.get();
    m_pic[i] = in.get();
  }
}

Picture::~Picture() { delete [] m_pic; }

Picture& Picture::operator=(const Picture& other) {
  Picture tmp(other);
  std::swap(m_pic, tmp.m_pic);
  std::swap(m_w, tmp.m_w);
  std::swap(m_h, tmp.m_h);
  return *this;
}

void Picture::clearColor() {
  for(size_t i = 0; i < m_w*m_h*3; ++i) {
    m_pic[i] = 255;
  }
}

size_t Picture::width() const { return m_w; }

size_t Picture::height() const { return m_h; }

bool Picture::insidePicture(int x, int y) const {
  return ( x >= 0 && (size_t)x < m_w && y >= 0 && (size_t)y < m_h );
}


void Picture::paintColor(int x, int y, const Color& color) {
  if(!insidePicture(x, y)) return;
  byte *pixel = m_pic + 3*m_w*y + 3*x;
  double alpha = color.normalizedAlpha();
  for(size_t i = 0; i < 3; ++i) {
    pixel[i] = (byte) ((1 - alpha)*pixel[i] + alpha*color[i]);
  }
}

void Picture::writeToTGA(const char* fname) const {
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

Color Picture::color(int x, int y) const {
  if( x < 0 || (size_t)x >= m_w || y < 0 || (size_t)y >= m_h )
    return Color(0,0,0,255);
  byte *pixel = m_pic + 3*m_w*y + 3*x;
  return Color(pixel[0], pixel[1], pixel[2], 255);
}


double Picture::distance(const Picture& pic) const {
  assert(m_w == pic.m_w && m_h == pic.m_h);
  double dsum = 0;
  for(size_t x = 0; x < m_w; ++x) {
    for(size_t y = 0; y < m_h; ++y) {
      Color c1 = color(x,y), c2 = pic.color(x,y);
      double tmp = 0;
      for(size_t i = 0; i < 3; ++i) {
        double p = c1[i] - (double)c2[i];
        tmp += p*p;
      }
      dsum += sqrt(tmp);
    }
  }
  return dsum;
}



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
      lineEnds[i] = std::max(x1 + 1, lineEnds[i]);
    } else{
      for(int x = x1; x <= lineEnds[i]; ++x) paintColor(x, y1, color);
      lineEnds[i] = std::min(x1 - 1, lineEnds[i]);
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

// Returns whether the last point is on right or leftside
bool Picture::gatherEndPoints(Edge *edges, std::vector<int>& endPoints) const {
  // This is subprocedure used in rasterization and colorAverage
  assert(endPoints.size() == 0);
  std::sort(edges, edges+3);
  size_t height = edges[2].height();
  endPoints.resize(height);

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

  // The idea is to fill vector endPoints to tell which is the farthest
  // point to color at the same y-coordinate, when handling the last two
  // edges. 
  endPoints[i] = x1;

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
    if(ny != y1) endPoints[i] = nx;
    else if(nx > x1 && !onRightSide) endPoints[i] = nx;
    else if(nx < x1 && onRightSide) endPoints[i] = nx;
    x1 = nx, y1 = ny;
  }
  assert((size_t)i == height-1);
  if(edges[1].v1.y < edges[0].v1.y) std::swap(edges[1], edges[0]);
  return onRightSide;
}


void Picture::paintTriangle(const Triangle& triangle) {
  Edge edges[3];
  for(int i = 0; i < 3; ++i) {
    edges[i] = Edge(triangle.vertex(i), triangle.vertex((i+1)%3));
  }
  std::vector<int> lineEndpoints;
  bool onRightSide = gatherEndPoints(edges, lineEndpoints);
  int i = 0;
  i = scanLine(edges[0], triangle.color(), lineEndpoints, onRightSide, i);
  scanLine(edges[1], triangle.color(), lineEndpoints, onRightSide, i);
}

// This is used for sampling the colors inside given triangle
int Picture::scanLine(Edge& edge, std::vector<int>& lineEnds,  bool rightSide,
                      int i, size_t* rgb, size_t& pix) const
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
      for(int x = x1; x >= lineEnds[i]; --x) {
        if(insidePicture(x,y1)) {
          ++pix;
          Color c = color(x, y1);
          for(int i = 0; i < 3; ++i) rgb[i] += c[i];
        }
      }
      lineEnds[i] = std::max(x1 + 1, lineEnds[i]);
    } else{
      for(int x = x1; x <= lineEnds[i]; ++x) {
        if(insidePicture(x,y1)) {
          ++pix;
          Color c = color(x, y1);
          for(int i = 0; i < 3; ++i) rgb[i] += c[i];
        }
      }
      lineEnds[i] = std::min(x1 - 1, lineEnds[i]);
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


Color Picture::getAverageColor(const ivec2& v1, const ivec2& v2, const ivec2& v3) const
{
  // This algorithm is basically the same algorithm as in rasterization
  Edge edges[3];
  edges[0] = Edge(v1, v2);
  edges[1] = Edge(v3, v2);
  edges[2] = Edge(v1, v3);

  std::vector<int> lineEndpoints;
  bool onRightSide = gatherEndPoints(edges, lineEndpoints);
  int i = 0;
  size_t rgb[4] = {0}, pix = 0;
  i = scanLine(edges[0], lineEndpoints, onRightSide, i, rgb, pix);
  scanLine(edges[1], lineEndpoints, onRightSide, i, rgb, pix);
  if(pix == 0) return Color(0,0,0,0);
  for(int i = 0; i < 3; ++i) rgb[i] /= pix;
  rgb[3] = 255;
  return Color(rgb);
}

} //namespace aicha


