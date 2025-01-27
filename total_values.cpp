#include <iostream>
#include <fstream>
#include <string>
#include<vector>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

void total_values(string filepath){
    // string filename = "Velocityf25.binLE.raw_corrected_2_subsampled";
    // cout << "Enter the filename: ";
    // cin >> filename;
    cout<<filepath<<endl;
    FILE *file = fopen(filepath.c_str(), "rb");

    int count = 0;

    while(!feof(file)) {
        float value;
        fread(&value, sizeof(float), 1, file);
        // cout<<value<<" ";
        if(value > 5000 || value < -5000) cout<<value<<" ";
        count++;
    }

    fclose(file);

    cout << "Number of values in the file: " << count << endl;
}

int main() {
    string folder_path = "Isabel_data_all variables_raw";
    vector<string> filepaths;
    for (const auto & entry : fs::directory_iterator(folder_path)) {
        if (entry.is_regular_file()) {
            filepaths.push_back(entry.path().string());
        }
    }
    // for(int i=0;i<filepaths.size();i++){
    //     total_values(filepaths[i]);
    // }
    total_values("reconstructed_data/0.raw_corrected");
    total_values("reconstructed_data/1.raw_corrected");
    return 0;
}