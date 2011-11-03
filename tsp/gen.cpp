#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <utility>

using namespace std;

int main(int argc, char** argv) {
    if(argc < 2) {
        cout<<"City count needed, finishing"<<endl;
        return 0;
    }
    int cc = atoi(argv[1]);
    srand((unsigned)time(0));

    cout<<cc<<endl;

    vector<pair<int,int> > E;

    for(int i = 0; i < cc; ++i) {
        for(int j = i + 1; j < cc; ++j) {
            if(rand() % 10 < 1) {
                E.push_back(make_pair(i,j));
            }
        }
    }

    cout<<E.size()<<endl;
    
    for(int i = 0; i < (int)E.size(); ++i) {
        cout<<E[i].first<<" "<<E[i].second<<" "<<(rand()%100+1)<<endl;
    }

    return 0;
}
