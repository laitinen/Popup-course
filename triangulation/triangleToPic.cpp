#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>
#include <cmath>
#include <vector>

class Picture {
 public:
  Picture(int w, int h) : m_w(w), m_h(h) {
    m_pic = new int[m_w*m_h*3];
    for(size_t i = 0; i < m_w*m_h*3; ++i) {
      m_pic[i] = 0;
    }
  }

  Picture(const Picture& other) : m_w(other.m_w), m_h(other.m_h) {
    m_pic = new int[m_w*m_h*3];
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
    int *pixel = (m_pic + 3*m_w*y + 3*x);
    for(size_t i = 0; i < 3; ++i)
      pixel[i] = (int) (((255 - color[3])/255.0)*pixel[i]);
  }
  
 private:
  int m_w, m_h;
  int *m_pic;

};

struct ivec2 {
  int x, y;
};

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

  // Now apply Bresenham's line rasterization algorithm for lines!
  // First the longest line, store also the endpoint for scanlines
  int dx = abs(edges[2].v1.x - edges[2].v2.x);
  int sx = (edges[2].v1.x < edges[2].v2.x)?1:-1;
  int i = 0;
  int err = dx - height;
  int x1 = edges[2].v1.x, y1 = edges[2].v1.y, x2 = edges[2].v2.x, y2 = edges[2].v2.y;


  for(;;) {
    pic.paintColor(x1,y1, color);
    if(x1 == x2 && y1 == y2) break;
    int e2 = 2*err;
    if(e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      ++y1;
    }
  }
  

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

}
