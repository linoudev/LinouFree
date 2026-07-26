#pragma once
#include "sdk/sdk.h"
#include "../imgui/imgui.h"
#include <algorithm>
#include <cmath>
#include <string>

inline bool IsBot(uintptr_t playerState) {
    if (!playerState) return false;
    uint8_t flags = memory.read<uint8_t>(playerState + offsets::bIsABot);
    return (flags & (1 << 3)) != 0;
}

inline std::string GetPlatformRaw(uintptr_t playerState) {
    if (!playerState) return "";

    uintptr_t strPtr = memory.read<uintptr_t>(playerState + offsets::Platform);
    int strLen = memory.read<int>(playerState + offsets::Platform + 8);

    if (!strPtr || strLen <= 0 || strLen > 50) return "";

    char buffer[64] = {0};
    for (int i = 0; i < strLen && i < 63; i++) {
        buffer[i] = memory.read<char>(strPtr + i * 2);
    }
    return std::string(buffer);
}

inline std::string GetPlatformName(const std::string& platform) {
    if (platform.find("WIN") != std::string::npos) return "Windows";
    if (platform.find("XBL") != std::string::npos || platform.find("XSX") != std::string::npos || platform.find("XB1") != std::string::npos) return "Xbox";
    if (platform.find("PSN") != std::string::npos || platform.find("PS5") != std::string::npos || platform.find("PS4") != std::string::npos) return "PlayStation";
    if (platform.find("SWT") != std::string::npos) return "Nintendo Switch";
    if (platform.find("AND") != std::string::npos) return "Android";
    if (platform.find("IOS") != std::string::npos) return "iOS";
    return platform.empty() ? "" : platform;
}

inline ImU32 GetPlatformColor(const std::string& platform) {
    if (platform.find("WIN") != std::string::npos) return IM_COL32(125, 165, 170, 255);
    if (platform.find("XBL") != std::string::npos || platform.find("XSX") != std::string::npos || platform.find("XB1") != std::string::npos) return IM_COL32(138, 154, 91, 255);
    if (platform.find("PSN") != std::string::npos || platform.find("PS5") != std::string::npos || platform.find("PS4") != std::string::npos) return IM_COL32(100, 120, 220, 255);
    if (platform.find("SWT") != std::string::npos) return IM_COL32(230, 50, 50, 255);
    if (platform.find("AND") != std::string::npos) return IM_COL32(115, 135, 83, 255);
    if (platform.find("IOS") != std::string::npos) return IM_COL32(144, 154, 251, 255);
    return IM_COL32(180, 180, 180, 255);
}

inline void DrawTextOutlined(const char* text, float x, float y, ImU32 color) {
    auto draw = ImGui::GetBackgroundDrawList();
    ImU32 shadow = IM_COL32(0, 0, 0, 255);

    draw->AddText(ImVec2(x - 1, y), shadow, text);
    draw->AddText(ImVec2(x + 1, y), shadow, text);
    draw->AddText(ImVec2(x, y - 1), shadow, text);
    draw->AddText(ImVec2(x, y + 1), shadow, text);
    draw->AddText(ImVec2(x - 1, y - 1), shadow, text);
    draw->AddText(ImVec2(x + 1, y - 1), shadow, text);
    draw->AddText(ImVec2(x - 1, y + 1), shadow, text);
    draw->AddText(ImVec2(x + 1, y + 1), shadow, text);

    draw->AddText(ImVec2(x, y), color, text);
}

inline void DrawBox(float x, float y, float w, float h, ImU32 color) {
    auto draw = ImGui::GetBackgroundDrawList();

    draw->AddRect(ImVec2(x - 1, y - 1), ImVec2(x + w + 1, y + h + 1), IM_COL32(0, 0, 0, 255), 0, 0, 3.0f);

    draw->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0, 2.0f);
}

