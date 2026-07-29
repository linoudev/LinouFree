#pragma once
#include "../sdk/sdk.h"
#include "../imgui/imgui.h"
#include "../util/settings.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>
// most unoptimized code + its not esp i added aimbot here cuz i was too lazy to make a new file 
struct PlayerCache {
    std::string platformRaw;
    std::string platformName;
    ImU32       platformColor = 0;
    bool        isBot         = false;
    uint64_t    lastUpdate    = 0;
};
inline std::unordered_map<uintptr_t, PlayerCache> g_playerCache;
inline uint64_t g_frameCount = 0;
constexpr uint64_t CACHE_TTL = 60;

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

    wchar_t wbuf[64] = {};
    driver.ReadPhysicalMemory((PVOID)strPtr, wbuf, (DWORD)(strLen * sizeof(wchar_t)));

    char buf[64] = {};
    for (int i = 0; i < strLen && i < 63; i++)
        buf[i] = (char)wbuf[i];

    return std::string(buf);
}

inline std::string GetPlatformName(const std::string& p) {
    if (p.find("WIN") != std::string::npos) return "PC";
    if (p.find("XBL") != std::string::npos || p.find("XSX") != std::string::npos || p.find("XB1") != std::string::npos) return "Xbox";
    if (p.find("PSN") != std::string::npos || p.find("PS5") != std::string::npos || p.find("PS4") != std::string::npos) return "PlayStation";
    if (p.find("SWT") != std::string::npos) return "Switch";
    if (p.find("AND") != std::string::npos) return "Android";
    if (p.find("IOS") != std::string::npos) return "iOS";
    return p.empty() ? "" : p;
}

inline ImU32 GetPlatformColor(const std::string& p) {
    if (p.find("WIN") != std::string::npos) return IM_COL32(125, 165, 170, 255);
    if (p.find("XBL") != std::string::npos || p.find("XSX") != std::string::npos || p.find("XB1") != std::string::npos) return IM_COL32(138, 154, 91,  255);
    if (p.find("PSN") != std::string::npos || p.find("PS5") != std::string::npos || p.find("PS4") != std::string::npos) return IM_COL32(100, 120, 220, 255);
    if (p.find("SWT") != std::string::npos) return IM_COL32(230,  50,  50, 255);
    if (p.find("AND") != std::string::npos) return IM_COL32(115, 135,  83, 255);
    if (p.find("IOS") != std::string::npos) return IM_COL32(144, 154, 251, 255);
    return IM_COL32(180, 180, 180, 255);
}

inline void RefreshPlayerCache(uintptr_t playerState) {
    auto& c = g_playerCache[playerState];
    if (g_frameCount - c.lastUpdate < CACHE_TTL) return;

    c.isBot          = IsBot(playerState);
    c.platformRaw    = GetPlatformRaw(playerState);
    c.platformName   = GetPlatformName(c.platformRaw);
    c.platformColor  = GetPlatformColor(c.platformRaw);
    c.lastUpdate     = g_frameCount;
}

inline void DrawTextOutlined(ImDrawList* draw, const char* text, float x, float y, ImU32 color) {
    ImU32 shadow = IM_COL32(0, 0, 0, 220);
    draw->AddText(ImVec2(x - 1, y - 1), shadow, text);
    draw->AddText(ImVec2(x + 1, y - 1), shadow, text);
    draw->AddText(ImVec2(x - 1, y + 1), shadow, text);
    draw->AddText(ImVec2(x + 1, y + 1), shadow, text);
    draw->AddText(ImVec2(x,     y    ), color,  text);
}

inline void DrawBox(ImDrawList* draw, float x, float y, float w, float h, ImU32 color) {
    draw->AddRect(ImVec2(x - 1, y - 1), ImVec2(x + w + 1, y + h + 1), IM_COL32(0,0,0,255), 0, 0, 3.0f);
    draw->AddRect(ImVec2(x,     y    ), ImVec2(x + w,     y + h    ), color,               0, 0, 2.0f);
}

inline void DrawFovCircle(ImDrawList* draw) {
    if (!drawFovCircle) return;
    draw->AddCircle(ImVec2(Width / 2.0f, Height / 2.0f), aimbotFov,
                    IM_COL32(255, 255, 255, 100), 48, 1.5f); 
}

