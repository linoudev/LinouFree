#pragma once

inline bool showMenu = true;

inline bool boxEsp = true;
inline bool boneEsp = false;
inline bool snaplines = true;
inline bool showDistance = true;
inline bool showPlatform = true;
inline bool showPlayerId = true;
inline float maxEspDistance = 350.0f;
inline float maxBoneDistance = 230.0f;

inline bool radarEnabled = true;
inline bool radarDistance = true;
inline float radarOpacity = 150.0f;
inline int radarPositionX = 100;
inline int radarPositionY = 500;
inline int radarSize = 220;
inline int radarRange = 100;
inline bool radarGrid = true;
inline int radarGridDivisions = 8;
inline bool radarLineOfSight = false;
inline bool radarLosFill = true;
inline bool radarRotationMode = true;
inline bool radarShowPickups = false;

inline bool vsyncEnabled = false;
inline bool showFpsCounter = true;
inline float fpsColor[4] = { 0.608f, 0.588f, 0.859f, 1.0f };

inline bool aimbotEnabled = true;
inline float aimbotFov = 180.0f;
inline float aimbotSmooth = 0.9f;
inline bool aimbotVisibleOnly = false;
inline bool aimbotLockTarget = true;
inline int aimbotTargetPlayerId = -1;
inline bool drawFovCircle = true;
inline int aimbotKey = VK_RBUTTON;
inline bool waitingForKey = false;

inline bool fovChanger = false;
inline float fovValue = 120.0f;
inline bool instaReload = false;
inline bool silentAim = false;
inline bool playerSize = false;
inline float playerSizeValue = 1.0f;
inline bool noSpread = false;
inline bool rapidFire = false;
inline float rapidFireValue = 3.0f;
