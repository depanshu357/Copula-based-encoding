#include <bits/stdc++.h>

using namespace std;
namespace fs = std::filesystem;

const int b_dim = 5, z_dim = 50, x_dim = 250, y_dim = 250, h_dim = 10;

void output_file(vector<vector<vector<int>>> file_histogram, vector<vector<pair<float,float>>> file_histogram_min_max, map<pair<pair<int,int>,int>, float> block_covariances){
    FILE *file = fopen("output.bin", "wb");
    for(int i=0;i<file_histogram.size();i++){
        for(int j=0;j<file_histogram[i].size();j++){
            for(int k=0;k<file_histogram[i][j].size();k++){
                fwrite(&file_histogram[i][j][k], sizeof(int), 1, file);
            }
        }
    }
    for(int i=0;i<file_histogram_min_max.size();i++){
        for(int j=0;j<file_histogram_min_max[i].size();j++){
            fwrite(&file_histogram_min_max[i][j].first, sizeof(float), 1, file);
            fwrite(&file_histogram_min_max[i][j].second, sizeof(float), 1, file);
        }
    }
    for(auto it = block_covariances.begin(); it != block_covariances.end(); it++){
        fwrite(&it->first.first.first, sizeof(int), 1, file);
        fwrite(&it->first.first.second, sizeof(int), 1, file);
        fwrite(&it->first.second, sizeof(int), 1, file);
        fwrite(&it->second, sizeof(float), 1, file);
    }
    fclose(file);
}

void print_3d_array(float ***data, int z_dim, int y_dim, int x_dim){
    for(int z=0; z< z_dim; z++){
        // cout<<z<<endl;
        for(int y=0; y<y_dim; y++){
            for(int x=0; x<x_dim; x++){
                // cout<<data[z][y][x]<<" ";
            }
            // cout<<endl;
        }
        // cout<<endl;
    }
}


float calculate_covariance(float block1[b_dim][b_dim][b_dim], float block2[b_dim][b_dim][b_dim]){
    float mean1 = 0, mean2 = 0;
    for(int i=0;i<b_dim;i++){
        for(int j=0;j<b_dim;j++){
            for(int k=0;k<b_dim;k++){
                mean1 += block1[i][j][k]/ (b_dim * b_dim * b_dim);
                mean2 += block2[i][j][k] / (b_dim * b_dim * b_dim);
            }
        }
    }
    float numerator = 0, denominator = 0;
    for(int i=0;i<b_dim;i++){
        for(int j=0;j<b_dim;j++){
            for(int k=0;k<b_dim;k++){
                numerator += (block1[i][j][k] - mean1) * (block2[i][j][k] - mean2);
            }
        }
    }
    float sub_deno1 = 0, sub_deno2 = 0;
    for(int i=0;i<b_dim;i++){
        for(int j=0;j<b_dim;j++){
            for(int k=0;k<b_dim;k++){
                sub_deno1 += pow(block1[i][j][k] - mean1, 2);
                sub_deno2 += pow(block2[i][j][k] - mean2, 2);
            }
        }
    }
    denominator = sqrt(sub_deno1) * sqrt(sub_deno2);

    float covariance = numerator / denominator;
    // cout<<"covariance "<<covariance<<endl;
    // covariances.push_back(covariance);
    return covariance;
}

