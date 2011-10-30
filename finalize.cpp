#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

using namespace std;

vector<vector<pair<int,int> > > G;

struct sit {
    int x;
    int cost;
    sit* prev;
};

struct cmp {
    bool operator () (const sit* s1, const sit* s2) {
        return s1->cost > s2->cost;
    }
};

int main(int argc, char** argv) {
    // first is the actual map
    int cc, ec;
    ifstream fin(argv[1]);
    fin>>cc>>ec;
    G.resize(cc);
    for(int i = 0; i < ec; ++i) {
        int x,y,w;
        fin>>x>>y>>w;
        G[x].push_back(make_pair(y,w));
        G[y].push_back(make_pair(x,w));
    }
    fin.close();
    // second is the concorde-solution
    fin.open(argv[2]);
    int rl;
    fin>>rl;
    vector<int> R(rl);
    for(int i = 0; i < rl; i++) fin>>R[i];
    R.push_back(R[0]);
    fin.close();

    // run Dijkstra multiple times to find the actual route
    vector<int> F; F.push_back(R[0]);

    for(int i = 0; i < (int)R.size() - 1; ++i) {
        vector<bool> used(cc);
        priority_queue<sit*,vector<sit*>,cmp> Q;
        sit* s = new sit;
        s->x = R[i];
        s->cost = 0;
        s->prev = NULL;

        Q.push(s);
        sit* cur;
        while(!Q.empty()) {
            cur = Q.top(); Q.pop();
            if(used[cur->x]) continue;
            used[cur->x] = true;
            if(cur->x == R[i+1]) break;
            for(int j = 0; j < (int)G[cur->x].size(); ++j) {
                sit* is = new sit;
                is->x = G[cur->x][j].first;
                is->cost = cur->cost + G[cur->x][j].second;
                is->prev = cur;
                Q.push(is);
            }
        }
        vector<int> C;
        while(cur != NULL) {
            C.push_back(cur->x);
            cur = cur->prev;
        }
        for(int i = C.size() - 2; i >= 0; --i) {
            F.push_back(C[i]);
        }
    }

    for(int i = 0; i < (int)F.size(); ++i) {
        cout<<F[i]<<endl;
    }

    return 0;
}
