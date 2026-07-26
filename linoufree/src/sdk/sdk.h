#pragma once
#include "../util/math.h"
#include "offsets.h"
#include "../memory/driver.h"
#include <bit>
#include <numbers>

namespace LocalPtrs {
    inline uintptr_t Player;
    inline uintptr_t PlayerController;
    inline uintptr_t Gworld;
}

inline Camera vCamera;

struct RotationData {
    double A;
    char pad1[24];
    double B;
    char pad2[424];
    double C;
};

inline Vector3 GetBoneWithRotation(DWORD_PTR mesh, int boneId, const FTransform* cachedComponentToWorld = nullptr) {
    if (!mesh) return Vector3();

    int32_t activeTransforms = memory.read<int32_t>(mesh + offsets::BoneArray + 0x48) * 0x10;
    auto boneArray = memory.read<tarray<FTransform>>(mesh + offsets::BoneArray + activeTransforms);
    
    auto boneTransform = memory.read<FTransform>((uintptr_t)boneArray.data + boneId * 0x60);
    auto componentToWorld = cachedComponentToWorld
        ? *cachedComponentToWorld
        : memory.read<FTransform>(mesh + offsets::ComponentToWorld);

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

inline bool is_dead(uintptr_t pawn) {
    return (memory.read<char>(pawn + offsets::bIsDying) >> 5) & 1;
}

inline Vector2 ProjectWorldToScreen(Vector3 worldPos) {
    auto MMatrix = Matrix(vCamera.Rotation);
    auto XAxis = Vector3(MMatrix.m[0][0], MMatrix.m[0][1], MMatrix.m[0][2]);
    auto YAxis = Vector3(MMatrix.m[1][0], MMatrix.m[1][1], MMatrix.m[1][2]);
    auto ZAxis = Vector3(MMatrix.m[2][0], MMatrix.m[2][1], MMatrix.m[2][2]);

    auto VDeltaCoordinates = worldPos - vCamera.Location;
    auto TTransform = Vector3(VDeltaCoordinates.Dot(YAxis), VDeltaCoordinates.Dot(ZAxis), VDeltaCoordinates.Dot(XAxis));

    TTransform.z = (TTransform.z < 1.f) ? 1.f : TTransform.z;
    auto FovRadians = tanf(vCamera.FieldOfView * std::numbers::pi_v<float> / 360.0f);

    return Vector2(Width / 2 + TTransform.x * ((Width / 2.0) / FovRadians) / TTransform.z, Height / 2 - TTransform.y * ((Width / 2.0) / FovRadians) / TTransform.z);
}

// Check if a world position is in front of the camera (not behind the player)
inline bool IsInFrontOfCamera(const Vector3& worldPos) {
    auto rotMatrix = Matrix(vCamera.Rotation);
    Vector3 axisX(rotMatrix.m[0][0], rotMatrix.m[0][1], rotMatrix.m[0][2]);

    Vector3 delta = worldPos - vCamera.Location;
    double forward = delta.Dot(axisX);

    return forward > 0.0f;
}

inline bool IsVisible(uintptr_t mesh) {
    if (!mesh || mesh < 0x10000) return false;

    uintptr_t proxy = memory.read<uintptr_t>(mesh + offsets::VisibilityProxy);
    if (!proxy || proxy < 0x10000) return false;

    float last_render = memory.read<float>(proxy + offsets::VisProxyLastRenderTime);
    float tolerance = memory.read<float>(mesh + offsets::VisTolerance);
    double last_submit = memory.read<double>(proxy + offsets::VisProxyLastSubmit);

    float adjusted_render = std::fmax(0.f, last_render + 0.0001f);
    double adjusted_submit = last_submit - static_cast<double>(tolerance);

    return adjusted_render >= adjusted_submit;
}
