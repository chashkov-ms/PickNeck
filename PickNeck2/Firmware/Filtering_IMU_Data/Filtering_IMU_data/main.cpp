#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>
#include <stdlib.h>

extern "C" {
#include "filter.h"
}

using namespace std;

vector<float> split(string read_data, string delimiter){
    vector<float> list;
    size_t pos = 0;
    string token;
    float data;
    while ((pos = read_data.find(delimiter)) != string::npos) {
        token = read_data.substr(0, pos);
        istringstream word(token);
        word >> data;
        list.push_back(data);
        read_data.erase(0, pos + delimiter.length());
    }
    return list;
}

int main()
{
    string s;
    IMU_data read_imu_data;
    osanka_angle calculate_angle;
    ifstream data_file ("../../../../Test_IMU_data.txt");
    if (!data_file.is_open())
    {
        cout << "File not open!" << endl;
        return -1;
    }


    while (data_file >> s)
    {
        read_imu_data.acc_x = split(s, ",").at(0);
        read_imu_data.acc_y = split(s, ",").at(1);
        read_imu_data.acc_z = split(s, ",").at(2);
        read_imu_data.gyr_x = split(s, ",").at(3);
        read_imu_data.gyr_y = split(s, ",").at(4);
        read_imu_data.gyr_z = split(s, ",").at(5);
        calculate_angle = filtering(read_imu_data);

        cout << "Pitch: " << calculate_angle.pitch << endl;
        cout << "Roll: " <<calculate_angle.roll << endl;
        cout << endl;
    }

    test();

    cout << "Hello World!" << endl;
    return 0;
}
