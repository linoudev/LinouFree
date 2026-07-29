#pragma once
#include <d3d9.h>
#include <cmath>
#include <numbers>
// i was the worst in my math class so enjoy the terrifying math <3

inline int Width = GetSystemMetrics(SM_CXSCREEN);
inline int Height = GetSystemMetrics(SM_CYSCREEN);

class Vector2 {
public:
    double x, y;
    Vector2() : x(0), y(0) {}
    Vector2(double _x, double _y) : x(_x), y(_y) {}
};

// 3d vector for world positions
class Vector3 {
public:
    double x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
    
    double Dot(const Vector3& v) const { 
        return x * v.x + y * v.y + z * v.z; 
    }
    
    Vector3 operator-(const Vector3& v) const { 
        return Vector3(x - v.x, y - v.y, z - v.z); 
    }

    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
};
struct FPlane : Vector3 { 
    double W = 0; 
};

struct FQuat { 
    double x, y, z, w; 
};

struct Camera { 
    Vector3 Location;
    Vector3 Rotation;
    float FieldOfView; 
};

class FMatrix {
public:
    double m[4][4];
    FPlane XPlane, YPlane, ZPlane, WPlane;

    D3DMATRIX ToD3DMATRIX() const {
        D3DMATRIX r;
        r._11 = static_cast<float>(XPlane.x); r._12 = static_cast<float>(XPlane.y); r._13 = static_cast<float>(XPlane.z); r._14 = static_cast<float>(XPlane.W);
        r._21 = static_cast<float>(YPlane.x); r._22 = static_cast<float>(YPlane.y); r._23 = static_cast<float>(YPlane.z); r._24 = static_cast<float>(YPlane.W);
        r._31 = static_cast<float>(ZPlane.x); r._32 = static_cast<float>(ZPlane.y); r._33 = static_cast<float>(ZPlane.z); r._34 = static_cast<float>(ZPlane.W);
        r._41 = static_cast<float>(WPlane.x); r._42 = static_cast<float>(WPlane.y); r._43 = static_cast<float>(WPlane.z); r._44 = static_cast<float>(WPlane.W);
        return r;
    }
};

// transform with rotation, translation, scale
struct FTransform {
    FQuat Rotation;
    Vector3 Translation;
    uint8_t pad1[8];
    Vector3 Scale3D;
    uint8_t pad2[8];

    FMatrix ToMatrixWithScale() const {
        Vector3 S(
            Scale3D.x ? Scale3D.x : 1,
            Scale3D.y ? Scale3D.y : 1,
            Scale3D.z ? Scale3D.z : 1
        );

        double x2 = Rotation.x * 2, y2 = Rotation.y * 2, z2 = Rotation.z * 2;
        double xx2 = Rotation.x * x2, yy2 = Rotation.y * y2, zz2 = Rotation.z * z2;
        double yz2 = Rotation.y * z2, wx2 = Rotation.w * x2;
        double xy2 = Rotation.x * y2, wz2 = Rotation.w * z2;
        double xz2 = Rotation.x * z2, wy2 = Rotation.w * y2;

        FMatrix M;
        M.WPlane.x = Translation.x;
        M.WPlane.y = Translation.y;
        M.WPlane.z = Translation.z;

        M.XPlane.x = (1 - (yy2 + zz2)) * S.x;
        M.YPlane.y = (1 - (xx2 + zz2)) * S.y;
        M.ZPlane.z = (1 - (xx2 + yy2)) * S.z;

        M.ZPlane.y = (yz2 - wx2) * S.z;
        M.YPlane.z = (yz2 + wx2) * S.y;
        M.YPlane.x = (xy2 - wz2) * S.y;
        M.XPlane.y = (xy2 + wz2) * S.x;
        M.ZPlane.x = (xz2 + wy2) * S.z;
        M.XPlane.z = (xz2 - wy2) * S.x;

        M.XPlane.W = 0;
        M.YPlane.W = 0;
        M.ZPlane.W = 0;
        M.WPlane.W = 1;

        return M;
    }
};