inline void DrawFovCircle() {
    if (!drawFovCircle) return;

    auto draw = ImGui::GetBackgroundDrawList();
    float centerX = Width / 2.0f;
    float centerY = Height / 2.0f;

    draw->AddCircle(ImVec2(centerX, centerY), aimbotFov, IM_COL32(255, 255, 255, 100), 64, 1.5f);
}

namespace Bones {
    constexpr int Head = 68;
    constexpr int Neck = 66;
    constexpr int Pelvis = 2;

    constexpr int UpperarmL = 9;
    constexpr int LowerarmL = 10;
    constexpr int HandL = 11;

    constexpr int UpperarmR = 38;
    constexpr int LowerarmR = 39;
    constexpr int HandR = 40;

    constexpr int ThighL = 71;
    constexpr int CalfL = 72;
    constexpr int FootL = 86;

    constexpr int ThighR = 78;
    constexpr int CalfR = 79;
    constexpr int FootR = 87;
}

inline void DrawBoneLine(uintptr_t mesh, int bone1, int bone2, ImU32 color) {
    Vector3 pos1 = GetBoneWithRotation(mesh, bone1);
    Vector3 pos2 = GetBoneWithRotation(mesh, bone2);

    Vector2 screen1 = ProjectWorldToScreen(pos1);
    Vector2 screen2 = ProjectWorldToScreen(pos2);

    if ((screen1.x == 0 && screen1.y == 0) || (screen2.x == 0 && screen2.y == 0)) return;

    auto draw = ImGui::GetBackgroundDrawList();
    draw->AddLine(ImVec2(static_cast<float>(screen1.x), static_cast<float>(screen1.y)),
                  ImVec2(static_cast<float>(screen2.x), static_cast<float>(screen2.y)),
                  color, 1.5f);
}

inline void DrawSkeleton(uintptr_t mesh, ImU32 color) {

    DrawBoneLine(mesh, Bones::Head, Bones::Neck, color);
    DrawBoneLine(mesh, Bones::Neck, Bones::Pelvis, color);

    DrawBoneLine(mesh, Bones::Neck, Bones::UpperarmL, color);
    DrawBoneLine(mesh, Bones::UpperarmL, Bones::LowerarmL, color);
    DrawBoneLine(mesh, Bones::LowerarmL, Bones::HandL, color);

    DrawBoneLine(mesh, Bones::Neck, Bones::UpperarmR, color);
    DrawBoneLine(mesh, Bones::UpperarmR, Bones::LowerarmR, color);
    DrawBoneLine(mesh, Bones::LowerarmR, Bones::HandR, color);

    DrawBoneLine(mesh, Bones::Pelvis, Bones::ThighL, color);
    DrawBoneLine(mesh, Bones::ThighL, Bones::CalfL, color);
    DrawBoneLine(mesh, Bones::CalfL, Bones::FootL, color);

    DrawBoneLine(mesh, Bones::Pelvis, Bones::ThighR, color);
    DrawBoneLine(mesh, Bones::ThighR, Bones::CalfR, color);
    DrawBoneLine(mesh, Bones::CalfR, Bones::FootR, color);
}

struct FRotator {
    double pitch = 0.0;
    double yaw = 0.0;
    double roll = 0.0;

    FRotator() = default;
    FRotator(double p, double y, double r) : pitch(p), yaw(y), roll(r) {}

    FRotator operator-(const FRotator& other) const {
        return FRotator(pitch - other.pitch, yaw - other.yaw, roll - other.roll);
    }

    FRotator& operator-=(const FRotator& other) {
        pitch -= other.pitch;
        yaw -= other.yaw;
        roll -= other.roll;
        return *this;
    }

    double clampAxis(double angle) const {

        angle = fmod(angle, 360.0);
        if (angle < 0.0) angle += 360.0;
        return angle;
    }

    double normalizeAxis(double angle) const {
        angle = clampAxis(angle);
        if (angle > 180.0) angle -= 360.0;
        return angle;
    }

    void normalize() {
        pitch = normalizeAxis(pitch);
        yaw = normalizeAxis(yaw);
        roll = normalizeAxis(roll);
    }
};

