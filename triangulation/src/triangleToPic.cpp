#include "Common.hpp"
#include "Picture.hpp"
#include "Triangle.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
  using namespace aicha;

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

  for(int i = 0; i < n; ++i) {
    int V[6];
    int rgba[4];
    for(size_t j = 0; j < 3; ++j) {
      in >> V[2*j] >> V[2*j + 1];
    }
    for(size_t j = 0; j < 4; ++j) {
      in >> rgba[j];
      if(rgba[j] < 0) rgba[j] = 0;
      if(rgba[j] > 255) rgba[j] = 255;
    }
    Triangle tr(V, Color(rgba));
    pic.paintTriangle(tr);
  }
  pic.writeToTGA(argv[2]);
}
