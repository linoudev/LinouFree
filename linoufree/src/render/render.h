#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_4.h>
#include <dwmapi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwmapi.lib")

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include "../util/settings.h"
#include "esp.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

inline HWND g_overlayHwnd = nullptr;

namespace d3d {
    inline ID3D11Device* pDevice = nullptr;
    inline ID3D11DeviceContext* pContext = nullptr;
    inline IDXGISwapChain* pSwapChain = nullptr;
    inline ID3D11RenderTargetView* pRenderTarget = nullptr;

    inline void ApplyStyle() {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();
        ImGui::StyleColorsDark();
    }

    inline bool Init(HWND hwnd) {
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL level;
        if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            nullptr, 0, D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, &level, &pContext)))
            return false;

        ID3D11Texture2D* pBackBuffer;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTarget);
        pBackBuffer->Release();

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(pDevice, pContext);
        ApplyStyle();

        return true;
    }

    inline void Shutdown() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (pRenderTarget) pRenderTarget->Release();
        if (pSwapChain) pSwapChain->Release();
        if (pContext) pContext->Release();
        if (pDevice) pDevice->Release();
    }
}

inline const char* GetKeyName(int vk) {
    switch (vk) {
        case VK_LBUTTON: return "Left Mouse";
        case VK_RBUTTON: return "Right Mouse";
        case VK_MBUTTON: return "Middle Mouse";
        case VK_XBUTTON1: return "Mouse 4";
        case VK_XBUTTON2: return "Mouse 5";
        case VK_SHIFT: return "Shift";
        case VK_CONTROL: return "Ctrl";
        case VK_MENU: return "Alt";
        case VK_CAPITAL: return "Caps Lock";
        case VK_TAB: return "Tab";
        case VK_SPACE: return "Space";
        case VK_RETURN: return "Enter";
        case VK_BACK: return "Backspace";
        case VK_ESCAPE: return "Escape";
        case VK_INSERT: return "Insert";
        case VK_DELETE: return "Delete";
        case VK_HOME: return "Home";
        case VK_END: return "End";
        case VK_PRIOR: return "Page Up";
        case VK_NEXT: return "Page Down";
        default:
            if (vk >= 'A' && vk <= 'Z') {
                static char buf[2] = {};
                buf[0] = (char)vk;
                buf[1] = 0;
                return buf;
            }
            if (vk >= '0' && vk <= '9') {
                static char buf[2] = {};
                buf[0] = (char)vk;
                buf[1] = 0;
                return buf;
            }
            if (vk >= VK_F1 && vk <= VK_F12) {
                static char buf[4] = {};
                snprintf(buf, 4, "F%d", vk - VK_F1 + 1);
                return buf;
            }
            return "Unknown";
    }
}

namespace colors {
    inline ImColor bg = ImColor(16, 16, 18, 255);
    inline ImColor sidebar = ImColor(14, 14, 16, 255);
    inline ImColor accent = ImColor(155, 150, 219, 255);
    inline ImColor accentDark = ImColor(100, 95, 160, 255);
    inline ImColor textPrimary = ImColor(255, 255, 255, 255);
    inline ImColor textSecondary = ImColor(140, 140, 150, 255);
    inline ImColor separator = ImColor(255, 255, 255, 8);
    inline ImColor footer = ImColor(12, 12, 14, 255);
}

namespace menu {
    inline bool wasMenuOpen = false;
    inline int currentTab = 0;