inline FRotator FindLookAtRotation(Vector3 start, Vector3 target) {
    Vector3 direction = target - start;

    double length = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    if (length > 0.0001) {
        direction.x /= length;
        direction.y /= length;
        direction.z /= length;
    }

    constexpr double PI = 3.14159265358979323846;

    double yaw = atan2(direction.y, direction.x) * (180.0 / PI);
    double pitch = atan2(direction.z, sqrt(direction.x * direction.x + direction.y * direction.y)) * (180.0 / PI);

    return FRotator(pitch, yaw, 0.0);
}

inline FRotator GetCameraRotationForAim() {
    return FRotator(vCamera.Rotation.x, vCamera.Rotation.y, vCamera.Rotation.z);
}

struct AimTarget {
    bool valid = false;
    Vector2 screenPos;
    Vector3 worldPos;
    float distance;
    uintptr_t mesh;
    uintptr_t pawn;
    int playerId;
};

inline AimTarget currentTarget = {};
inline AimTarget lockedTarget = {};
inline bool isTargetLocked = false;

inline void Aimbot() {
    if (!aimbotEnabled) return;
    if (!LocalPtrs::PlayerController) return;

    bool aimKeyPressed = (GetAsyncKeyState(aimbotKey) & 0x8000) != 0;
    static bool wasAimKeyPressed = false;
    bool aimKeyJustPressed = aimKeyPressed && !wasAimKeyPressed;
    bool aimKeyJustReleased = !aimKeyPressed && wasAimKeyPressed;
    wasAimKeyPressed = aimKeyPressed;

    if (aimbotLockTarget && aimKeyJustReleased) {
        isTargetLocked = false;
        lockedTarget.valid = false;
    }

    AimTarget* targetToUse = nullptr;

    if (aimbotTargetPlayerId >= 0 && aimbotLockTarget && isTargetLocked && lockedTarget.valid) {
        if (lockedTarget.pawn && !is_dead(lockedTarget.pawn)) {
            targetToUse = &lockedTarget;
        } else {
            isTargetLocked = false;
            lockedTarget.valid = false;
        }
    } else if (aimbotLockTarget && isTargetLocked && lockedTarget.valid) {

        if (lockedTarget.pawn && !is_dead(lockedTarget.pawn)) {
            targetToUse = &lockedTarget;
        } else {

            isTargetLocked = false;
            lockedTarget.valid = false;
        }
    } else if (currentTarget.valid && !(aimbotTargetPlayerId >= 0 && aimbotLockTarget)) {

        targetToUse = &currentTarget;

        if (aimbotLockTarget && aimKeyJustPressed && aimKeyPressed) {
            lockedTarget = currentTarget;
            isTargetLocked = true;
            targetToUse = &lockedTarget;
        }
    }

    if (aimbotTargetPlayerId >= 0 && aimbotLockTarget && aimKeyJustPressed && aimKeyPressed) {

        isTargetLocked = true;
    }

    if (!targetToUse || !targetToUse->valid) return;
    if (!aimKeyPressed) return;

    if (aimbotVisibleOnly && !IsVisible(targetToUse->mesh)) return;

    Vector3 dir = targetToUse->worldPos - vCamera.Location;
    double len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
    if (len < 1.0) return;

    constexpr double PI = 3.14159265358979323846;
    double targetYaw = atan2(dir.y, dir.x) * (180.0 / PI);
    double targetPitch = atan2(dir.z, sqrt(dir.x*dir.x + dir.y*dir.y)) * (180.0 / PI);

    double deltaPitch = targetPitch - vCamera.Rotation.x;
    double deltaYaw = targetYaw - vCamera.Rotation.y;

    while (deltaPitch > 180.0) deltaPitch -= 360.0;
    while (deltaPitch < -180.0) deltaPitch += 360.0;
    while (deltaYaw > 180.0) deltaYaw -= 360.0;
    while (deltaYaw < -180.0) deltaYaw += 360.0;

    if (aimbotSmooth > 1.0) {
        deltaPitch /= aimbotSmooth;
        deltaYaw /= aimbotSmooth;
    }

    if (!std::isfinite(deltaPitch) || !std::isfinite(deltaYaw)) return;

    deltaPitch = std::clamp(deltaPitch, -89.0, 89.0);
    deltaYaw = std::clamp(deltaYaw, -180.0, 180.0);

    if (fabs(deltaPitch) < 0.01 && fabs(deltaYaw) < 0.01) return;

    if (silentAim) {

        uintptr_t cameraMgr = memory.read<uintptr_t>(LocalPtrs::PlayerController + offsets::CameraManager);
        if (cameraMgr) {
            uintptr_t weapon = memory.read<uintptr_t>(LocalPtrs::Player + offsets::CurrentWeapon);
            float origPitch = memory.read<float>(cameraMgr + offsets::ViewPitch);
            float origYaw = memory.read<float>(cameraMgr + offsets::ViewYaw);
            float origAimMin = 0.0f, origAimMax = 0.0f;
            if (weapon) {
                origAimMin = memory.read<float>(weapon + offsets::AimPitchMin);
                origAimMax = memory.read<float>(weapon + offsets::AimPitchMax);
                memory.write<float>(weapon + offsets::AimPitchMin, static_cast<float>(targetPitch));
                memory.write<float>(weapon + offsets::AimPitchMax, static_cast<float>(targetPitch));
            }
            memory.write<float>(cameraMgr + offsets::ViewPitch, static_cast<float>(targetPitch));
            memory.write<float>(cameraMgr + offsets::ViewYaw, static_cast<float>(targetYaw));
            Sleep(1);
            memory.write<float>(cameraMgr + offsets::ViewPitch, origPitch);
            memory.write<float>(cameraMgr + offsets::ViewYaw, origYaw);
            if (weapon) {
                memory.write<float>(weapon + offsets::AimPitchMin, origAimMin);
                memory.write<float>(weapon + offsets::AimPitchMax, origAimMax);
            }
        }
    } else {

        struct { double pitch, yaw, roll; } aimWrite = { deltaPitch, deltaYaw, 0.0 };
        memory.write<decltype(aimWrite)>(LocalPtrs::PlayerController + offsets::NetConnection + sizeof(uintptr_t), aimWrite);
    }
}