namespace Bones {
    constexpr int Head      = 68;
    constexpr int Neck      = 66;
    constexpr int Pelvis    = 2;
    constexpr int UpperarmL = 9;
    constexpr int LowerarmL = 10;
    constexpr int HandL     = 11;
    constexpr int UpperarmR = 38;
    constexpr int LowerarmR = 39;
    constexpr int HandR     = 40;
    constexpr int ThighL    = 71;
    constexpr int CalfL     = 72;
    constexpr int FootL     = 86;
    constexpr int ThighR    = 78;
    constexpr int CalfR     = 79;
    constexpr int FootR     = 87;

    constexpr int All[] = {
        Head, Neck, Pelvis,
        UpperarmL, LowerarmL, HandL,
        UpperarmR, LowerarmR, HandR,
        ThighL, CalfL, FootL,
        ThighR, CalfR, FootR
    };
    constexpr int MaxId = 88;
}

inline void DrawBoneLineFromCache(ImDrawList* draw, const Vector2* s, int a, int b, ImU32 color) {
    if ((s[a].x == 0 && s[a].y == 0) || (s[b].x == 0 && s[b].y == 0)) return;
    draw->AddLine(ImVec2((float)s[a].x, (float)s[a].y),
                  ImVec2((float)s[b].x, (float)s[b].y), color, 1.5f);
}

inline void DrawSkeletonFromPositions(ImDrawList* draw, const Vector2* s, ImU32 color) {
    DrawBoneLineFromCache(draw, s, Bones::Head,      Bones::Neck,      color);
    DrawBoneLineFromCache(draw, s, Bones::Neck,      Bones::Pelvis,    color);
    DrawBoneLineFromCache(draw, s, Bones::Neck,      Bones::UpperarmL, color);
    DrawBoneLineFromCache(draw, s, Bones::UpperarmL, Bones::LowerarmL, color);
    DrawBoneLineFromCache(draw, s, Bones::LowerarmL, Bones::HandL,     color);
    DrawBoneLineFromCache(draw, s, Bones::Neck,      Bones::UpperarmR, color);
    DrawBoneLineFromCache(draw, s, Bones::UpperarmR, Bones::LowerarmR, color);
    DrawBoneLineFromCache(draw, s, Bones::LowerarmR, Bones::HandR,     color);
    DrawBoneLineFromCache(draw, s, Bones::Pelvis,    Bones::ThighL,    color);
    DrawBoneLineFromCache(draw, s, Bones::ThighL,    Bones::CalfL,     color);
    DrawBoneLineFromCache(draw, s, Bones::CalfL,     Bones::FootL,     color);
    DrawBoneLineFromCache(draw, s, Bones::Pelvis,    Bones::ThighR,    color);
    DrawBoneLineFromCache(draw, s, Bones::ThighR,    Bones::CalfR,     color);
    DrawBoneLineFromCache(draw, s, Bones::CalfR,     Bones::FootR,     color);
}

struct AimTarget {
    bool      valid    = false;
    Vector2   screenPos;
    Vector3   worldPos;
    float     distance = 0.f;
    uintptr_t mesh     = 0;
    uintptr_t pawn     = 0;
    int       playerId = -1;
};

inline AimTarget currentTarget = {};
inline AimTarget lockedTarget  = {};
inline AimTarget silentTarget  = {};
inline bool      isTargetLocked = false;