    inline void ApplyModernStyle() {
        ImGuiStyle& s = ImGui::GetStyle();

        s.WindowRounding = 10.0f;
        s.FrameRounding = 6.0f;
        s.GrabRounding = 4.0f;
        s.ChildRounding = 6.0f;
        s.PopupRounding = 6.0f;
        s.ScrollbarRounding = 4.0f;

        s.WindowPadding = ImVec2(12, 12);
        s.FramePadding = ImVec2(8, 5);
        s.ItemSpacing = ImVec2(8, 8);
        s.ItemInnerSpacing = ImVec2(6, 6);

        s.WindowBorderSize = 0.0f;
        s.FrameBorderSize = 0.0f;
        s.ScrollbarSize = 10.0f;

        ImVec4* c = s.Colors;
        c[ImGuiCol_WindowBg] = ImVec4(0.063f, 0.063f, 0.071f, 1.0f);
        c[ImGuiCol_ChildBg] = ImVec4(0.055f, 0.055f, 0.063f, 1.0f);
        c[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 0.03f);
        c[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.1f, 0.11f, 1.0f);
        c[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.15f, 0.17f, 1.0f);
        c[ImGuiCol_FrameBgActive] = ImVec4(0.2f, 0.2f, 0.22f, 1.0f);
        c[ImGuiCol_TitleBg] = ImVec4(0.055f, 0.055f, 0.063f, 1.0f);
        c[ImGuiCol_TitleBgActive] = ImVec4(0.055f, 0.055f, 0.063f, 1.0f);
        c[ImGuiCol_CheckMark] = ImVec4(0.608f, 0.588f, 0.859f, 1.0f);
        c[ImGuiCol_SliderGrab] = ImVec4(0.608f, 0.588f, 0.859f, 1.0f);
        c[ImGuiCol_SliderGrabActive] = ImVec4(0.7f, 0.68f, 0.95f, 1.0f);
        c[ImGuiCol_Button] = ImVec4(0.608f, 0.588f, 0.859f, 0.3f);
        c[ImGuiCol_ButtonHovered] = ImVec4(0.608f, 0.588f, 0.859f, 0.5f);
        c[ImGuiCol_ButtonActive] = ImVec4(0.608f, 0.588f, 0.859f, 0.7f);
        c[ImGuiCol_Header] = ImVec4(0.608f, 0.588f, 0.859f, 0.2f);
        c[ImGuiCol_HeaderHovered] = ImVec4(0.608f, 0.588f, 0.859f, 0.3f);
        c[ImGuiCol_HeaderActive] = ImVec4(0.608f, 0.588f, 0.859f, 0.4f);
        c[ImGuiCol_Separator] = ImVec4(1.0f, 1.0f, 1.0f, 0.05f);
        c[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        c[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.55f, 0.59f, 1.0f);
    }

    inline void TabButton(const char* label, int tabIndex) {
        ImVec4 activeCol = ImVec4(0.608f, 0.588f, 0.859f, 1.0f);
        ImVec4 inactiveCol = ImVec4(0.4f, 0.4f, 0.43f, 1.0f);

        bool isActive = (currentTab == tabIndex);

        ImGui::PushStyleColor(ImGuiCol_Text, isActive ? activeCol : inactiveCol);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.05f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.1f));

        if (ImGui::Button(label, ImVec2(90, 30))) {
            currentTab = tabIndex;
        }

        ImGui::PopStyleColor(4);

