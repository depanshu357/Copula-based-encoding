#include<bits/stdc++.h>
using namespace std;

const int file_count = 2, h_dim = 10,b_dim = 5, z_dim = 50, x_dim = 250, y_dim = 250;

void file_reader(string filepath){
    FILE *file = fopen(filepath.c_str(), "rb");
    if(file == NULL){
        cerr<<"Error opening file"<<endl;
        return;
    }
    int blocks = (z_dim * x_dim * y_dim) / (b_dim * b_dim * b_dim);
    vector<vector<vector<float>>> file_histogram(file_count, vector<vector<float>>(blocks, vector<float>(h_dim, 0)));
    for(int i=0;i<file_count;i++){
        for(int j=0;j<blocks;j++){
            for(int k=0;k<h_dim;k++){
                int val;
                fread(&val, sizeof(int), 1, file);
                file_histogram[i][j][k] = val;
                // cout<<val<<" ";
            }
        }
    }
    vector<vector<pair<float,float>>> file_histogram_min_max(file_count,vector<pair<float,float>>(blocks));
    for(int i=0;i<file_count;i++){
        for(int j=0;j<blocks;j++){
            float val;
            fread(&val, sizeof(float), 1, file);
            file_histogram_min_max[i][j].first = val;
            // cout<<val<<" ";
            fread(&val, sizeof(float), 1, file);
            file_histogram_min_max[i][j].second = val;
            // cout<<val<<" ";
        }
    }
    int block_covariances_size = ((file_count + 3) * (file_count + 2)) / 2;
    map<pair<pair<int,int>,int>, float> block_covariances;
    for(int i=0;i<block_covariances_size;i++){
        for(int j=0;j<blocks;j++){

        int m,n,t;
        float val;
        fread(&m, sizeof(int), 1, file);
        fread(&n, sizeof(int), 1, file);
        fread(&t, sizeof(int), 1, file);
        fread(&val, sizeof(float), 1, file);
        block_covariances[make_pair(make_pair(m,n),t)] = val;
        // cout<<m<<" "<<n<<" "<<t<<" "<<val<<endl;
        }
    }        


    
}

int main(){
    file_reader("output.bin");
    cout<<"Done"<<endl;
}