inline void Aimbot() {
    if (!aimbotEnabled) return;
    if (!LocalPtrs::PlayerController) return;

    if (exploitMasterSwitch && silentAimEnabled) {
        if (silentTarget.valid) {
            Vector3 dir = silentTarget.worldPos - vCamera.Location;
            double len = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
            if (len >= 1.0) {
                constexpr double PI = 3.14159265358979323846;
                double targetYaw = atan2(dir.y, dir.x) * (180.0 / PI);
                double targetPitch = atan2(dir.z, sqrt(dir.x * dir.x + dir.y * dir.y)) * (180.0 / PI);
                uintptr_t weapon = memory.read<uint64_t>(LocalPtrs::Player + offsets::CurrentWeapon);
                uintptr_t camMgr = memory.read<uintptr_t>(LocalPtrs::PlayerController + offsets::CameraManager);
                if (camMgr) {
                    memory.write<float>(camMgr + offsets::ViewPitch, (float)targetPitch);
                    memory.write<float>(camMgr + offsets::ViewYaw, (float)targetYaw);
                    if (weapon) {
                        memory.write<float>(weapon + offsets::AimPitchMin, (float)targetPitch);
                        memory.write<float>(weapon + offsets::AimPitchMax, (float)targetPitch);
                    }
                }
            }
        }
        silentTarget.valid = false;
        silentTarget.distance = silentAimFov;
        return;
    }

    bool aimKeyPressed = (GetAsyncKeyState(aimbotKey) & 0x8000) != 0;
    static bool wasAimKeyPressed = false;
    bool aimKeyJustPressed = aimKeyPressed && !wasAimKeyPressed;
    bool aimKeyJustReleased = !aimKeyPressed && wasAimKeyPressed;
    wasAimKeyPressed = aimKeyPressed;

    // Release lock if aim button is released (only if lock feature is enabled)
    if (aimbotLockTarget && aimKeyJustReleased) {
        isTargetLocked = false;
        lockedTarget.valid = false;
    }

    // Use locked target if available, otherwise use current target
    AimTarget* targetToUse = nullptr;

    // If targeting specific player ID, prioritize locked target
    if (aimbotTargetPlayerId >= 0 && aimbotLockTarget && isTargetLocked && lockedTarget.valid) {
        if (lockedTarget.pawn && !is_dead(lockedTarget.pawn)) {
            targetToUse = &lockedTarget;
        }
        else {
            isTargetLocked = false;
            lockedTarget.valid = false;
        }
    }
    else if (aimbotLockTarget && isTargetLocked && lockedTarget.valid) {
        // Check if locked target is still alive
        if (lockedTarget.pawn && !is_dead(lockedTarget.pawn)) {
            targetToUse = &lockedTarget;
        }
        else {
            // Target died, release lock
            isTargetLocked = false;
            lockedTarget.valid = false;
        }
    }
    else if (currentTarget.valid && !(aimbotTargetPlayerId >= 0 && aimbotLockTarget)) {
        // Only use currentTarget if not targeting specific ID
        targetToUse = &currentTarget;
        // Lock onto target when aim button is first pressed (only if lock feature is enabled)
        if (aimbotLockTarget && aimKeyJustPressed && aimKeyPressed) {
            lockedTarget = currentTarget;
            isTargetLocked = true;
            targetToUse = &lockedTarget;
        }
    }

    // If targeting specific player ID, lock onto that player when aim button is pressed
    if (aimbotTargetPlayerId >= 0 && aimbotLockTarget && aimKeyJustPressed && aimKeyPressed) {
        // The locked target will be set in ActorLoop when we find the matching player ID
        isTargetLocked = true;
    }

    if (!targetToUse || !targetToUse->valid) return;
    if (!aimKeyPressed) return;

    // Check visibility if required
    if (aimbotVisibleOnly && !IsVisible(targetToUse->mesh)) return;

    // Calculate look-at rotation from camera to target (uses current worldPos which is updated each frame)
    Vector3 dir = targetToUse->worldPos - vCamera.Location;
    double len = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    if (len < 1.0) return;

    constexpr double PI = 3.14159265358979323846;
    double targetYaw = atan2(dir.y, dir.x) * (180.0 / PI);
    double targetPitch = atan2(dir.z, sqrt(dir.x * dir.x + dir.y * dir.y)) * (180.0 / PI);

    // Subtract camera rotation to get delta
    double deltaPitch = targetPitch - vCamera.Rotation.x;
    double deltaYaw = targetYaw - vCamera.Rotation.y;

    // Normalize angles
    while (deltaPitch > 180.0) deltaPitch -= 360.0;
    while (deltaPitch < -180.0) deltaPitch += 360.0;
    while (deltaYaw > 180.0) deltaYaw -= 360.0;
    while (deltaYaw < -180.0) deltaYaw += 360.0;

    // Apply smoothing
    if (aimbotSmooth > 1.0) {
        deltaPitch /= aimbotSmooth;
        deltaYaw /= aimbotSmooth;
    }

    // Validate
    if (!std::isfinite(deltaPitch) || !std::isfinite(deltaYaw)) return;

    // Clamp
    deltaPitch = std::clamp(deltaPitch, -89.0, 89.0);
    deltaYaw = std::clamp(deltaYaw, -180.0, 180.0);

    // Skip if too small
    if (fabs(deltaPitch) < 0.01 && fabs(deltaYaw) < 0.01) return;

    struct { double pitch, yaw, roll; } aimWrite = { deltaPitch, deltaYaw, 0.0 };
    memory.write<decltype(aimWrite)>(LocalPtrs::PlayerController + offsets::NetConnection + sizeof(uintptr_t), aimWrite);
}
namespace radar {
    inline void RotatePoint(Vector2& out, const Vector3& origin) {
        Vector3 delta = origin - vCamera.Location;
        double angle  = atan2(delta.y, delta.x);

        if (radarRotationMode) {
            double pr = vCamera.Rotation.y * 3.14159265358979323846 / 180.0;
            angle -= pr;
        }

        double dist   = sqrt(delta.x*delta.x + delta.y*delta.y) * 0.01;
        double scaled = (dist / radarRange) * (radarSize * 0.5);
        if (scaled > radarSize * 0.5) scaled = radarSize * 0.5;

        float cx = radarPositionX + radarSize * 0.5f;
        float cy = radarPositionY + radarSize * 0.5f;

        if (radarRotationMode) {
            out.x = cx + scaled * sin(angle);
            out.y = cy - scaled * cos(angle);
        } else {
            out.x = cx + scaled * cos(angle);
            out.y = cy + scaled * sin(angle);
        }
    }

