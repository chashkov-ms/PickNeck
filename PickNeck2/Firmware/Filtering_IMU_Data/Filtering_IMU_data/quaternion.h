#ifndef QUATERNION_H
#define QUATERNION_H

typedef struct quaternion
{
    float fi;
    float i;
    float j;
    float k;
} quaternion_t;

typedef struct q_vector
{
    float i;
    float j;
    float k;
} vector_t;

quaternion_t create (float angle, vector_t v);

quaternion_t add (quaternion_t q1, quaternion_t q2);
quaternion_t sub (quaternion_t q1, quaternion_t q2);
quaternion_t c_multiply(quaternion_t q1, float c);
quaternion_t multiply(quaternion_t, quaternion_t);
quaternion_t conj(quaternion_t q);
float norm(quaternion_t q);
quaternion_t normalise(quaternion_t q);
quaternion_t reverse(quaternion_t q);

#endif // QUATERNION_H
