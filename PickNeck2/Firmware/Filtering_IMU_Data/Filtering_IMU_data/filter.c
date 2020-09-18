#include <stdio.h>
#include "math.h"
#include "filter.h"
#include "quaternion.h"


osanka_angle filtering (IMU_data input_imu_data)
{
    osanka_angle angles;

    float pitch = input_imu_data.gyr_x*10;
    float roll = input_imu_data.gyr_y*10;
    angles.pitch = pitch;
    angles.roll = roll;

    return angles;
}


quaternion_t calibrating (IMU_data input_imu_data)
{
    static quaternion_t calibration_q = {0, 1, 1, 1};

    return calibration_q;
}


void test (void)
{
    static quaternion_t test_q1 = {5, 1, 1, 1};
    static quaternion_t test_q2 = {5, 2, 3, 4};
    vector_t vect = {1, 0, 0};
    quaternion_t test_q3 = create(3.14/2, vect);

    printf("test 1, i = %f \n", conj(test_q2).i);
    printf("test 2, i = %f \n", norm(test_q2));
    printf("test 3, i = %f \n", normalise(test_q2).j);
    printf("test 4, fi = %f \n", multiply(test_q1, test_q2).fi);
    printf("test 4, i = %f \n", multiply(test_q1, test_q2).i);
    printf("test 4, j = %f \n", multiply(test_q1, test_q2).j);
    printf("test 4, k = %f \n", multiply(test_q1, test_q2).k);

    printf("test 5, fi = %f \n", reverse(test_q2).fi);
    printf("test 5, i = %f \n", reverse(test_q2).i);
    printf("test 5, j = %f \n", reverse(test_q2).j);
    printf("test 5, k = %f \n", reverse(test_q2).k);

    printf("test 6, fi = %f \n", test_q3.fi);
    printf("test 6, i = %f \n", test_q3.i);
    printf("test 6, j = %f \n", test_q3.j);
    printf("test 6, k = %f \n", test_q3.k);



}
