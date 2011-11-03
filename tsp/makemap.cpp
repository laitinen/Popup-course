// Ainoastaan kaupungit.txt ja etaisyydet.txt - tiedostoon
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
  int taydellinen[327][327];
  int lopullinen[327][327];
  int polku[327];
  for(size_t i = 0; i < 327; ++i) {
    polku[i] = i;
    for(size_t j = 0; j < 327; ++j) {
      taydellinen[i][j] = (i==j)?0:-1;
      lopullinen[i][j] = taydellinen[i][j];
    }
  }
  int kaaria = 0;
  while(!cin.eof()) {
    int t,f,d;
    cin >> t >> f >> d;
    if(f > 326 || t > 326) continue;
    taydellinen[t][f] = taydellinen[f][t] = d;
  }
  srand(time(0));
  random_shuffle(polku, polku + 327);
  for(size_t i = 0; i < 327; ++i) {
    int im = (i+1)%327;
    ++kaaria;
    lopullinen[polku[i]][polku[im]] = lopullinen[polku[im]][polku[i]] = taydellinen[polku[i]][polku[im]];
  }
  for(size_t i = 0; i < 6000; ++i) {
    int f = rand()%327;
    int t = rand()%327;
    if(f==t || lopullinen[t][f] != -1) continue;
    ++kaaria;
    lopullinen[t][f] = lopullinen[f][t] = taydellinen[f][t];
  }
  cout << 327 << endl << kaaria << endl;
  for(size_t i = 0; i < 327; ++i) 
    for(size_t j = i+1; j < 327; ++j) 
      if(lopullinen[i][j] != -1) cout << i << " " << j << " " << lopullinen[i][j] << endl;

}
