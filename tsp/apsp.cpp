#include <iostream>
#include <vector>

using namespace std;
typedef vector<vector<int> > VVI;

void fw(VVI& G) {
    int cc = G.size();
    for(int k = 0; k < cc; ++k) {
        for(int i = 0; i < cc; ++i) {
            for(int j = 0; j < cc; ++j) {
                G[i][j] = min(G[i][j], G[i][k] + G[k][j]);
            }
        }
    }

    cout<<cc<<endl;

    for(int i = 0; i < cc; ++i) {
        for(int j = 0; j < cc; ++j) {
            cout<<G[i][j]<<" ";
        }
        cout<<endl;
    }
}

int main() {
    int cc;
    cin>>cc;
    VVI G(cc,vector<int>(cc)); 

    for(int i = 0; i < cc; ++i) {
        for(int j = 0; j < cc; ++j) {
            cin>>G[i][j];
            if(G[i][j] == -1) G[i][j] = (int)1e8;
        }
    }

    fw(G);    

    return 0;
}
