#include "Vec3.h"
#include <math.h>

Vec3::Vec3()
{
    x = 0;
    y = 0;
    z = 0;
}

Vec3::Vec3(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vec3 Vec3::operator+(Vec3 V)
{
    return Vec3(x+V.x, y + V.y, z + V.z);
}

Vec3 Vec3::operator+(float bias)
{
    return Vec3(x + bias, y + bias, z + bias);
}

Vec3 Vec3::operator-(Vec3 V)
{
    return Vec3(x - V.x, y - V.y, z - V.z);
}

Vec3 Vec3::operator-(float bias)
{
    return Vec3(x - bias, y -bias, z - bias);
}

Vec3 Vec3::operator*(float s)
{
    return Vec3(x * s, y * s, z * s);
}

float Vec3::operator*(Vec3 V)
{
    return x * V.x+ y * V.y+ z * V.z;
}

Vec3 Vec3::Mul(Vec3 V)
{
    return Vec3(x * V.x , y * V.y , z * V.z);
}

Vec3 Vec3::Cross(Vec3 V)
{
    float a1 = x;
    float b1 = y;
    float c1 = z;
    float a2 = V.x;
    float b2 = V.y;
    float c2 = V.z;

    return Vec3(b1*c2 - b2 * c1, c1*a2 - a1 * c2, a1*b2 - a2 * b1);
}

Vec3 Vec3::normalize()
{
    float L = sqrt(x*x + y*y + z*z);
    if (L == 0.f) {
        return Vec3(0, 1, 0);
    }
    return Vec3(x / L, y / L, z / L);
}

float Vec3::lenth()
{
    return sqrt(x* x+ y * y+ z * z);
}
