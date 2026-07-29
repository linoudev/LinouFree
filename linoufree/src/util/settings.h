#pragma once

// for retards, this is basically how the cheat will know what are the defaults settings, bool for true/false and float for numbers/int numbers, basic c++ stuff, you can change these values in the menu ingame and they will be saved in memory temporarily, if you want to change the default settings just change it there
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
inline bool silentAimEnabled = false;
inline float silentAimFov = 180.0f;

// Exploits
inline bool exploitMasterSwitch = false;
inline bool exploitFovChanger = false;
inline float exploitFovValue = 120.0f;
inline bool exploitInstaReload = false;

inline bool exploitNoSpread = false;
inline bool exploitRapidFire = false;
inline float exploitRapidFireValue = 3.0f;