        if (isActive) {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddRectFilled(
                ImVec2(min.x + 10, max.y - 2),
                ImVec2(max.x - 10, max.y),
                IM_COL32(155, 150, 219, 255),
                2.0f
            );
        }
    }

    inline void Render() {
        static bool styleApplied = false;
        if (!styleApplied) {
            ApplyModernStyle();
            styleApplied = true;
        }

        if (GetAsyncKeyState(VK_INSERT) & 1) {
            showMenu = !showMenu;
        }

        if (showMenu != wasMenuOpen) {
            wasMenuOpen = showMenu;
            LONG style = GetWindowLong(g_overlayHwnd, GWL_EXSTYLE);
            if (showMenu) {
                SetWindowLong(g_overlayHwnd, GWL_EXSTYLE, style & ~WS_EX_TRANSPARENT);
                SetForegroundWindow(g_overlayHwnd);
            } else {
                SetWindowLong(g_overlayHwnd, GWL_EXSTYLE, style | WS_EX_TRANSPARENT);
            }
        }

        if (!showMenu) return;

        ImGui::SetNextWindowSize(ImVec2(420, 380), ImGuiCond_FirstUseEver);
        ImGui::Begin("##MainMenu", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar);

        auto draw = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();

        draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 50), IM_COL32(14, 14, 16, 255), 10.0f, ImDrawFlags_RoundCornersTop);

        ImGui::SetCursorPos(ImVec2(15, 15));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.608f, 0.588f, 0.859f, 1.0f));
        ImGui::Text("Linou Free");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.59f, 1.0f));
        ImGui::Text("");
        ImGui::PopStyleColor();

        ImGui::SetCursorPos(ImVec2(size.x - 300, 10));
        TabButton("Aimbot", 0);
        ImGui::SameLine();
        TabButton("Visuals", 1);
        ImGui::SameLine();
        TabButton("Misc", 2);
        ImGui::SameLine();
        TabButton("Exploits", 3);

        draw->AddRectFilled(ImVec2(pos.x, pos.y + 50), ImVec2(pos.x + size.x, pos.y + 51), IM_COL32(255, 255, 255, 8));

        ImGui::SetCursorPos(ImVec2(15, 60));
        ImGui::BeginChild("##Content", ImVec2(size.x - 30, size.y - 100), false);

        if (currentTab == 0) {

            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.608f, 0.588f, 0.859f, 1.0f));
            ImGui::Text("Aimbot Settings");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Enable Aimbot", &aimbotEnabled);

            if (aimbotEnabled) {
                ImGui::Spacing();

                ImGui::Checkbox("Visible Only", &aimbotVisibleOnly);
                ImGui::Checkbox("Lock Target", &aimbotLockTarget);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Lock onto target until button release or target dies");
                }

                if (aimbotLockTarget) {
                    ImGui::Spacing();
                    ImGui::PushItemWidth(100);
                    ImGui::InputInt("Target Player ID", &aimbotTargetPlayerId, 1, 10);
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Set to -1 for auto target, or specific player number (0+)");
                    }
                    if (aimbotTargetPlayerId < -1) aimbotTargetPlayerId = -1;
                    ImGui::PopItemWidth();
                }

                ImGui::Checkbox("Draw FOV Circle", &drawFovCircle);

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::SliderFloat("FOV Radius", &aimbotFov, 20.0f, 400.0f, "%.0f px");
                ImGui::SliderFloat("Smoothing", &aimbotSmooth, 0.1f, 20.0f, "%.1f");

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::Spacing();
                if (!waitingForKey) {
                    char keyLabel[64];
                    snprintf(keyLabel, 64, "Aim Key: %s", GetKeyName(aimbotKey));
                    if (ImGui::Button(keyLabel, ImVec2(180, 0))) {
                        waitingForKey = true;
                    }
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.5f, 0.6f));
                    if (ImGui::Button("Press a key...", ImVec2(180, 0))) {
                        waitingForKey = false;
                    }
                    ImGui::PopStyleColor();
                    for (int vk = 0; vk < 256; vk++) {
                        if (GetAsyncKeyState(vk) & 1) {
                            if (vk != VK_INSERT && vk != VK_ESCAPE) {
                                aimbotKey = vk;
                            }
                            waitingForKey = false;
                            break;
                        }
                    }
                }
            }
        }
        else if (currentTab == 1) {

            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.608f, 0.588f, 0.859f, 1.0f));
            ImGui::Text("ESP Settings");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Box ESP", &boxEsp);
            ImGui::Checkbox("Bone ESP", &boneEsp);
            ImGui::Checkbox("Snaplines", &snaplines);
            ImGui::Checkbox("Show Player ID", &showPlayerId);
            ImGui::Checkbox("Show Distance", &showDistance);
            ImGui::Checkbox("Show Platform", &showPlatform);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.608f, 0.588f, 0.859f, 1.0f));
            ImGui::Text("Radar Settings");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ImGui::Checkbox("Enable Radar", &radarEnabled);
            if (radarEnabled) {

                ImGui::Checkbox("Radar Distance", &radarDistance);
                ImGui::Checkbox("Rotated Orientation", &radarRotationMode);
                ImGui::Checkbox("Show Grid", &radarGrid);
                ImGui::Checkbox("Line of Sight", &radarLineOfSight);
                if (radarLineOfSight) {
                    ImGui::Checkbox("Filled LOS", &radarLosFill);
                }

                ImGui::Spacing();
                ImGui::SliderFloat("Opacity", &radarOpacity, 0.0f, 255.0f, "%.0f");
                ImGui::SliderInt("Position X", &radarPositionX, 0, Width);
                ImGui::SliderInt("Position Y", &radarPositionY, 0, Height);
                ImGui::SliderInt("Size", &radarSize, 50, 800);
                ImGui::SliderInt("Range", &radarRange, 50, 1000);
                if (radarGrid) {
                    ImGui::SliderInt("Grid Divisions", &radarGridDivisions, 2, 20);
                }
            }

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.59f, 1.0f));
            ImGui::TextWrapped("Green = Visible, Red = Player, Yellow = Bot");
            ImGui::PopStyleColor();
        }
        else if (currentTab == 2) {

            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.608f, 0.588f, 0.859f, 1.0f));
            ImGui::Text("Misc Settings");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("VSync", &vsyncEnabled);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Enable vertical sync (limits FPS to monitor refresh rate)");
            }

            ImGui::Checkbox("Show FPS Counter", &showFpsCounter);

            if (showFpsCounter) {
                ImGui::Spacing();
                ImGui::ColorEdit4("FPS Color", fpsColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
            }
        }
        else if (currentTab == 3) {

            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.608f, 0.588f, 0.859f, 1.0f));
            ImGui::Text("Exploits");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("FOV Changer", &fovChanger);
            if (fovChanger) {
                ImGui::SameLine();
                ImGui::PushItemWidth(120);
                ImGui::SliderFloat("##FOV", &fovValue, 10.0f, 180.0f, "%.0f");
                ImGui::PopItemWidth();
            }
            ImGui::Checkbox("Insta Reload", &instaReload);
            ImGui::Checkbox("Silent Aim", &silentAim);
            ImGui::Spacing();

            ImGui::Checkbox("No Spread", &noSpread);
            ImGui::Spacing();

            ImGui::Checkbox("Player Size", &playerSize);
            if (playerSize) {
                ImGui::SameLine();
                ImGui::PushItemWidth(120);
                ImGui::SliderFloat("##PlayerSize", &playerSizeValue, 0.1f, 5.0f, "%.1f");
                ImGui::PopItemWidth();
            }
            ImGui::Checkbox("Rapid Fire", &rapidFire);
            if (rapidFire) {
                ImGui::SameLine();
                ImGui::PushItemWidth(120);
                ImGui::SliderFloat("##RapidFire", &rapidFireValue, 0.5f, 10.0f, "%.1f");
                ImGui::PopItemWidth();
            }
        }

        ImGui::EndChild();

        draw->AddRectFilled(ImVec2(pos.x, pos.y + size.y - 30), ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(12, 12, 14, 255), 10.0f, ImDrawFlags_RoundCornersBottom);
        draw->AddRectFilled(ImVec2(pos.x, pos.y + size.y - 31), ImVec2(pos.x + size.x, pos.y + size.y - 30), IM_COL32(255, 255, 255, 8));

        ImGui::SetCursorPos(ImVec2(15, size.y - 23));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.45f, 0.5f, 1.0f));
        ImGui::Text("Press INSERT to toggle menu");
        ImGui::PopStyleColor();

        ImGui::End();
    }
}