void analyse(vector<string> filepaths){
    int blocks = (z_dim * x_dim * y_dim) / (b_dim * b_dim * b_dim);
    vector<FILE*> files;
    for(auto file: filepaths){
        FILE *f = fopen(file.c_str(), "rb");
        if(f == NULL){
            cerr<<"Error opening file"<<endl;
            return;
        }
        files.push_back(f);
    }
    vector<float***> file_data_pointers(filepaths.size());
    for(int t=0;t<filepaths.size();t++){
        cout<<filepaths[t]<<endl;
        float ***data = new float**[z_dim];
        FILE *file = files[t];
        float mini = INT_MAX, maxi = INT_MIN;
        for(int z=0; z< z_dim; z++){
            data[z] = new float*[y_dim];
            for(int y=0; y<y_dim; y++){
                data[z][y] = new float[x_dim];
                for(int x=0; x<x_dim; x++){
                    float val;
                    fread(&val, sizeof(float), 1, file);
                    data[z][y][x] = val;
                    if(val > 5000 || val < -5000){
                        cout<<val<<" "; 
                        cerr<<"Error in file "<<filepaths[t]<<endl;
                    }
                }
            }
        }

        file_data_pointers[t] = data;
    }
    vector<vector<vector<int>>> file_histogram(filepaths.size(), vector<vector<int>>(blocks, vector<int>(h_dim, 0)));
    vector<vector<pair<float,float>>> file_histogram_min_max(filepaths.size(),vector<pair<float,float>>(blocks)); 
    map<pair<pair<int,int>,int>, float> block_covariances;
    for(int i=0;i<blocks;i++){
        float block[filepaths.size()+3][b_dim][b_dim][b_dim];
        int b_z_start = (i / ((x_dim / b_dim) * (y_dim / b_dim))) * b_dim;
        for(int z=b_z_start;z<b_z_start+b_dim;z++){
                int b_y_start = (i/(x_dim/b_dim)) % (y_dim/b_dim) * b_dim;
                for(int y=b_y_start; y<b_y_start+b_dim; y++){
                    int b_x_start = (i % (x_dim/b_dim)) * b_dim;
                    for(int x=b_x_start; x<b_x_start+b_dim;x++){
                        block[filepaths.size()][z%b_dim][y%b_dim][x%b_dim] = x;
                        block[filepaths.size()+1][z%b_dim][y%b_dim][x%b_dim] = y;
                        block[filepaths.size()+2][z%b_dim][y%b_dim][x%b_dim] = z;
                    }
                }
        }
        for(int t=0;t<filepaths.size();t++){
        // generate histogram 
            int b_z_start = (i / ((x_dim / b_dim) * (y_dim / b_dim))) * b_dim;
            float mini = INT_MAX, maxi = INT_MIN;
            for(int z=b_z_start;z<b_z_start+b_dim;z++){
                int b_y_start = (i/(x_dim/b_dim)) % (y_dim/b_dim) * b_dim;
                for(int y=b_y_start; y<b_y_start+b_dim; y++){
                    int b_x_start = (i % (x_dim/b_dim)) * b_dim;
                    for(int x=b_x_start; x<b_x_start+b_dim;x++){
                        mini = min(mini, file_data_pointers[t][z][y][x]);
                        maxi = max(maxi, file_data_pointers[t][z][y][x]);
                        block[t][z%b_dim][y%b_dim][x%b_dim] = file_data_pointers[t][z][y][x];
                        
                    }
                }
            }
            file_histogram_min_max[t][i] = {mini, maxi};
            vector<int> histogram(h_dim, 0);
            if(mini!=maxi){
                for(int z=0;z<b_dim;z++){
                    for(int y=0;y<b_dim;y++){
                        for(int x=0;x<b_dim;x++){
                            int bin = int(((block[t][z][y][x] - mini) / (maxi - mini) ) * h_dim);
                            if(bin == h_dim) bin = h_dim - 1;
                            if(block[t][z][y][x] > 5000 || block[t][z][y][x] < -5000){
                                cout<<file_data_pointers[t][z][y][x]<<" ";
                                cerr<<"Error in file "<<filepaths[t]<<endl;
                            }
                            if(bin<0 || bin>=h_dim) {cout<<bin<<" "<<mini<<" "<<maxi<<endl;}
                            histogram[bin]++;
                        }
                    }
                }
            } 
            file_histogram[t][i] = histogram;
            
        }
            // generate covariance matrix
        for(int m=0;m<filepaths.size()+3;m++){
            for(int n=m+1;n<filepaths.size()+3;n++){
                float covariance = calculate_covariance(block[m], block[n]);
                block_covariances[{{m,n},i}] = covariance;
            }
        }
    }

    output_file(file_histogram, file_histogram_min_max, block_covariances);
    cout<<"done"<<endl;
}

int main()
{

    // read all files present in the folder Isabel_data_all_variables_raw and store list of strings of filepaths in a vector
    string folder_path = "Isabel_data_all variables_raw";
    vector<string> filepaths;
    for (const auto & entry : fs::directory_iterator(folder_path)) {
        if (entry.is_regular_file()) {
            filepaths.push_back(entry.path().string());
            // cout<<entry.path().string()<<endl;
        }
    }
    // analyse({"Velocityf25.binLE.raw_corrected_2_subsampled", "Pf25.binLE.raw_corrected_2_subsampled"});
    analyse(filepaths);
    return 0;
}