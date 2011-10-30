#include <iostream>
#include <vector>

using namespace std;
typedef vector<vector<int> > VVI;

int main() {
    int cc, ec;
    cin>>cc>>ec;
    VVI G(cc,vector<int>(cc,-1));
    for(int i = 0; i < cc; ++i) G[i][i] = 0;
    for(int i = 0; i < ec; ++i) {
        int x,y,w;
        cin>>x>>y>>w;
        G[x][y] = w;
        G[y][x] = w;
    }

    cout<<cc<<endl;
    for(int i = 0; i < cc; ++i) {
        for(int j = 0; j < cc; ++j) {
            cout<<G[i][j]<<" ";
        }
        cout<<endl;
    }
    return 0;
}
