#include "quaternion.h"
#include "math.h"

quaternion_t create (float angle, vector_t v)
{
    quaternion_t result;

    result.fi = cos(angle/2);
    result.i = v.i*sin(angle/2);
    result.j = v.j*sin(angle/2);
    result.k = v.k*sin(angle/2);

    return result;
}

quaternion_t multiply(quaternion_t q1, quaternion_t q2)
{
    quaternion_t result;

    result.fi = q1.fi*q2.fi - q1.i*q2.i - q1.j*q2.j - q1.k*q2.k;
    result.i = q1.fi*q2.i + q1.i*q2.fi + q1.j*q2.k - q1.k*q2.j;
    result.j = q1.fi*q2.j - q1.i*q2.k + q1.j*q2.fi + q1.k*q2.i;
    result.k = q1.fi*q2.k + q1.i*q2.j - q1.j*q2.i + q1.k*q2.fi;

    return result;
}

quaternion_t c_multiply(quaternion_t q1, float c)
{
    quaternion_t result;

    result.fi = q1.fi * c;
    result.i = q1.i * c;
    result.j = q1.j * c;
    result.k = q1.k * c;

    return result;
}

quaternion_t add (quaternion_t q1, quaternion_t q2)
{
    quaternion_t result;

    result.fi = q1.fi + q2.fi;
    result.i = q1.i + q2.i;
    result.j = q1.j + q2.j;
    result.k = q1.k + q2.k;

    return result;
}

quaternion_t sub (quaternion_t q1, quaternion_t q2)
{
    quaternion_t result;

    result.fi = q1.fi - q2.fi;
    result.i = q1.i - q2.i;
    result.j = q1.j - q2.j;
    result.k = q1.k - q2.k;

    return result;
}

quaternion_t conj(quaternion_t q)
{
    quaternion_t result;

    result.fi = q.fi;
    result.i = -q.i;
    result.j = -q.j;
    result.k = -q.k;

    return result;
}

float norm(quaternion_t q)
{
    return q.fi*q.fi + q.i*q.i + q.j*q.j + q.k*q.k;
}

quaternion_t normalise(quaternion_t q)
{
    quaternion_t result;
    double q_length = sqrt(norm(q));

    result.fi = q.fi/q_length;
    result.i = q.i/q_length;
    result.j = q.j/q_length;
    result.k = q.k/q_length;

    return result;
}

quaternion_t reverse(quaternion_t q)
{
    quaternion_t result;
    float q_norm = norm(q);

    result = c_multiply(conj(q), 1/q_norm);
    return result;
}
