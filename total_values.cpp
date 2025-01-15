#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
    string filename = "Pf25.binLE.raw_corrected_2_subsampled";
    // string filename = "Velocityf25.binLE.raw_corrected_2_subsampled";
    // cout << "Enter the filename: ";
    // cin >> filename;

    FILE *file = fopen(filename.c_str(), "rb");

    int count = 0;

    while(!feof(file)) {
        float value;
        fread(&value, sizeof(float), 1, file);
        if(value > 5000 || value < -5000) cout<<value<<" ";
        count++;
    }

    fclose(file);

    cout << "Number of values in the file: " << count << endl;
    return 0;
}