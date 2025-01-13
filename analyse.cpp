#include <bits/stdc++.h>
using namespace std;

vector<int> generate_histogram(vector<vector<vector<float>>> block, int h_dim, float mini, float maxi)
{
    vector<int> histogram(h_dim, 0);
    float h_width = (maxi - mini) / h_dim;
    int b_dim = block.size();
    // cout<<mini<<" "<<maxi<<" "<<h_width<<endl;
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

vector<vector<int>> readFile(string filepath, int b_dim, int z_dim, int x_dim, int y_dim)
{
    int blocks = (z_dim * x_dim * y_dim) / pow(b_dim, 3);
    vector<vector<vector<float>>> block(b_dim, vector<vector<float>>(b_dim, vector<float>(b_dim, 0)));
    vector<vector<int>> histograms;
    FILE *file = fopen(filepath.c_str(), "rb");
    if (file == NULL)
    {
        cerr << "Error opening file" << endl;
        return histograms;
    }
    for (int b_no = 0; b_no < blocks; b_no++)
    {
        float mini = INT_MAX, maxi = INT_MIN;
        int count = 0;
        fseek(file, 0, SEEK_SET);
        for (int z = 0; z < z_dim; z++)
        {
            for (int y = 0; y < y_dim; y++)
            {
                for (int x = 0; x < x_dim; x++)
                {
                    int block_no = z / b_dim * (x_dim / b_dim) * (y_dim / b_dim) + y / b_dim * (x_dim / b_dim) + x / b_dim;
                    float value;
                    fread(&value, sizeof(float), 1, file);
                    if (block_no == b_no){
                        count++;
                        mini = min(mini, value);
                        maxi = max(maxi, value); 
                        block[z % b_dim][y % b_dim][x % b_dim] = value;
                    }
                }
            }
        }
        // print_block(block);
        // cout << "block_no:" << b_no << " " << "count:" << count << " "<< maxi<< " " << mini <<endl;
        vector<int> histogram = generate_histogram(block, 10, mini, maxi);
        histograms.push_back(histogram);
    }
    fclose(file);
    return histograms;
}

int main()
{
    vector<vector<int>> velocity = readFile("Velocityf25.binLE.raw_corrected_2_subsampled", 25, 50, 250, 250);
    vector<vector<int>> pressure = readFile("Pf25.binLE.raw_corrected_2_subsampled", 25, 50, 250, 250);
    cout<<"velocity"<<endl;
    for(int i=0;i<velocity.size();i++){
        for(int j=0;j<velocity[i].size();j++){
            cout<<velocity[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"pressure"<<endl;
    for(int i=0;i<pressure.size();i++){
        for(int j=0;j<pressure[i].size();j++){
            cout<<pressure[i][j]<<" ";
        }
        cout<<endl;
    }
    return 0;
}
