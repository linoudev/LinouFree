#pragma once
#include <cstdint>

// PASTERS FAVOURITE PART!! HELLO IF THIS IS THE ONLY FILE YOU LOOK AT, THAT MEAN YOU ARE A PROFESSIONAL PASTER!

namespace offsets {
    inline uintptr_t UWorld = 0x19DE3C10;
    inline uintptr_t UWorldXorKey = 0x148AEA3LL;
    inline uintptr_t OwningGameInstance = 0x248;
    inline uintptr_t GameState = 0x1D0;
    inline uintptr_t PlayerArray = 0x288;

    inline uintptr_t LocalPlayers = 0x38;
    inline uintptr_t PlayerController = 0x30;
    inline uintptr_t AcknowledgedPawn = 0x318;
    inline uintptr_t PlayerCameraManager = 0x328;
    inline uintptr_t ControlRotation = 0x2E8;
    inline uintptr_t PlayerAimOffset = 0x2218;

    inline uintptr_t PawnPrivate = 0x2E8;
    inline uintptr_t bIsABot = 0x27A;
    inline uintptr_t Platform = 0x400;
    inline uintptr_t PlayerName = 0x9C8;
    inline uintptr_t PlayerNamePrivate = 0x308;

    inline uintptr_t PlayerState = 0x290;
    inline uintptr_t TeamIndex = 0xF11;
    inline uintptr_t InitialSquadSize = 0x16B8;
    inline uintptr_t bIsDying = 0x720;
    inline uintptr_t bIsDBNO = 0x849;

    inline uintptr_t TargetSourceOffset = 0x1018;
    inline uintptr_t TargetSourceOffsetWhileTargeting = 0x1048;
    inline uintptr_t TargetSourceOffsetWhileCrouched = 0x1030;
    inline uintptr_t bTraceThroughWorld = 0xff9;

    inline uintptr_t Health = 0xCB4;
    inline uintptr_t MaxHealth = 0xCB8;

    inline uintptr_t RootComponent = 0x1B0;
    inline uintptr_t RelativeLocation = 0x140;
    inline uintptr_t RelativeRotation = 0x158;
    inline uintptr_t Mesh = 0x2F0;
    inline uintptr_t BoneArray = 0x650;
    inline uintptr_t BoneCache = 0x660;
    inline uintptr_t ComponentToWorld = 0x1E0;
    inline uintptr_t ComponentVelocity = 0x188;
    inline uintptr_t LastRenderTime = 0x330;
    inline uintptr_t OverlappingBuildings = 0x1C28;

    inline uintptr_t CameraLocation = 0x178;
    inline uintptr_t CameraRotation = CameraLocation + 0x10;
    inline uintptr_t CameraFOV = 0x374;

    inline uintptr_t CurrentVehicle = 0x2AE8;
    inline uintptr_t CurrentWeapon = 0x998;
    inline uintptr_t WeaponData = 0x630;
    inline uintptr_t WeaponOffsetCorrection = 0x2230;
    inline uintptr_t AdditionalAimOffset = 0x21D0;
    inline uintptr_t AmmoCount = 0x113C;

    inline uintptr_t ProjectileSpeed = 0x230C;
    inline uintptr_t ProjectileGravity = 0x2310;

    inline uintptr_t HabaneroComponent = 0x908;
    inline uintptr_t RankedProgress = 0xd8;
    inline uintptr_t KillScore = 0xEEC;
    inline uintptr_t RebootCount = 0x15AC;
    inline uintptr_t TargetedFortPawn = 0x1630;

    inline uintptr_t Spectators = 0xA70;
    inline uintptr_t SpectatorArray = 0x17F8;

    inline uintptr_t PersistentLevel = 0x38;
    inline uintptr_t Levels = 0x1E8;
    inline uintptr_t SeasonLevelUIDisplay = 0x0F2C;
    inline uintptr_t AActor = 0x38;

    inline uintptr_t Tier = 0xaa;
    inline uintptr_t ItemName = 0x38;
}

inline uint64_t decrypt_world(uint64_t v)
{
    return static_cast<uintptr_t>(
        _byteswap_uint64(v ^ 0x93F1FA5800000000ull)
        );
}