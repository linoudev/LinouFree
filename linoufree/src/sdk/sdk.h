#pragma once
#include "../util/math.h"
#include "offsets.h"
#include "../memory/driver.h"
#include <bit>
#include <numbers>
// basic sdk for reading game data, projecting world to screen, etc, also still shitcoded
namespace LocalPtrs {
    inline uintptr_t Player;
    inline uintptr_t PlayerController;
    inline uintptr_t Gworld;
}

inline Camera vCamera;

struct FrameProjection {
    Vector3 XAxis, YAxis, ZAxis;
    float FovRadians;
    float halfW, halfH;
    bool valid = false;
};
inline FrameProjection gProj;

inline float gWorldTime = 0.f;

struct RotationData {
    double A;
    char pad1[24];
    double B;
    char pad2[424];
    double C;
};

inline Vector3 GetBoneWithRotation(DWORD_PTR mesh, int boneId) {
    if (!mesh) return Vector3();

    int32_t activeTransforms = memory.read<int32_t>(mesh + offsets::BoneArray + 0x48) * 0x10;
    auto boneArray = memory.read<tarray<FTransform>>(mesh + offsets::BoneArray + activeTransforms);

    auto boneTransform = memory.read<FTransform>((uintptr_t)boneArray.data + boneId * 0x60);
    auto componentToWorld = memory.read<FTransform>(mesh + offsets::ComponentToWorld);

    D3DMATRIX matrix = MatrixMultiplication(
        boneTransform.ToMatrixWithScale().ToD3DMATRIX(),
        componentToWorld.ToMatrixWithScale().ToD3DMATRIX()
    );

    return Vector3(matrix._41, matrix._42, matrix._43);
}

inline Camera GetViewAngles() {
    auto locPtr = memory.read<uintptr_t>(LocalPtrs::Gworld + offsets::CameraLocation);
    auto rotPtr = memory.read<uintptr_t>(LocalPtrs::Gworld + offsets::CameraRotation);

    RotationData rot = memory.read<RotationData>(rotPtr);
    Vector3 loc = memory.read<Vector3>(locPtr);

    Vector3 angles;
    angles.x = asin(rot.C) * 180.0 / std::numbers::pi;
    angles.y = atan2(-rot.A, rot.B) * 180.0 / std::numbers::pi;
    angles.z = 0;

    float fov = memory.read<float>(LocalPtrs::PlayerController + offsets::CameraFOV) * 90.f;

    return { loc, angles, fov };
}

inline void UpdateFrameProjection() {
    auto MMatrix = Matrix(vCamera.Rotation);
    gProj.XAxis = Vector3(MMatrix.m[0][0], MMatrix.m[0][1], MMatrix.m[0][2]);
    gProj.YAxis = Vector3(MMatrix.m[1][0], MMatrix.m[1][1], MMatrix.m[1][2]);
    gProj.ZAxis = Vector3(MMatrix.m[2][0], MMatrix.m[2][1], MMatrix.m[2][2]);
    gProj.FovRadians = tanf(vCamera.FieldOfView * std::numbers::pi_v<float> / 360.0f);
    gProj.halfW = Width / 2.0f;
    gProj.halfH = Height / 2.0f;
    gProj.valid = true;
}

inline Vector2 ProjectWorldToScreen(Vector3 worldPos) {
    if (!gProj.valid) return Vector2(0, 0);

    auto delta = worldPos - vCamera.Location;
    float x = (float)(delta.Dot(gProj.YAxis));
    float y = (float)(delta.Dot(gProj.ZAxis));
    float z = (float)(delta.Dot(gProj.XAxis));

    if (z < 1.f) z = 1.f;

    float scale = gProj.halfW / (gProj.FovRadians * z);
    return Vector2(gProj.halfW + x * scale, gProj.halfH - y * scale);
}
// I AM CURRENTLY DYING PLEASE ITS SO HOT OUTSIDE
inline bool is_dead(uintptr_t pawn) {
    return (memory.read<char>(pawn + offsets::bIsDying) >> 5) & 1;
}

inline bool IsInFrontOfCamera(const Vector3& worldPos) {
    if (!gProj.valid) return false;
    auto delta = worldPos - vCamera.Location;
    return delta.Dot(gProj.XAxis) > 0.0f;
}

// this visible check is broken, dont try to use it + lastrendertime is a really bad method (dont work through player builds, etc)
inline bool IsVisible(uintptr_t mesh) {
    if (!mesh) return false;
    float lastRender = memory.read<float>(mesh + offsets::LastRenderTime);
    return (gWorldTime - lastRender) <= 0.06f;
}
