#pragma once
#include <cstdint>

namespace offsets {
    inline uintptr_t UWorld = 0x1B2439A0;
    inline uintptr_t UWorldXorKey = 0x012F546CULL;
    inline uintptr_t OwningGameInstance = 0x250;
    inline uintptr_t GameState = 0x1D8;
    inline uintptr_t PlayerArray = 0x288;

    inline uintptr_t LocalPlayers = 0x38;
    inline uintptr_t PlayerController = 0x30;
    inline uintptr_t AcknowledgedPawn = 0x318;
    inline uintptr_t PlayerCameraManager = 0x328;
    inline uintptr_t ControlRotation = 0x2E8;
    inline uintptr_t NetConnection = 0x4A8;
    inline uintptr_t LastRenderTime = 0x530;
    inline uintptr_t LastRenderTimeOnScreen = 0x32C;
    inline uintptr_t LastSubmitTime = 0x338;
    inline uintptr_t VisTolerance = 0x330;
    inline uintptr_t VisibilityProxy = 0xB0;
    inline uintptr_t VisProxyLastSubmit = 0x7B8;
    inline uintptr_t VisProxyLastRenderTime = 0x7DC;

    inline uintptr_t PawnPrivate = 0x2E8;
    inline uintptr_t bIsABot = 0x27A;
    inline uintptr_t Platform = 0x400;
    inline uintptr_t PlayerName = 0x9C8;
    inline uintptr_t PlayerNamePrivate = 0x308;

    inline uintptr_t PlayerState = 0x290;
    inline uintptr_t TeamIndex = 0xF31;
    inline uintptr_t InitialSquadSize = 0x16B8;
    inline uintptr_t bIsDying = 0x720;
    inline uintptr_t bIsDBNO = 0x849;

    inline uintptr_t TargetSourceOffset = 0x1018;
    inline uintptr_t TargetSourceOffsetWhileTargeting = 0x1048;
    inline uintptr_t TargetSourceOffsetWhileCrouched = 0x1030;
    inline uintptr_t bTraceThroughWorld = 0xff9;

	inline uintptr_t Health = 0xCD4;
    inline uintptr_t MaxHealth = 0xCD8;

    inline uintptr_t RootComponent                      = 0x1B0;
    inline uintptr_t RelativeLocation                   = 0x140;
    inline uintptr_t RelativeRotation                   = 0x158;
    inline uintptr_t Mesh                               = 0x2F0;
    inline uintptr_t BoneArray                          = 0x650;
    inline uintptr_t BoneCache                          = 0x660;
    inline uintptr_t ComponentToWorld                   = 0x1E0;
    inline uintptr_t ComponentVelocity                  = 0x188;
    inline uintptr_t OverlappingBuildings               = 0x1C28;

    inline uintptr_t CameraLocation                     = 0x178;
    inline uintptr_t CameraRotation                     = 0x188;
    inline uintptr_t CameraFOV                          = 0x374;

    inline uintptr_t CurrentVehicle                     = 0x2AE8;
    inline uintptr_t CurrentWeapon                      = 0x998;
    inline uintptr_t WeaponData                         = 0x630;
    inline uintptr_t WeaponOffsetCorrection             = 0x2230;
    inline uintptr_t AdditionalAimOffset                = 0x21D0;
    inline uintptr_t AmmoCount                          = 0x10A4;

    inline uintptr_t ProjectileSpeed                    = 0x2664;
    inline uintptr_t ProjectileGravity                  = 0x2668;

    inline uintptr_t HabaneroComponent                  = 0x918;
    inline uintptr_t RankedProgress                     = 0xD8;
    inline uintptr_t killscore                          = 0xF48;
    inline uintptr_t RebootCount                        = 0x15AC;
    inline uintptr_t TargetedFortPawn                   = 0x1630;

    inline uintptr_t Spectators                         = 0xA90;
    inline uintptr_t SpectatorArray                     = 0x48;

    inline uintptr_t PersistentLevel                    = 0x38;
    inline uintptr_t Levels                             = 0x1F0;
    inline uintptr_t SeasonLevelUIDisplay               = 0xF4C;
    inline uintptr_t AActor = 0x38;

    inline uintptr_t Tier = 0xaa;
    inline uintptr_t ItemName = 0x38;

    inline uintptr_t FOVMinimum                     = 0x2158;
    inline uintptr_t FOVMaximum                     = 0x215C;
    inline uintptr_t PlayRate                       = 0xAB4;
    inline uintptr_t bIsReloading                   = 0x381;
    inline uintptr_t CameraManager                  = 0x328;
    inline uintptr_t ViewPitch                      = 0x28AC;
    inline uintptr_t ViewYaw                        = 0x28B4;
    inline uintptr_t AimPitchMin                    = 0x1DD8;
    inline uintptr_t AimPitchMax                    = 0x1DDC;
    inline uintptr_t CapsuleComponent               = 0x300;
    inline uintptr_t RelativeScale3D                = 0x170;
    inline uintptr_t WeaponSpread                   = 0x68;
    inline uintptr_t WeaponRampingData              = 0x21D8;
}

inline uint64_t decrypt_world(uint64_t v)
{
    return static_cast<uintptr_t>(
        _byteswap_uint64(v ^ 0x012F546CULL) - 1274101633ULL
        );
}