    inline void DrawBackground(ImDrawList* dl) {
        if (!radarEnabled) return;

        ImVec2 rp((float)radarPositionX, (float)radarPositionY);
        float  rs = (float)radarSize;

        dl->AddRectFilled(rp, ImVec2(rp.x + rs, rp.y + rs),
                          IM_COL32(0, 0, 0, (int)(radarOpacity * 0.5f)));
        dl->AddRect(rp, ImVec2(rp.x + rs, rp.y + rs), IM_COL32(5,5,5,255), 0, 0, 1.0f);

        float cx = rp.x + rs * 0.5f;
        float cy = rp.y + rs * 0.5f;

        dl->AddLine(ImVec2(rp.x, rp.y - 2.f), ImVec2(rp.x + rs, rp.y - 2.f),
                    IM_COL32(144, 154, 251, 255), 2.f);

        if (radarGrid) {
            float gs  = rs / radarGridDivisions;
            ImU32 gc  = IM_COL32(255,255,255,30);
            for (int i = 1; i < radarGridDivisions; i++) {
                float gx = rp.x + i * gs;
                float gy = rp.y + i * gs;
                dl->AddLine(ImVec2(gx, rp.y),    ImVec2(gx, rp.y + rs), gc, 0.5f);
                dl->AddLine(ImVec2(rp.x, gy),    ImVec2(rp.x + rs, gy), gc, 0.5f);
            }
        } else {
            ImU32 lc = IM_COL32(5,5,5,255);
            dl->AddLine(ImVec2(rp.x, cy), ImVec2(rp.x + rs, cy), lc, 1.f);
            dl->AddLine(ImVec2(cx, rp.y), ImVec2(cx, rp.y + rs), lc, 1.f);
        }

        float fov     = vCamera.FieldOfView;
        float halfFov = (fov * 0.5f) * 3.14159265358979323846f / 180.f;
        float dist2top = cy - rp.y;
        float xOff    = dist2top * tanf(halfFov);
        float lx = std::clamp(cx - xOff, rp.x, rp.x + rs);
        float rx = std::clamp(cx + xOff, rp.x, rp.x + rs);
        ImU32 fc  = IM_COL32(239,187,255,255);
        dl->AddLine(ImVec2(cx, cy), ImVec2(lx, rp.y), fc, 1.5f);
        dl->AddLine(ImVec2(cx, cy), ImVec2(rx, rp.y), fc, 1.5f);

        dl->AddCircleFilled(ImVec2(cx, cy), 3.f, IM_COL32(195,177,255,255), 15);
    }

