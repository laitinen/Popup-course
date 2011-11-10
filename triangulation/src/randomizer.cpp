#include "Common.hpp"
#include "Picture.hpp"
#include "Triangle.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

using namespace aicha;

void randomCoordinates(ivec2* vecs, int w, int h) {
  for(int i = 0; i < 3; ++i) {
    ivec2 v = {(rand() % (int)(1.5*w)) - w*0.25,
               (rand() % (int)(1.5*h)) - h*0.25 };
    vecs[i] = v;
  }
}

void printDef(const char* fname, const std::vector<Triangle>& triangles,
              int w, int h)
{
  std::ofstream out(fname);
  out << w << " " << h << std::endl << triangles.size() << std::endl;
  for(size_t i = 0; i < triangles.size(); ++i) {
    for(size_t j = 0; j < 3; ++j)
      out << triangles[i].vertex(j).x << " " << triangles[i].vertex(j).y << " ";
    Color color = triangles[i].color();
    for(size_t j = 0; j < 3; ++j) 
      out << (int)color[j] << " ";
    out << (int)color[3] << std::endl;
  }
}

int main(int argc, char **argv) {
  if(argc < 4) {
    std::cout << "Usage: " << argv[0]
              << " input.tga output.tr nr_of_triangles"
              << std::endl;
    return 0;
  }
  Picture orig(argv[1]);
  int n = atoi(argv[3]);
  int w = orig.width(), h = orig.height();

  srand(time(0));
  
  Picture best(w, h);
  std::vector<Triangle> triangles;
  // The triangles in beginning, alpha-value is randomized:
  for(int i = 0; i < n; ++i) {
    ivec2 vecs[3];
    randomCoordinates(vecs, w, h);
    Color avg = orig.getAverageColor(vecs[0],vecs[1],vecs[2]);
    avg[3] = (rand() % 235) + 20;
    triangles.push_back(Triangle(vecs, avg));
    best.paintTriangle(triangles.back());
  }
  double bestDistance = best.distance(orig); 

  size_t iter = 1;
  while(true) {
    // First create a triangle with average color to random spot
    ivec2 vecs[3];
    randomCoordinates(vecs, w, h);
    Color avg = orig.getAverageColor(vecs[0],vecs[1],vecs[2]);
    avg[3] = (rand() % 235) + 20;
    Triangle tr(vecs, avg);

    // Swap with the triangle with some existing triangle
    size_t index = rand() % n;
    std::swap(tr, triangles[index]);
    Picture tmp(w,h);
    for(int i = 0; i < n; ++i) tmp.paintTriangle(triangles[i]);

    // And evaluate the change
    double distance = tmp.distance(orig);
    if(distance < bestDistance) {
      std::cout << "Found improved solution at " << iter << "-th iteration." << std::endl;
      std::swap(best, tmp);
      bestDistance = distance;
      printDef(argv[2], triangles, w, h);
    } else {
      std::swap(tr, triangles[index]);
    }

    // See if changing the drawing order would help
    tmp.clearColor();
    int j = rand() % n, k = rand() % n;
    std::swap(triangles[k], triangles[j]);
    for(int i = 0; i < n; ++i) tmp.paintTriangle(triangles[i]);
    distance = tmp.distance(orig);
    if(distance < bestDistance) {
      std::swap(best, tmp);
      bestDistance = distance;
      std::cout << "Found improved solution when changed "
                << "the drawing order of triangles." << std::endl;
    } else {
      std::swap(triangles[k], triangles[j]);
    }

    ++iter;
  }
}