template<class T>
class tarray {
public:
    T* data = nullptr;
    int32_t count = 0;
    int32_t max = 0;
};


inline D3DMATRIX MatrixMultiplication(D3DMATRIX m1, D3DMATRIX m2) {
    D3DMATRIX o;
    o._11 = m1._11*m2._11 + m1._12*m2._21 + m1._13*m2._31 + m1._14*m2._41;
    o._12 = m1._11*m2._12 + m1._12*m2._22 + m1._13*m2._32 + m1._14*m2._42;
    o._13 = m1._11*m2._13 + m1._12*m2._23 + m1._13*m2._33 + m1._14*m2._43;
    o._14 = m1._11*m2._14 + m1._12*m2._24 + m1._13*m2._34 + m1._14*m2._44;
    o._21 = m1._21*m2._11 + m1._22*m2._21 + m1._23*m2._31 + m1._24*m2._41;
    o._22 = m1._21*m2._12 + m1._22*m2._22 + m1._23*m2._32 + m1._24*m2._42;
    o._23 = m1._21*m2._13 + m1._22*m2._23 + m1._23*m2._33 + m1._24*m2._43;
    o._24 = m1._21*m2._14 + m1._22*m2._24 + m1._23*m2._34 + m1._24*m2._44;
    o._31 = m1._31*m2._11 + m1._32*m2._21 + m1._33*m2._31 + m1._34*m2._41;
    o._32 = m1._31*m2._12 + m1._32*m2._22 + m1._33*m2._32 + m1._34*m2._42;
    o._33 = m1._31*m2._13 + m1._32*m2._23 + m1._33*m2._33 + m1._34*m2._43;
    o._34 = m1._31*m2._14 + m1._32*m2._24 + m1._33*m2._34 + m1._34*m2._44;
    o._41 = m1._41*m2._11 + m1._42*m2._21 + m1._43*m2._31 + m1._44*m2._41;
    o._42 = m1._41*m2._12 + m1._42*m2._22 + m1._43*m2._32 + m1._44*m2._42;
    o._43 = m1._41*m2._13 + m1._42*m2._23 + m1._43*m2._33 + m1._44*m2._43;
    o._44 = m1._41*m2._14 + m1._42*m2._24 + m1._43*m2._34 + m1._44*m2._44;
    return o;
}

inline D3DMATRIX Matrix(Vector3 rot) {
    float p = static_cast<float>(rot.x) * std::numbers::pi_v<float> / 180.0f;
    float y = static_cast<float>(rot.y) * std::numbers::pi_v<float> / 180.0f;
    float r = static_cast<float>(rot.z) * std::numbers::pi_v<float> / 180.0f;

    float SP = sinf(p), CP = cosf(p);
    float SY = sinf(y), CY = cosf(y);
    float SR = sinf(r), CR = cosf(r);

    D3DMATRIX m;
    m.m[0][0] = CP * CY;
    m.m[0][1] = CP * SY;
    m.m[0][2] = SP;
    m.m[0][3] = 0;

    m.m[1][0] = SR * SP * CY - CR * SY;
    m.m[1][1] = SR * SP * SY + CR * CY;
    m.m[1][2] = -SR * CP;
    m.m[1][3] = 0;

    m.m[2][0] = -(CR * SP * CY + SR * SY);
    m.m[2][1] = CY * SR - CR * SP * SY;
    m.m[2][2] = CR * CP;
    m.m[2][3] = 0;

    m.m[3][0] = 0;
    m.m[3][1] = 0;
    m.m[3][2] = 0;
    m.m[3][3] = 1;

    return m;
}

struct FScalableFloat {
    float value;
    uintptr_t curve;
    struct { float min, max; uintptr_t ptr; } table;
};

struct FFortWeaponRampingData {
    bool bIsRamping;
    char pad1[7];
    FScalableFloat maxRampStacks;
    FScalableFloat fireRateToAdd;
    FScalableFloat graceDuration;
};
