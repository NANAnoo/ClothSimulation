#include "Vec3.h"
#include <math.h>

Vec3::Vec3()
{
    Array[0] = 0;
    Array[1] = 0;
    Array[2] = 0;
}

Vec3::Vec3(float x, float y, float z)
{
    Array[0] = x;
    Array[1] = y;
    Array[2] = z;
}

Vec3 Vec3::operator+(Vec3 V)
{
    return Vec3(Array[0]+V.Array[0], Array[1] + V.Array[1], Array[2] + V.Array[2]);
}

Vec3 Vec3::operator+(float bias)
{
    return Vec3(Array[0] + bias, Array[1] + bias, Array[2] + bias);
}

Vec3 Vec3::operator-(Vec3 V)
{
    return Vec3(Array[0] - V.Array[0], Array[1] - V.Array[1], Array[2] - V.Array[2]);
}

Vec3 Vec3::operator-(float bias)
{
    return Vec3(Array[0] - bias, Array[1] -bias, Array[2] - bias);
}

Vec3 Vec3::operator*(float s)
{
    return Vec3(Array[0] * s, Array[1] * s, Array[2] * s);
}

float Vec3::operator*(Vec3 V)
{
    return Array[0] * V.x()+ Array[1] * V.y()+ Array[2] * V.z();
}

Vec3 Vec3::Mul(Vec3 V)
{
    return Vec3(Array[0] * V.x() , Array[1] * V.y() , Array[2] * V.z());
}

Vec3 Vec3::Cross(Vec3 V)
{
    float a1 = Array[0];
    float b1 = Array[1];
    float c1 = Array[2];
    float a2 = V.Array[0];
    float b2 = V.Array[1];
    float c2 = V.Array[2];

    return Vec3(b1*c2 - b2 * c1, c1*a2 - a1 * c2, a1*b2 - a2 * b1);
}

Vec3 Vec3::normalize()
{
    float x = Array[0], y = Array[1], z = Array[2];
    float L = sqrt(x*x + y*y + z*z);
    if (L == 0.f) {
        return Vec3(0, 1, 0);
    }
    return Vec3(x / L, y / L, z / L);
}

float Vec3::lenth()
{
    return sqrt(Array[0]* Array[0]+ Array[1] * Array[1]+ Array[2] * Array[2]);
}

float Vec3::x()
{
    return Array[0];
}

float Vec3::y()
{
    return  Array[1];
}

float Vec3::z()
{
    return  Array[2];
}

void Vec3::setX(float x)
{
    Array[0] = x;
}

void Vec3::setY(float y)
{
    Array[1] = y;
}
void Vec3::setZ(float z)
{
    Array[2] = z;
}