inline void ExploitsTick() {
    if (!LocalPtrs::PlayerController) return;
    uintptr_t pc = LocalPtrs::PlayerController;

    if (fovChanger) {
        memory.write<float>(pc + offsets::FOVMinimum, fovValue);
        memory.write<float>(pc + offsets::FOVMaximum, fovValue);
    } else {

        memory.write<float>(pc + offsets::FOVMinimum, 80.0f);
        memory.write<float>(pc + offsets::FOVMaximum, 80.0f);
    }

    if (noSpread) {
        memory.write<float>(pc + offsets::WeaponSpread, -1.0f);
    }

    if (!LocalPtrs::Player) return;
    uintptr_t pawn = LocalPtrs::Player;

    uintptr_t mesh = memory.read<uintptr_t>(pawn + offsets::Mesh);
    uintptr_t weapon = memory.read<uintptr_t>(pawn + offsets::CurrentWeapon);

    if (instaReload && mesh && weapon) {
        bool reloading = memory.read<uint8_t>(weapon + offsets::bIsReloading) & 1;
        static bool boostedReload = false;
        if (reloading) {
            if (!boostedReload) {
                memory.write<float>(mesh + offsets::PlayRate, 100.0f);
                boostedReload = true;
            }
        } else {
            if (boostedReload) {
                memory.write<float>(mesh + offsets::PlayRate, 1.0f);
                boostedReload = false;
            }
        }
    }

    if (rapidFire && weapon) {
        struct fscalablefloat {
            float m_value;
            uintptr_t m_curve;
            struct { float m_min, m_max; uintptr_t m_curve_ptr; } m_curve_table;
        };
        struct ffortweaponrampingdata {
            bool m_b_is_ramping_weapon;
            fscalablefloat m_max_ramp_stacks;
            fscalablefloat m_ramp_fire_rate_to_add;
            fscalablefloat m_ramp_grace_duration;
        };
        auto data = memory.read<ffortweaponrampingdata>(weapon + offsets::WeaponRampingData);
        data.m_b_is_ramping_weapon = true;
        data.m_max_ramp_stacks.m_value = 1;
        data.m_ramp_grace_duration.m_value = 1;
        data.m_ramp_fire_rate_to_add.m_value = rapidFireValue;
        memory.write<ffortweaponrampingdata>(weapon + offsets::WeaponRampingData, data);
    }

    if (playerSize) {
        struct FVec3f { float x, y, z; };
        FVec3f scale = { playerSizeValue, playerSizeValue, playerSizeValue };
        FVec3f reset = { 1.0f, 1.0f, 1.0f };
        uintptr_t gameState = memory.read<uintptr_t>(LocalPtrs::Gworld + offsets::GameState);
        if (gameState) {
            uintptr_t playerArray = memory.read<uintptr_t>(gameState + offsets::PlayerArray);
            int playerCount = memory.read<int>(gameState + offsets::PlayerArray + 8);
            if (playerCount > 0 && playerCount <= 150) {
                for (int i = 0; i < playerCount; i++) {
                    uintptr_t ps = memory.read<uintptr_t>(playerArray + i * sizeof(uintptr_t));
                    if (!ps) continue;
                    uintptr_t otherPawn = memory.read<uintptr_t>(ps + offsets::PawnPrivate);
                    if (!otherPawn) continue;
                    uintptr_t capsule = memory.read<uintptr_t>(otherPawn + offsets::CapsuleComponent);
                    if (capsule) {
                        memory.write<FVec3f>(capsule + offsets::RelativeScale3D, scale);
                    }
                    uintptr_t rootComp = memory.read<uintptr_t>(otherPawn + offsets::RootComponent);
                    if (rootComp) {
                        memory.write<FVec3f>(rootComp + offsets::RelativeScale3D, reset);
                    }
                }
            }
        }
    }
}