    inline void RenderPlayer(ImDrawList* dl, const Vector3& pos, float dist, ImU32 color) {
        if (!radarEnabled) return;

        Vector2 rs;
        RotatePoint(rs, pos);
        dl->AddCircleFilled(ImVec2((float)rs.x, (float)rs.y), 3.f, color, 12);

        if (radarDistance) {
            char buf[32];
            snprintf(buf, sizeof(buf), "(%.0fm)", dist);
            ImVec2 ts = ImGui::CalcTextSize(buf);
            DrawTextOutlined(dl, buf,
                             (float)rs.x - ts.x * 0.5f,
                             (float)rs.y + 2.f, color);
        }
    }
}

// ===== EXPLOITS =====
inline void RunLocalExploits(uintptr_t pc, uintptr_t localPawn) {
    if (!exploitMasterSwitch) return;

    uintptr_t weapon = memory.read<uint64_t>(localPawn + offsets::CurrentWeapon);
    uintptr_t mesh = memory.read<uint64_t>(localPawn + offsets::Mesh);

    if (exploitFovChanger) {
        memory.write<float>(pc + offsets::FOVMinimum, exploitFovValue);
        memory.write<float>(pc + offsets::FOVMaximum, exploitFovValue);
    } else {
        memory.write<float>(pc + offsets::FOVMinimum, 80.0f);
        memory.write<float>(pc + offsets::FOVMaximum, 80.0f);
    }

    if (exploitInstaReload && weapon && mesh && weapon >= 0x10000 && mesh >= 0x10000) {
        bool is_reloading = (memory.read<uint8_t>(weapon + offsets::bIsReloading) & 1);
        static bool boosted = false;
        if (is_reloading) {
            if (!boosted) {
                memory.write<float>(mesh + offsets::GlobalAnimRateScale, 100.0f);
                boosted = true;
            }
        } else {
            if (boosted) {
                memory.write<float>(mesh + offsets::GlobalAnimRateScale, 1.0f);
                boosted = false;
            }
        }
    }

    if (exploitNoSpread) {
        memory.write<float>(pc + offsets::WeaponSpread, -1.0f);
    }

    if (exploitRapidFire && weapon) {
        auto data = memory.read<FFortWeaponRampingData>(weapon + offsets::WeaponRampingData);
        data.bIsRamping = true;
        data.maxRampStacks.value = 1.0f;
        data.graceDuration.value = 1.0f;
        data.fireRateToAdd.value = exploitRapidFireValue;
        memory.write<FFortWeaponRampingData>(weapon + offsets::WeaponRampingData, data);
    }
}

