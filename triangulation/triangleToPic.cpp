#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>
#include <cmath>
#include <vector>
#include <cassert>

typedef unsigned char byte;

class Picture {
 public:
  Picture(int w, int h) : m_w(w), m_h(h) {
    m_pic = new byte[m_w*m_h*3];
    for(size_t i = 0; i < m_w*m_h*3; ++i) {
      m_pic[i] = 255;
    }
  }

  Picture(const Picture& other) : m_w(other.m_w), m_h(other.m_h) {
    m_pic = new byte[m_w*m_h*3];
    std::copy(other.m_pic, other.m_pic + m_w*m_h*3, m_pic);
  }

  ~Picture() { delete [] m_pic; }

  Picture& operator=(const Picture& other) {
    Picture tmp(other);
    std::swap(m_pic, tmp.m_pic);
    std::swap(m_w, tmp.m_w);
    std::swap(m_h, tmp.m_h);
  }

  void paintColor(int x, int y, int *color) {
    if( x < 0 || x >= m_w || y < 0 || y >= m_h ) return;
    byte *pixel = (m_pic + 3*m_w*y + 3*x);
    for(size_t i = 0; i < 3; ++i) {
      double alpha = color[3]/255.0;
      pixel[i] = (byte) ((1 - alpha)*pixel[i] + alpha*color[i]);
    }
  }
  
  void writeToTGA(const char* fname) {
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

 private:
  int m_w, m_h;
  byte *m_pic;

};

struct ivec2 {
  int x, y;
};

bool operator==(const ivec2& v1, const ivec2& v2) {
  return v1.x == v2.x && v1.y == v2.y;
}

int dot(const ivec2& v1, const ivec2& v2) {
  return v1.x*v2.x + v1.y*v2.y;
}

ivec2 operator-(const ivec2& v1, const ivec2& v2) {
  ivec2 v = {v1.x - v2.x, v1.y - v2.y};
  return v;
}

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

int scanLine(Picture& pic, Edge& edge, int *color, std::vector<int>& lineEnds, bool rightSide, int i) {
  if(edge.v1.y == edge.v2.y) {
    if(rightSide && edge.v1.x > edge.v2.x) std::swap(edge.v1, edge.v2);
    else if(!rightSide && edge.v1.x < edge.v2.x) std::swap(edge.v1, edge.v2);
  }

  int dx = abs(edge.v1.x - edge.v2.x), dy = edge.height()-1;
  int sx = (edge.v1.x < edge.v2.x)?1:-1;
  int err = dx - dy;
  int x1 = edge.v1.x, y1 = edge.v1.y, x2 = edge.v2.x, y2 = edge.v2.y;

  for(;;) {
    if(rightSide) {
      for(int x = x1; x > lineEnds[i]; --x) pic.paintColor(x, y1, color);
    } else {
      for(int x = x1; x < lineEnds[i]; ++x) pic.paintColor(x, y1, color);
    }
    lineEnds.at(i) = x1;
    if(x1 == x2 && y1 >= y2) break;
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

double cosAngle(const ivec2& v1, const ivec2& v2) {
  double n1 = sqrt(dot(v1,v1)), n2 = sqrt(dot(v2,v2));
  std::cout << "cosAngle of (" << v1.x << ", " << v1.y << ") and ("
            << v2.x << ", " << v2.y << ") is " << dot(v1,v2)/(n1*n2) << std::endl;
  return dot(v1,v2)/(n1*n2);
}


void rasterizeTriangle(Picture& pic, ivec2* vertices, int *color) {
  Edge edges[3];
  for(int i = 0; i < 3; ++i) {
    edges[i] = Edge(vertices[i], vertices[(i+1)%3]);
  }
  std::sort(edges, edges+3);
  size_t height = edges[2].height();
  size_t offset = edges[2].v2.y;
  std::vector<int> lineEndpoints;
  lineEndpoints.resize(height);
  std::fill(lineEndpoints.begin(), lineEndpoints.end(), -2);

  // Now apply Bresenham's line rasterization algorithm for lines!
  // First the heightest line is rasterized
  // Decide whether the remaining point is on the left or the right side of the
  // longest edge by using the properties of cross-product
  ivec2 remPoint = (edges[0].v1 == edges[2].v1 || edges[0].v1 == edges[2].v2)?edges[0].v2:edges[0].v1;
  ivec2 longestVec = edges[2].v2 - edges[2].v1;
  ivec2 remVec = remPoint - edges[2].v1;
  bool right = (longestVec.x*remVec.y - longestVec.y*remVec.x)  < 0;

  if(right) std::cout << "right" << std::endl;
  else std::cout << "left" << std::endl;

  int dx = abs(edges[2].v1.x - edges[2].v2.x), dy = height-1;
  int sx = (edges[2].v1.x < edges[2].v2.x)?1:-1;
  int i = 0;
  int err = dx - dy;
  int x1 = edges[2].v1.x, y1 = edges[2].v1.y, x2 = edges[2].v2.x, y2 = edges[2].v2.y;


  for(;;) {
    pic.paintColor(x1,y1, color);
    if(x1 == x2 && y1 == y2) break;
    int e2 = 2*err;
    if(e2 > -dy) {
      err -= dy;
      x1 += sx;
      if(right && sx > 0) lineEndpoints[i] = x1;
      else if(!right && sx < 0) lineEndpoints[i] = x1;
    }
    if (e2 < dx) {
      err += dx;
      if(lineEndpoints[i] == -2) lineEndpoints[i] = x1;
      ++i;
      ++y1;
    }
  }
  assert(i == height-1);
  if(edges[1].v1.y < edges[0].v1.y) std::swap(edges[1], edges[0]);
  i = 0;
  i = scanLine(pic, edges[0], color, lineEndpoints, right, i);
  scanLine(pic, edges[1], color, lineEndpoints, right, i);

}




int main(int argc, char **argv) {
  if(argc < 3) {
    std::cout << argv[0] << " generates tga-image of a triangle-file format." << std::endl;
    std::cout << "Usage " << argv[0] << " input output" << std::endl;
    return 0;
  }
  int h, w, n;
  std::ifstream in(argv[1]);
  if(!in) {
    std::cout << "Error opening file " << argv[1] << std::endl;
  }
  in >> w >> h >> n;
  Picture pic(w,h);

  for(size_t i = 0; i < n; ++i) {
    ivec2 V[3];
    int rgba[4];
    for(size_t j = 0; j < 3; ++j) {
      in >> V[j].x >> V[j].y;
    }
    for(size_t j = 0; j < 4; ++j) {
      in >> rgba[j];
      if(rgba[j] < 0) rgba[j] = 0;
      if(rgba[j] > 255) rgba[j] = 255;
    }
    rasterizeTriangle(pic, V, rgba);
  }

  pic.writeToTGA(argv[2]);

}