namespace radar {
    inline void RotatePoint(Vector2* screen, Vector3 origin, bool rotationMode) {
        Vector3 delta = origin - vCamera.Location;
        double angle = atan2(delta.y, delta.x);

        if (rotationMode) {
            double playerRadians = vCamera.Rotation.y * 3.14159265358979323846 / 180.0;
            angle -= playerRadians;
        }

        double distance = sqrt(delta.x * delta.x + delta.y * delta.y) * 0.01;
        double scaledDistance = (distance / radarRange) * (radarSize / 2.0);
        if (scaledDistance > radarSize / 2.0) scaledDistance = radarSize / 2.0;

        double x, y;
        if (rotationMode) {
            x = scaledDistance * sin(angle);
            y = -scaledDistance * cos(angle);
        } else {
            x = scaledDistance * cos(angle);
            y = scaledDistance * sin(angle);
        }

        float radarCenterX = radarPositionX + (radarSize / 2.0f);
        float radarCenterY = radarPositionY + (radarSize / 2.0f);

        screen->x = radarCenterX + x;
        screen->y = radarCenterY + y;
    }

    inline void DrawGrid(ImDrawList* drawList, ImVec2 radarPos, float radarSize) {
        float gridSpacing = radarSize / radarGridDivisions;
        ImU32 gridColor = IM_COL32(255, 255, 255, 30);

        for (int i = 1; i < radarGridDivisions; i++) {
            float x = radarPos.x + (i * gridSpacing);
            drawList->AddLine(ImVec2(x, radarPos.y), ImVec2(x, radarPos.y + radarSize), gridColor, 0.5f);
        }

        for (int i = 1; i < radarGridDivisions; i++) {
            float y = radarPos.y + (i * gridSpacing);
            drawList->AddLine(ImVec2(radarPos.x, y), ImVec2(radarPos.x + radarSize, y), gridColor, 0.5f);
        }
    }

