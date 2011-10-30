#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    int dim;
    cin>>dim;
    cout<<"NAME: "<<argv[1]<<endl;
    cout<<"TYPE: TSP"<<endl;
    cout<<"DIMENSION: "<<dim<<endl;
    cout<<"EDGE_WEIGHT_TYPE: EXPLICIT"<<endl;
    cout<<"EDGE_WEIGHT_FORMAT: FULL_MATRIX"<<endl;
    cout<<"DISPLAY_DATA_TYPE: NO_DISPLAY"<<endl;
    cout<<"EDGE_WEIGHT_SECTION"<<endl;

    for(int i = 0; i < dim; ++i) {
        for(int j = 0; j < dim; ++j) {
            int a; cin>>a; cout<<a<<" ";
        }
        cout<<endl;
    }

    cout<<"EOF"<<endl;
    return 0;
}