namespace overlay {
    inline WNDCLASSEX wc = {};

    inline LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    inline bool Create() {
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = L"OverlayClass";
        RegisterClassEx(&wc);

        g_overlayHwnd = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
            wc.lpszClassName, L"",
            WS_POPUP,
            0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
            nullptr, nullptr, wc.hInstance, nullptr
        );

        if (!g_overlayHwnd) return false;

        SetLayeredWindowAttributes(g_overlayHwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

        MARGINS margin = { -1 };
        DwmExtendFrameIntoClientArea(g_overlayHwnd, &margin);

        ShowWindow(g_overlayHwnd, SW_SHOWDEFAULT);
        UpdateWindow(g_overlayHwnd);

        return d3d::Init(g_overlayHwnd);
    }
}

namespace fps {
    inline void Draw() {
        if (!showFpsCounter) return;

        auto draw = ImGui::GetBackgroundDrawList();
        float fps = ImGui::GetIO().Framerate;

        char fpsText[32];
        snprintf(fpsText, sizeof(fpsText), "FPS: %.0f", fps);

        float x = 10.0f;
        float y = 10.0f;

        ImU32 textColor = IM_COL32(
            (int)(fpsColor[0] * 255),
            (int)(fpsColor[1] * 255),
            (int)(fpsColor[2] * 255),
            (int)(fpsColor[3] * 255)
        );

        draw->AddText(ImVec2(x + 1, y + 1), IM_COL32(0, 0, 0, 255), fpsText);

        draw->AddText(ImVec2(x, y), textColor, fpsText);
    }
}

inline void RenderLoop() {
    MSG msg = {};
    LARGE_INTEGER freq, last;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ActorLoop();

        radar::DrawBackground();

        fps::Draw();

        menu::Render();

        ImGui::Render();
        float clear[4] = { 0, 0, 0, 0 };
        d3d::pContext->OMSetRenderTargets(1, &d3d::pRenderTarget, nullptr);
        d3d::pContext->ClearRenderTargetView(d3d::pRenderTarget, clear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        d3d::pSwapChain->Present(vsyncEnabled ? 1 : 0, 0);

        if (!vsyncEnabled) {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            double elapsed = double(now.QuadPart - last.QuadPart) / freq.QuadPart * 1000.0;
            if (elapsed < 6.9) {
                Sleep(DWORD(6.9 - elapsed));
            }
            QueryPerformanceCounter(&last);
        }
    }

    d3d::Shutdown();
}
