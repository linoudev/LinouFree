#pragma once

// Menu
inline bool showMenu = true;

// ESP
inline bool boxEsp = true;
inline bool boneEsp = false;
inline bool snaplines = true;
inline bool showDistance = true;
inline bool showPlatform = true;
inline bool showPlayerId = true;          // Show 'Player 1-100' labels above ESP
inline float maxEspDistance = 350.0f;    // Max distance for ESP (meters)
inline float maxBoneDistance = 230.0f;   // Max distance for bone ESP (meters)

// Radar
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

// Misc
inline bool vsyncEnabled = false;
inline bool showFpsCounter = true;
inline float fpsColor[4] = { 0.608f, 0.588f, 0.859f, 1.0f };  // FPS counter color (RGBA)

// Aimbot (using PlayerAimOffset from popstar)
inline bool aimbotEnabled = true;
inline float aimbotFov = 180.0f;        // FOV radius in pixels
inline float aimbotSmooth = 0.9f;       // Higher = slower/smoother (1 = instant, 10 = smooth)
inline bool aimbotVisibleOnly = false;  // Aim at all targets (not just visible)
inline bool aimbotLockTarget = true;   // Lock onto target until button release or target dies
inline int aimbotTargetPlayerId = -1;   // Target specific player ID (-1 = auto, 0+ = specific player)
inline bool drawFovCircle = true;       // Draw FOV circle on screen
inline int aimbotKey = VK_RBUTTON;      // Right mouse button
inline bool waitingForKey = false;      // Key binding state

// Exploits
inline bool fovChanger = false;
inline float fovValue = 120.0f;
inline bool instaReload = false;
inline bool silentAim = false;
inline bool playerSize = false;
inline float playerSizeValue = 1.0f;
inline bool noSpread = false;
inline bool rapidFire = false;
inline float rapidFireValue = 3.0f;