    inline void DrawBackground() {
        if (!radarEnabled) return;

        auto drawList = ImGui::GetForegroundDrawList();
        ImVec2 radarPos(static_cast<float>(radarPositionX), static_cast<float>(radarPositionY));
        float radarSizeF = (float)radarSize;

        ImU32 bgColor = IM_COL32(0, 0, 0, (int)(radarOpacity * 0.5f));
        drawList->AddRectFilled(radarPos, ImVec2(radarPos.x + radarSizeF, radarPos.y + radarSizeF), bgColor);

        drawList->AddRect(radarPos, ImVec2(radarPos.x + radarSizeF, radarPos.y + radarSizeF), IM_COL32(5, 5, 5, 255), 0.0f, 0, 1.0f);

        float radarCenterX = radarPos.x + radarSizeF / 2.0f;
        float radarCenterY = radarPos.y + radarSizeF / 2.0f;
        ImVec2 radarCenter(radarCenterX, radarCenterY);

        drawList->AddLine(ImVec2(radarPos.x, radarPos.y - 2.0f), ImVec2(radarPos.x + radarSizeF, radarPos.y - 2.0f), IM_COL32(144, 154, 251, 255), 2.0f);

        if (radarGrid) {
            DrawGrid(drawList, radarPos, radarSizeF);
        } else {
            ImU32 lineColor = IM_COL32(5, 5, 5, 255);
            drawList->AddLine(ImVec2(radarPos.x, radarCenterY), ImVec2(radarPos.x + radarSizeF, radarCenterY), lineColor, 1.0f);
            drawList->AddLine(ImVec2(radarCenterX, radarPos.y), ImVec2(radarCenterX, radarPos.y + radarSizeF), lineColor, 1.0f);
        }

        float fov = vCamera.FieldOfView;
        float halfFovRadians = (fov / 2.0f) * 3.14159265358979323846f / 180.0f;
        float distanceToTop = radarCenterY - radarPos.y;
        float xOffset = distanceToTop * tanf(halfFovRadians);

        ImVec2 leftEnd(radarCenterX - xOffset, radarPos.y);
        ImVec2 rightEnd(radarCenterX + xOffset, radarPos.y);
        leftEnd.x = std::clamp(leftEnd.x, radarPos.x, radarPos.x + radarSizeF);
        rightEnd.x = std::clamp(rightEnd.x, radarPos.x, radarPos.x + radarSizeF);

        drawList->AddLine(radarCenter, leftEnd, IM_COL32(239, 187, 255, 255), 1.5f);
        drawList->AddLine(radarCenter, rightEnd, IM_COL32(239, 187, 255, 255), 1.5f);

        drawList->AddCircleFilled(radarCenter, 3.0f, IM_COL32(195, 177, 255, 255), 15);
    }

    inline void RenderPlayer(Vector3 playerPos, float distance, ImU32 color, bool isBot) {
        if (!radarEnabled) return;

        Vector2 radarScreen;
        RotatePoint(&radarScreen, playerPos, radarRotationMode);

        auto drawList = ImGui::GetForegroundDrawList();
        drawList->AddCircleFilled(ImVec2(static_cast<float>(radarScreen.x), static_cast<float>(radarScreen.y)), 3.0f, color, 15);

        if (radarDistance) {
            char distText[32];
            snprintf(distText, sizeof(distText), "(%.0fm)", distance);
            ImVec2 textSize = ImGui::CalcTextSize(distText);
            ImVec2 textPos(static_cast<float>(radarScreen.x - textSize.x / 2.0f), static_cast<float>(radarScreen.y + 2.0f));

            ImU32 outlineColor = IM_COL32(0, 0, 0, 255);
            for (float dx = -1.0f; dx <= 1.0f; dx++) {
                for (float dy = -1.0f; dy <= 1.0f; dy++) {
                    if (dx == 0.0f && dy == 0.0f) continue;
                    drawList->AddText(ImVec2(textPos.x + dx, textPos.y + dy), outlineColor, distText);
                }
            }
            drawList->AddText(textPos, color, distText);
        }
    }
}

