#include <iostream>
#include <vector>

using namespace std;

vector<bool> used;

void dfs(const vector<vector<int> >& G, int a) {
    used[a] = true;
    for(int i = 0; i < (int)G[a].size(); ++i) {
        if(G[a][i] && !used[i]) {
            dfs(G,i);
        }
    }
}

int main() {
    int cc,ec;
    cin>>cc>>ec;
    used.resize(cc);
    vector<vector<int> > G(cc,vector<int>(cc));
    int x,y,w;
    for(int i = 0; i < ec; ++i) {
        cin>>x>>y>>w;
        G[x][y] = 1;
        G[y][x] = 1;
    }

    dfs(G,0);

    for(int i = 0; i < (int)used.size(); ++i) {
        if(!used[i]) {
            cout<<"Is NOT connected"<<endl;
            return 0;
        }
    }

    cout<<"Is connected"<<endl;

    return 0;
}
