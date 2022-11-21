#ifndef VEC3_H
#define VEC3_H


class Vec3
{
private:
    float Array[3];
public:
    Vec3();
    Vec3(float x, float y, float z);
    Vec3 operator+(Vec3 V);
    Vec3 operator+(float bias);
    Vec3 operator-(Vec3 V);
    Vec3 operator-(float bias);
    Vec3 operator*(float s);
    float operator*(Vec3 V);
    Vec3 Mul(Vec3 V);
    Vec3 Cross(Vec3 V);
    Vec3 normalize();
    float lenth();
    float x();
    float y();
    float z();
    void setX(float x);
    void setY(float y);
    void setZ(float z);
};

#endif // VEC3_H