inline void ActorLoop() {
    uintptr_t gWorld = decrypt_world(memory.read<uint64_t>(memory.BaseAddress + offsets::UWorld));
    uintptr_t gameInstance = memory.read<uint64_t>(gWorld + offsets::OwningGameInstance);
    uintptr_t localPlayersArray = memory.read<uint64_t>(gameInstance + offsets::LocalPlayers);
    uintptr_t localPlayer = memory.read<uint64_t>(localPlayersArray);
    uintptr_t playerController = memory.read<uint64_t>(localPlayer + offsets::PlayerController);
    uintptr_t localPawn = memory.read<uint64_t>(playerController + offsets::AcknowledgedPawn);

    LocalPtrs::Gworld = gWorld;
    LocalPtrs::PlayerController = playerController;
    LocalPtrs::Player = localPawn;

    DrawFovCircle();

    if (!(aimbotLockTarget && isTargetLocked)) {
        currentTarget.valid = false;
        currentTarget.distance = aimbotFov;
    }

    float centerX = Width / 2.0f;
    float centerY = Height / 2.0f;

    uintptr_t gameState = memory.read<uint64_t>(gWorld + offsets::GameState);
    if (!gameState) {
        Aimbot();
        return;
    }

    uintptr_t playerArray = memory.read<uint64_t>(gameState + offsets::PlayerArray);
    int playerCount = memory.read<int>(gameState + offsets::PlayerArray + 8);

    if (playerCount <= 0 || playerCount > 150) {
        Aimbot();
        return;
    }

    vCamera = GetViewAngles();

    for (int i = 0; i < playerCount; i++) {
        uintptr_t playerState = memory.read<uintptr_t>(playerArray + i * sizeof(uintptr_t));
        if (!playerState) continue;

        uintptr_t pawn = memory.read<uint64_t>(playerState + offsets::PawnPrivate);
        if (!pawn) continue;

        if (localPawn && pawn == localPawn) continue;

        uintptr_t mesh = memory.read<uint64_t>(pawn + offsets::Mesh);
        if (!mesh) continue;
        if (is_dead(pawn)) continue;

        FTransform cachedComponentToWorld = memory.read<FTransform>(mesh + offsets::ComponentToWorld);
        Vector3 head = GetBoneWithRotation(mesh, 110, &cachedComponentToWorld);
        Vector3 root = GetBoneWithRotation(mesh, 0, &cachedComponentToWorld);

        Vector2 headScreen = ProjectWorldToScreen(head);
        Vector2 rootScreen = ProjectWorldToScreen(root);

        if (headScreen.x == 0 && headScreen.y == 0) continue;

        float height = static_cast<float>(rootScreen.y - headScreen.y);
        if (height < 2) continue;

        bool isLockedPlayer = false;
        if (aimbotLockTarget && isTargetLocked) {

            if (lockedTarget.valid && lockedTarget.pawn == pawn) {
                isLockedPlayer = true;
            }

            else if (aimbotTargetPlayerId >= 0 && i == aimbotTargetPlayerId) {
                isLockedPlayer = true;
            }

            else if (lockedTarget.valid && lockedTarget.playerId == i) {
                isLockedPlayer = true;
            }

            if (isLockedPlayer) {

                lockedTarget.valid = true;
                lockedTarget.worldPos = head;
                lockedTarget.screenPos = headScreen;
                lockedTarget.mesh = mesh;
                lockedTarget.pawn = pawn;
                lockedTarget.playerId = i;
            }
        }

        if (aimbotEnabled && !(aimbotLockTarget && isTargetLocked)) {

            bool shouldTarget = true;
            if (aimbotTargetPlayerId >= 0 && i != aimbotTargetPlayerId) {
                shouldTarget = false;
            }

            if (shouldTarget) {
                float aimDx = (float)headScreen.x - centerX;
                float aimDy = (float)headScreen.y - centerY;
                float distToCrosshair = sqrtf(aimDx * aimDx + aimDy * aimDy);

                if (distToCrosshair < currentTarget.distance) {
                    bool isVisible = IsVisible(mesh);
                    if (!aimbotVisibleOnly || isVisible) {
                        currentTarget.valid = true;
                        currentTarget.screenPos = headScreen;
                        currentTarget.worldPos = head;
                        currentTarget.distance = distToCrosshair;
                        currentTarget.mesh = mesh;
                        currentTarget.pawn = pawn;
                        currentTarget.playerId = i;
                    }
                }
            }
        }

        float dx = static_cast<float>(head.x - vCamera.Location.x);
        float dy = static_cast<float>(head.y - vCamera.Location.y);
        float dz = static_cast<float>(head.z - vCamera.Location.z);
        float distMeters = sqrtf(dx*dx + dy*dy + dz*dz) / 100.0f;

        if (distMeters > maxEspDistance) continue;

        bool isBot = IsBot(playerState);
        ImU32 color;
        if (aimbotVisibleOnly && IsVisible(mesh)) {
            color = IM_COL32(0, 255, 0, 255);
        } else {
            color = isBot ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
        }

        float headPadding = height * 0.12f;
        float boxTop = (float)headScreen.y - headPadding;
        float boxHeight = height + headPadding;

        float bWidth = boxHeight * 0.5f;
        float boxX = (float)headScreen.x - bWidth / 2;
        if (boxEsp) {
            DrawBox(boxX, boxTop, bWidth, boxHeight, color);
        }

        if (boneEsp && distMeters <= maxBoneDistance) {
            DrawSkeleton(mesh, color);
        }

        float textOffsetY = 4.0f;

        if (showPlayerId) {
            char playerLabel[32];
            snprintf(playerLabel, sizeof(playerLabel), "Player %d", i + 1);
            ImVec2 nameSize = ImGui::CalcTextSize(playerLabel);
            float nameX = (float)headScreen.x - nameSize.x / 2.0f;
            float nameY = boxTop - nameSize.y - textOffsetY;
            DrawTextOutlined(playerLabel, nameX, nameY, IM_COL32(255, 255, 255, 255));
            textOffsetY += nameSize.y + 2.0f;
        }

        if (showPlatform && !isBot) {
            std::string platformRaw = GetPlatformRaw(playerState);
            std::string platformName = GetPlatformName(platformRaw);

            if (!platformName.empty()) {
                ImVec2 textSize = ImGui::CalcTextSize(platformName.c_str());
                float textX = (float)headScreen.x - textSize.x / 2.0f;
                float textY = boxTop - textSize.y - textOffsetY;

                ImU32 platformColor = GetPlatformColor(platformRaw);
                DrawTextOutlined(platformName.c_str(), textX, textY, platformColor);
                textOffsetY += textSize.y + 2.0f;
            }
        }

        if (showDistance) {
            char distText[32];
            snprintf(distText, sizeof(distText), "%.0fm", distMeters);

            ImVec2 textSize = ImGui::CalcTextSize(distText);
            float textX = (float)headScreen.x - textSize.x / 2.0f;
            float textY = boxTop - textSize.y - textOffsetY;

            DrawTextOutlined(distText, textX, textY, IM_COL32(255, 255, 255, 255));
        }

        if (snaplines && IsInFrontOfCamera(root)) {
            auto draw = ImGui::GetBackgroundDrawList();
            float screenBottomX = static_cast<float>(Width) / 2.0f;
            float screenBottomY = (float)Height;

            draw->AddLine(
                ImVec2(screenBottomX, screenBottomY),
                ImVec2((float)rootScreen.x, (float)rootScreen.y),
                color,
                1.5f
            );
        }

        if (radarEnabled) {
            radar::RenderPlayer(root, distMeters, color, isBot);
        }
    }

    Aimbot();

    ExploitsTick();
}
