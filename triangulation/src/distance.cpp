#include "Picture.hpp"

#include <iostream>
#include <fstream>
#include <cmath>

int main(int argc, char **argv) {
  using namespace aicha;
  if(argc < 3) {
    std::cout << "Usage: " <<  argv[0] << " original.tga approximation.tr" << std::endl;
    return 0;
  }
  Picture orig(argv[1]);

  int h, w, n;
  std::ifstream in(argv[2]);
  if(!in) {
    std::cout << "Error opening file " << argv[1] << std::endl;
    return 0;
  }
  in >> w >> h >> n;
  Picture approx(w,h);
  
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
    approx.paintTriangle(tr);
  }
  size_t pixels = orig.width()*orig.height();
  double nDist = approx.distance(orig); 
  std::cout << "With " << n << " triangles, the euclidean distance is "
            << nDist << std::endl;
  std::cout << (nDist + std::max(0.0, pixels*log(n/10.0))) << std::endl;
}
