#ifndef FILTER_H
#define FILTER_H

#include "quaternion.h"

#define IMU_FREQUENCY 1000

typedef struct imu_data
{
    float acc_x;
    float acc_y;
    float acc_z;
    float gyr_x;
    float gyr_y;
    float gyr_z;
} IMU_data;

typedef struct angle
{
    int roll;
    int pitch;
} osanka_angle;

osanka_angle filtering (IMU_data);
quaternion_t calibrating (IMU_data input_imu_data);

void test (void);

#endif // FILTER_H