inline void ActorLoop() {
    g_frameCount++;

    uintptr_t gWorld    = decrypt_world(memory.read<uint64_t>(memory.BaseAddress + offsets::UWorld));
    uintptr_t gameInst  = memory.read<uint64_t>(gWorld + offsets::OwningGameInstance);
    uintptr_t lpArr     = memory.read<uint64_t>(gameInst + offsets::LocalPlayers);
    uintptr_t lp        = memory.read<uint64_t>(lpArr);
    uintptr_t pc        = memory.read<uint64_t>(lp + offsets::PlayerController);
    uintptr_t localPawn = memory.read<uint64_t>(pc + offsets::AcknowledgedPawn);

    LocalPtrs::Gworld           = gWorld;
    LocalPtrs::PlayerController = pc;
    LocalPtrs::Player           = localPawn;

    if (localPawn)
        RunLocalExploits(pc, localPawn);

    vCamera = GetViewAngles();
    UpdateFrameProjection();

    {
        uintptr_t rotPtr = memory.read<uintptr_t>(gWorld + offsets::CameraRotation);
        gWorldTime = (float)memory.read<double>(rotPtr + 0x10);
    }

    ImDrawList* bgDraw = ImGui::GetBackgroundDrawList();
    ImDrawList* fgDraw = ImGui::GetForegroundDrawList();

    DrawFovCircle(bgDraw);

    if (!(aimbotLockTarget && isTargetLocked)) {
        currentTarget.valid    = false;
        currentTarget.distance = aimbotFov;
    }

    if (exploitMasterSwitch && silentAimEnabled) {
        silentTarget.valid = false;
        silentTarget.distance = silentAimFov;
    }

    float cx = gProj.halfW;
    float cy = gProj.halfH;

    uintptr_t gameState = memory.read<uint64_t>(gWorld + offsets::GameState);
    if (!gameState) { Aimbot(); return; }

    uintptr_t playerArray = memory.read<uint64_t>(gameState + offsets::PlayerArray);
    int       playerCount = memory.read<int>(gameState + offsets::PlayerArray + 8);
    if (playerCount <= 0 || playerCount > 150) { Aimbot(); return; }

    radar::DrawBackground(fgDraw);

    for (int i = 0; i < playerCount; i++) {
        uintptr_t playerState = memory.read<uintptr_t>(playerArray + i * sizeof(uintptr_t));
        if (!playerState) continue;

        uintptr_t pawn = memory.read<uint64_t>(playerState + offsets::PawnPrivate);
        if (!pawn || pawn == localPawn) continue;

        uintptr_t mesh = memory.read<uint64_t>(pawn + offsets::Mesh);
        if (!mesh) continue;
        if (is_dead(pawn)) continue;

        FTransform compToWorld = memory.read<FTransform>(mesh + offsets::ComponentToWorld);
        int32_t activeTransforms = memory.read<int32_t>(mesh + offsets::BoneArray + 0x48) * 0x10;
        auto boneArrayInfo = memory.read<tarray<FTransform>>(mesh + offsets::BoneArray + activeTransforms);
        if (!boneArrayInfo.data || boneArrayInfo.count <= 0) continue;

        constexpr int MAX_BONE = 112;
        int boneCount = min(boneArrayInfo.count, MAX_BONE);
        uint8_t boneData[MAX_BONE * sizeof(FTransform)];
        driver.ReadPhysicalMemory((PVOID)((uintptr_t)boneArrayInfo.data), boneData, boneCount * sizeof(FTransform));
        FTransform* allBones = reinterpret_cast<FTransform*>(boneData);
        D3DMATRIX compToWorldMat = compToWorld.ToMatrixWithScale().ToD3DMATRIX();

        auto getBonePos = [&](int boneId) -> Vector3 {
            D3DMATRIX m = MatrixMultiplication(
                allBones[boneId].ToMatrixWithScale().ToD3DMATRIX(),
                compToWorldMat
            );
            return Vector3(m._41, m._42, m._43);
        };

        Vector3 head = getBonePos(110);
        Vector3 root = getBonePos(0);

        Vector2 headScreen = ProjectWorldToScreen(head);
        Vector2 rootScreen = ProjectWorldToScreen(root);

        if (headScreen.x == 0 && headScreen.y == 0) continue;

        float height = (float)(rootScreen.y - headScreen.y);
        if (height < 2.f) continue;
        float dx = (float)(head.x - vCamera.Location.x);
        float dy = (float)(head.y - vCamera.Location.y);
        float dz = (float)(head.z - vCamera.Location.z);
        float distMeters = sqrtf(dx*dx + dy*dy + dz*dz) * 0.01f;
        if (distMeters > maxEspDistance) continue;

        RefreshPlayerCache(playerState);
        const PlayerCache& pc_cache = g_playerCache[playerState];
        bool isBot = pc_cache.isBot;

        bool visible = IsVisible(mesh); 

        ImU32 color;
        if (aimbotVisibleOnly && visible)
            color = IM_COL32(0, 255, 0, 255);
        else
            color = isBot ? IM_COL32(255, 220, 0, 255) : IM_COL32(255, 60, 60, 255);

        if (aimbotEnabled && !(aimbotLockTarget && isTargetLocked)) {
            bool shouldTarget = (aimbotTargetPlayerId < 0 || i == aimbotTargetPlayerId);
            if (shouldTarget) {
                float adx = headScreen.x - cx;
                float ady = headScreen.y - cy;
                float d2c = sqrtf(adx*adx + ady*ady);
                if (d2c < currentTarget.distance) {
                    if (!aimbotVisibleOnly || visible) {
                        currentTarget = { true, headScreen, head, d2c, mesh, pawn, i };
                    }
                }
            }
        }

        if (exploitMasterSwitch && silentAimEnabled) {
            bool shouldTarget = (aimbotTargetPlayerId < 0 || i == aimbotTargetPlayerId);
            if (shouldTarget) {
                float adx = headScreen.x - cx;
                float ady = headScreen.y - cy;
                float d2c = sqrtf(adx*adx + ady*ady);
                if (d2c < silentTarget.distance) {
                    if (!aimbotVisibleOnly || visible) {
                        silentTarget = { true, headScreen, head, d2c, mesh, pawn, i };
                    }
                }
            }
        }

        if (aimbotLockTarget && isTargetLocked && lockedTarget.valid) {
            if (lockedTarget.pawn == pawn ||
                (aimbotTargetPlayerId >= 0 && i == aimbotTargetPlayerId) ||
                lockedTarget.playerId == i) {
                lockedTarget.worldPos  = head;
                lockedTarget.screenPos = headScreen;
                lockedTarget.mesh      = mesh;
                lockedTarget.pawn      = pawn;
                lockedTarget.playerId  = i;
            }
        }
        float headPad  = height * 0.12f;
        float boxTop   = (float)headScreen.y - headPad;
        float boxH     = height + headPad;
        float boxW     = boxH * 0.5f;
        float boxX     = (float)headScreen.x - boxW * 0.5f;
        if (boxEsp) DrawBox(bgDraw, boxX, boxTop, boxW, boxH, color);

        if (boneEsp && distMeters <= maxBoneDistance) {
            Vector2 screenBones[Bones::MaxId] = {};
            for (int bid : Bones::All) {
                D3DMATRIX m = MatrixMultiplication(
                    allBones[bid].ToMatrixWithScale().ToD3DMATRIX(),
                    compToWorldMat
                );
                Vector3 wp(m._41, m._42, m._43);
                screenBones[bid] = ProjectWorldToScreen(wp);
            }
            DrawSkeletonFromPositions(bgDraw, screenBones, color);
        }

        float textOffY = 4.f;
        const ImVec2 fontSize = ImGui::CalcTextSize("A");

        if (showPlayerId) {
            char lbl[32];
            snprintf(lbl, sizeof(lbl), "Player %d", i + 1);
            ImVec2 ts = ImGui::CalcTextSize(lbl);
            DrawTextOutlined(bgDraw, lbl,
                             (float)headScreen.x - ts.x * 0.5f,
                             boxTop - ts.y - textOffY, IM_COL32(255,255,255,255));
            textOffY += ts.y + 2.f;
        }

        if (showPlatform && !isBot && !pc_cache.platformName.empty()) {
            ImVec2 ts = ImGui::CalcTextSize(pc_cache.platformName.c_str());
            DrawTextOutlined(bgDraw, pc_cache.platformName.c_str(),
                             (float)headScreen.x - ts.x * 0.5f,
                             boxTop - ts.y - textOffY, pc_cache.platformColor);
            textOffY += ts.y + 2.f;
        }

        if (showDistance) {
            char dist_buf[32];
            snprintf(dist_buf, sizeof(dist_buf), "%.0fm", distMeters);
            ImVec2 ts = ImGui::CalcTextSize(dist_buf);
            DrawTextOutlined(bgDraw, dist_buf,
                             (float)headScreen.x - ts.x * 0.5f,
                             boxTop - ts.y - textOffY, IM_COL32(255,255,255,255));
        }

                // ── Snaplines ──
        if (snaplines && IsInFrontOfCamera(root)) {
            bgDraw->AddLine(ImVec2(cx, (float)Height),
                            ImVec2((float)rootScreen.x, (float)rootScreen.y),
                            color, 1.5f);
        }

        // ── Radar dot ──
        radar::RenderPlayer(fgDraw, root, distMeters, color);
    }

    Aimbot();
}
