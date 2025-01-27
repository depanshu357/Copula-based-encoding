#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <random>
using namespace std;

const int file_count = 2, h_dim = 10,b_dim = 5, z_dim = 50, x_dim = 250, y_dim = 250;

void generate_distribution_for_file(vector<vector<float>> &histogram, vector<pair<float,float>> &histogram_min_max, map<pair<pair<int,int>,int>, float> &block_covariances, int file_no){
    random_device rd;
    mt19937 gen(rd());
    int total_samples = b_dim * b_dim * b_dim;
    float ***data = new float**[z_dim];
    for(int z=0; z< z_dim; z++){
        data[z] = new float*[y_dim];
        for(int y=0; y<y_dim; y++){
            data[z][y] = new float[x_dim];
        }
    }
    int blocks = (z_dim * x_dim * y_dim) / (b_dim * b_dim * b_dim);
    for(int block_no=0;block_no<blocks;block_no++){
        vector<float> block_histogram = histogram[block_no];
        pair<float,float> block_min_max = histogram_min_max[block_no];
        float bin_width = (block_min_max.second - block_min_max.first) / h_dim;
        vector<float> block(b_dim * b_dim * b_dim); int counter = 0;
        for(int i=0;i<h_dim;i++){
            uniform_real_distribution<float> dist(i*bin_width + block_min_max.first, (i+1)*bin_width + block_min_max.first);
            for(int j=0;j<block_histogram[i];j++){
                block[counter++] = dist(gen);
                // cout<<block[counter-1]<<" "<<i*bin_width + block_min_max.first<<" "<<(i+1)*bin_width<<endl;
            }
        }
        int b_z_start = (block_no / ((x_dim / b_dim) * (y_dim / b_dim))) * b_dim;
        int b_y_start = (block_no/(x_dim/b_dim)) % (y_dim/b_dim) * b_dim;
        int b_x_start = (block_no % (x_dim/b_dim)) * b_dim;

        counter = 0;
        for(int z=0;z<b_dim;z++){
            for(int y=0;y<b_dim;y++){
                for(int x=0;x<b_dim;x++){
                    // cout<<b_z_start+z<<" "<<b_y_start+y<<" "<<b_x_start+x<<" "<<data[z][y][x]<<endl;
                    data[b_z_start + z][b_y_start + y][b_x_start + x] = block[counter++];
                    cout<<block[counter]<<" ";
                }
            }
        }

    }
    // write the values to a file
    string directory_path = "reconstructed_data";
    string filename = to_string(file_no) + ".raw_corrected";
    string full_path = directory_path + "/" + filename;
    // Create directory if it does not exist
    system(("mkdir -p " + directory_path).c_str());
    FILE *file = fopen(full_path.c_str(), "wb");
    for(int z=0; z< z_dim; z++){
        for(int y=0; y<y_dim; y++){
            for(int x=0; x<x_dim; x++){
                fwrite(&data[z][y][x], sizeof(float), 1, file);
                cout<<data[z][y][x]<<" ";
            }
        }
    }
    fclose(file);

}

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

    for(int i=0;i<file_count;i++){
        generate_distribution_for_file(file_histogram[i], file_histogram_min_max[i], block_covariances, i);
    }

    
}

int main(){
    file_reader("output.bin");
    cout<<"Done"<<endl;
}