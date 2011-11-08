#include "Picture.hpp"

#include <iostream>

int main(int argc, char** argv) {
  using namespace aicha;
  if(argc < 3) {
    std::cout << "Usage " << argv[0] << " original.tga new.tga" << std::endl;
    return 0;
  }
  Picture pic(argv[1]);
  pic.writeToTGA(argv[2]);
}
