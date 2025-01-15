#include <bits/stdc++.h>
using namespace std;
int b_dim = 5, z_dim = 50, x_dim = 250, y_dim = 250;

vector<int> generate_histogram(vector<vector<vector<float>>> block, int h_dim, float mini, float maxi)
{
    vector<int> histogram(h_dim, 0);
    float h_width = (maxi - mini) / h_dim;
    int b_dim = block.size();
    // cout<<mini<<" "<<maxi<<" "<<h_width<<endl;
    // cout<<"mini:"<<mini<<" maxi:"<<maxi<<endl;
    for (int i = 0; i < b_dim; i++)
    {
        for (int j = 0; j < b_dim; j++)
        {
            for (int k = 0; k < b_dim; k++)
            {
                int h_no = (block[i][j][k] - mini) / h_width;
                if(h_no == h_dim) h_no--;
                histogram[h_no]++;
            }
        }
    }
    // for(int i=0;i<h_dim;i++){
    //     cout<<histogram[i]<<" ";
    // }
    // cout << endl;
    return histogram;
}

void print_block(vector<vector<vector<float>>> block)
{
    int b_dim = block.size();
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < 10; k++)
            {
                cout << block[i][j][k] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}


float calculate_covariance(vector<vector<vector<float>>> &data_points1, vector<vector<vector<float>>> &data_points2){
    int n = data_points1.size();
    float mean1 = 0, mean2 = 0;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int t=0;t<n;t++){
                mean1 += data_points1[i][j][t]/ pow(n, 3);
                mean2 += data_points2[i][j][t] / pow(n, 3);
            }
        }
    }
    // cout<<"means"<<" ";
    // cout<<mean1<<" "<<mean2<<endl;
    float numerator = 0, denominator = 0;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int k=0;k<n;k++){
                numerator += (data_points1[i][j][k] - mean1) * (data_points2[i][j][k] - mean2);
            }
        }
    }
    float sub_deno1 = 0, sub_deno2 = 0;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int k=0;k<n;k++){
                sub_deno1 += pow(data_points1[i][j][k] - mean1, 2);
                sub_deno2 += pow(data_points2[i][j][k] - mean2, 2);
            }
        }
    }
    denominator = sqrt(sub_deno1) * sqrt(sub_deno2);

    float covariance = numerator / denominator;
    cout<<"covariance "<<covariance<<endl;
    // covariances.push_back(covariance);
    return covariance;
}

void analyse(vector<string> filepaths, int b_dim, int z_dim, int x_dim, int y_dim){
    int blocks = (z_dim * x_dim * y_dim) / pow(b_dim, 3);
    vector<FILE*> files;
    for(auto file: filepaths){
        FILE *f = fopen(file.c_str(), "rb");
        if(f == NULL){
            cerr<<"Error opening file"<<endl;
            return;
        }
        files.push_back(f);
    }
    map<int, vector<vector<int>> > file_histogram;
    vector<vector<vector<float>>> block_covariances(blocks);
    for(int block_no = 0; block_no < blocks; block_no++){
        vector<vector<vector<vector<float>>>> block(filepaths.size(), vector<vector<vector<float>>>(b_dim, vector<vector<float>>(b_dim, vector<float>(b_dim, 0))));
        for(int t=0;t<filepaths.size();t++){
            FILE *file = files[t];
            fseek(file, 0, SEEK_SET);
            float mini = INT_MAX, maxi = INT_MIN;
            for (int z = 0; z < z_dim; z++)
            {
                for (int y = 0; y < y_dim; y++)
                {
                    for (int x = 0; x < x_dim; x++)
                    {
                        int curr_block_no = z / b_dim * (x_dim / b_dim) * (y_dim / b_dim) + y / b_dim * (x_dim / b_dim) + x / b_dim;
                        float value;
                        fread(&value, sizeof(float), 1, file);
                        if (curr_block_no == block_no){
                            block[t][z % b_dim][y % b_dim][x % b_dim] = value;
                            mini = min(mini, value);
                            maxi = max(maxi, value);
                        }
                    }
                }
            }
            vector<int> histogram = generate_histogram(block[t], 10, mini, maxi);
            file_histogram[t].push_back(histogram);
            // print_block({data_points});
        }
        vector<vector<float>> covariances(filepaths.size(), vector<float>(filepaths.size(), 0));
        for(int i=0;i<filepaths.size();i++){
            for(int j=i+1;j<filepaths.size();j++){
                float covariance = calculate_covariance(block[i], block[j]);
                covariances[i][j] = covariance;
            }
        }
        
    }
    cout<<"done"<<endl;
    // for(auto histogram: file_histogram){
    //     cout<<histogram.first<<" "<<histogram.second.size()<<endl;
    // }
}


int main()
{
    analyse({"Velocityf25.binLE.raw_corrected_2_subsampled", "Pf25.binLE.raw_corrected_2_subsampled"}, 25, 50, 250, 250);
    return 0;
}
