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


// most unoptimized render youll ever see in your life!


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

inline HWND g_overlayHwnd = nullptr;

namespace d3d {
    inline ID3D11Device*            pDevice      = nullptr;
    inline ID3D11DeviceContext*     pContext      = nullptr;
    inline IDXGISwapChain*          pSwapChain    = nullptr;
    inline ID3D11RenderTargetView*  pRenderTarget = nullptr;

    inline bool Init(HWND hwnd) {
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount                        = 2;
        sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow                       = hwnd;
        sd.SampleDesc.Count                   = 1;
        sd.Windowed                           = TRUE;
        sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL level;
        if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
            nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, &level, &pContext)))
            return false;

        ID3D11Texture2D* bb;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&bb));
        pDevice->CreateRenderTargetView(bb, nullptr, &pRenderTarget);
        bb->Release();

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(pDevice, pContext);
        return true;
    }

    inline void Shutdown() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        if (pRenderTarget) pRenderTarget->Release();
        if (pSwapChain)    pSwapChain->Release();
        if (pContext)      pContext->Release();
        if (pDevice)       pDevice->Release();
    }
}

namespace theme {
    constexpr ImU32 BG          = IM_COL32(12,  12,  15,  255);
    constexpr ImU32 PANEL       = IM_COL32(18,  18,  22,  255);
    constexpr ImU32 PANEL_HOVER = IM_COL32(24,  24,  30,  255);
    constexpr ImU32 BORDER      = IM_COL32(255, 255, 255, 12);
    constexpr ImU32 ACCENT      = IM_COL32(120, 110, 220, 255);
    constexpr ImU32 ACCENT_DIM  = IM_COL32( 80,  72, 160, 255);
    constexpr ImU32 ACCENT_GLOW = IM_COL32(120, 110, 220,  40);
    constexpr ImU32 TEXT        = IM_COL32(230, 230, 240, 255);
    constexpr ImU32 TEXT_DIM    = IM_COL32(110, 110, 130, 255);
    constexpr ImU32 TEXT_ACCENT = IM_COL32(160, 152, 255, 255);
    constexpr ImU32 DANGER      = IM_COL32(220,  60,  60, 255);
    constexpr ImU32 SUCCESS     = IM_COL32( 60, 200, 100, 255);
    constexpr ImU32 SEPARATOR   = IM_COL32(255, 255, 255,  8);

    inline ImVec4 ToVec4(ImU32 col) {
        return ImVec4(
            ((col >>  0) & 0xFF) / 255.f,
            ((col >>  8) & 0xFF) / 255.f,
            ((col >> 16) & 0xFF) / 255.f,
            ((col >> 24) & 0xFF) / 255.f
        );
    }
}

namespace menu {
    inline bool wasOpen     = false;
    inline int  activeTab   = 0;
    inline bool styleReady  = false;

    inline void SetupStyle() {
        ImGuiStyle& s = ImGui::GetStyle();
        s.WindowRounding    = 12.f;  s.ChildRounding     = 8.f;
        s.FrameRounding     = 6.f;   s.PopupRounding     = 8.f;
        s.GrabRounding      = 4.f;   s.ScrollbarRounding = 4.f;
        s.WindowPadding     = ImVec2(0, 0);
        s.FramePadding      = ImVec2(10, 5);
        s.ItemSpacing       = ImVec2(8, 7);
        s.ItemInnerSpacing  = ImVec2(6, 4);
        s.WindowBorderSize  = 1.f;
        s.FrameBorderSize   = 0.f;
        s.ScrollbarSize     = 8.f;

        ImVec4* c = s.Colors;
        auto v = [](ImU32 col) { return theme::ToVec4(col); };
        c[ImGuiCol_WindowBg]          = v(theme::BG);
        c[ImGuiCol_ChildBg]           = v(theme::PANEL);
        c[ImGuiCol_Border]            = v(theme::BORDER);
        c[ImGuiCol_FrameBg]           = ImVec4(0.09f,0.09f,0.12f,1.f);
        c[ImGuiCol_FrameBgHovered]    = ImVec4(0.13f,0.13f,0.17f,1.f);
        c[ImGuiCol_FrameBgActive]     = ImVec4(0.18f,0.18f,0.23f,1.f);
        c[ImGuiCol_TitleBg]           = v(theme::BG);
        c[ImGuiCol_TitleBgActive]     = v(theme::BG);
        c[ImGuiCol_CheckMark]         = v(theme::ACCENT);
        c[ImGuiCol_SliderGrab]        = v(theme::ACCENT);
        c[ImGuiCol_SliderGrabActive]  = ImVec4(0.56f,0.52f,0.95f,1.f);
        c[ImGuiCol_Button]            = ImVec4(0.47f,0.43f,0.86f,0.18f);
        c[ImGuiCol_ButtonHovered]     = ImVec4(0.47f,0.43f,0.86f,0.35f);
        c[ImGuiCol_ButtonActive]      = ImVec4(0.47f,0.43f,0.86f,0.55f);
        c[ImGuiCol_Header]            = ImVec4(0.47f,0.43f,0.86f,0.15f);
        c[ImGuiCol_HeaderHovered]     = ImVec4(0.47f,0.43f,0.86f,0.25f);
        c[ImGuiCol_HeaderActive]      = ImVec4(0.47f,0.43f,0.86f,0.40f);
        c[ImGuiCol_Separator]         = v(theme::SEPARATOR);
        c[ImGuiCol_Text]              = v(theme::TEXT);
        c[ImGuiCol_TextDisabled]      = v(theme::TEXT_DIM);
        c[ImGuiCol_ScrollbarBg]       = ImVec4(0,0,0,0);
        c[ImGuiCol_ScrollbarGrab]     = ImVec4(0.3f,0.3f,0.4f,0.5f);
        c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4f,0.4f,0.55f,0.7f);
    }

    inline void SectionHeader(const char* label) {
        ImDrawList* dl  = ImGui::GetWindowDrawList();
        ImVec2 p        = ImGui::GetCursorScreenPos();
        dl->AddRectFilled(ImVec2(p.x, p.y + 2), ImVec2(p.x + 3, p.y + 14),
                          theme::ACCENT, 2.f);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
        ImGui::PushStyleColor(ImGuiCol_Text, theme::ToVec4(theme::TEXT_ACCENT));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    inline void Checkbox(const char* label, bool* v) {
        ImGui::PushStyleColor(ImGuiCol_Text, theme::ToVec4(*v ? theme::TEXT : theme::TEXT_DIM));
        ImGui::Checkbox(label, v);
        ImGui::PopStyleColor();
    }

    inline void TabButton(ImDrawList* dl, const char* icon, const char* label,
                          int idx, float x, float y, float w, float h) {
        bool active  = (activeTab == idx);
        ImVec2 bMin  = ImVec2(x, y);
        ImVec2 bMax  = ImVec2(x + w, y + h);

        ImVec2 mp = ImGui::GetMousePos();
        bool hov = mp.x >= bMin.x && mp.x <= bMax.x && mp.y >= bMin.y && mp.y <= bMax.y;

        if (active)
            dl->AddRectFilled(bMin, bMax, theme::ACCENT_GLOW, 6.f);
        else if (hov)
            dl->AddRectFilled(bMin, bMax, IM_COL32(255,255,255,6), 6.f);

        if (active)
            dl->AddRectFilled(ImVec2(x + 8, y + h - 2), ImVec2(x + w - 8, y + h),
                              theme::ACCENT, 2.f);

        char buf[64];
        snprintf(buf, sizeof(buf), "%s  %s", icon, label);
        ImVec2 ts = ImGui::CalcTextSize(buf);
        float tx = x + (w - ts.x) * 0.5f;
        float ty = y + (h - ts.y) * 0.5f;

        ImU32 tc = active ? theme::TEXT_ACCENT : (hov ? theme::TEXT : theme::TEXT_DIM);
        dl->AddText(ImVec2(tx, ty), tc, buf);
        ImGui::SetCursorScreenPos(bMin);
        ImGui::InvisibleButton(label, ImVec2(w, h));
        if (ImGui::IsItemClicked()) activeTab = idx;
    }

    inline void DrawAimbotTab() {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
        SectionHeader("AIMBOT");

        Checkbox("Enable Aimbot", &aimbotEnabled);
        ImGui::Spacing();

        if (aimbotEnabled) {
            ImGui::Indent(10);
            Checkbox("Visible Only", &aimbotVisibleOnly);
            Checkbox("Lock Target",  &aimbotLockTarget);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Keeps aim locked until button release or target dies");

            if (aimbotLockTarget) {
                ImGui::Spacing();
                ImGui::PushItemWidth(110);
                ImGui::InputInt("Target Player##pid", &aimbotTargetPlayerId, 1, 5);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("-1 = nearest in FOV  |  0+ = specific player number");
                if (aimbotTargetPlayerId < -1) aimbotTargetPlayerId = -1;
                ImGui::PopItemWidth();
            }

            Checkbox("FOV Circle", &drawFovCircle);
            ImGui::Unindent(10);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        SectionHeader("SETTINGS");

        ImGui::PushItemWidth(-1);
        ImGui::SliderFloat("FOV Radius##fov",  &aimbotFov,    20.f, 400.f, "%.0f px");
        ImGui::SliderFloat("Smoothing##smt",   &aimbotSmooth,  0.1f, 20.f, "%.1f");
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, theme::ToVec4(theme::TEXT_DIM));
        ImGui::TextUnformatted("  Aim Key: Right Mouse Button");
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        SectionHeader("SILENT AIM");

        Checkbox("Enable Silent Aim", &silentAimEnabled);
        if (silentAimEnabled) {
            ImGui::PushItemWidth(-1);
            ImGui::SliderFloat("Silent FOV##sfov", &silentAimFov, 20.f, 400.f, "%.0f px");
            ImGui::PopItemWidth();
            ImGui::PushStyleColor(ImGuiCol_Text, theme::ToVec4(theme::TEXT_DIM));
            ImGui::TextUnformatted("  No key binding — always active");
            ImGui::PopStyleColor();
        }
    }

    inline void DrawVisualsTab() {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
        SectionHeader("ESP");

        Checkbox("Box ESP",      &boxEsp);
        Checkbox("Bone ESP",     &boneEsp);
        Checkbox("Snaplines",    &snaplines);
        Checkbox("Player ID",    &showPlayerId);
        Checkbox("Distance",     &showDistance);
        Checkbox("Platform",     &showPlatform);

        ImGui::PushItemWidth(-1);
        ImGui::SliderFloat("Max Distance##espd", &maxEspDistance, 50.f, 700.f, "%.0f m");
        if (boneEsp)
            ImGui::SliderFloat("Bone Distance##bnd", &maxBoneDistance, 50.f, 500.f, "%.0f m");
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        SectionHeader("RADAR");

        Checkbox("Enable Radar",       &radarEnabled);
        if (radarEnabled) {
            ImGui::Indent(10);
            Checkbox("Show Distance##rd",    &radarDistance);
            Checkbox("Rotate with Camera",   &radarRotationMode);
            Checkbox("Show Grid",            &radarGrid);

            ImGui::PushItemWidth(-1);
            ImGui::SliderFloat("Opacity##rop",   &radarOpacity,   0.f, 255.f, "%.0f");
            ImGui::SliderInt("Size##rsz",         &radarSize,      80, 400);
            ImGui::SliderInt("Range (m)##rrng",   &radarRange,     50, 800);
            ImGui::SliderInt("Pos X##rpx",        &radarPositionX, 0, Width);
            ImGui::SliderInt("Pos Y##rpy",        &radarPositionY, 0, Height);
            if (radarGrid)
                ImGui::SliderInt("Grid Div##rgd", &radarGridDivisions, 2, 16);
            ImGui::PopItemWidth();
            ImGui::Unindent(10);
        }

        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, theme::ToVec4(theme::TEXT_DIM));
        ImGui::TextUnformatted("  Green=Visible  Red=Player  Yellow=Bot");
        ImGui::PopStyleColor();
    }

    inline void DrawMiscTab() {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
        SectionHeader("DISPLAY");

        Checkbox("VSync",        &vsyncEnabled);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Caps FPS to monitor refresh rate");
        Checkbox("FPS Counter",  &showFpsCounter);

        if (showFpsCounter) {
            ImGui::Spacing();
            ImGui::ColorEdit4("FPS Color##fc", fpsColor,
                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
        }
    }

    inline void DrawExploitsTab() {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
        SectionHeader("EXPLOITS");

        Checkbox("Master Switch", &exploitMasterSwitch);
        ImGui::Spacing();

        if (exploitMasterSwitch) {
            ImGui::Indent(10);

            Checkbox("FOV Changer", &exploitFovChanger);
            if (exploitFovChanger) {
                ImGui::PushItemWidth(-1);
                ImGui::SliderFloat("FOV Value##fovx", &exploitFovValue, 10.f, 180.f, "%.0f");
                ImGui::PopItemWidth();
            }

            Checkbox("Insta Reload", &exploitInstaReload);

            Checkbox("No Spread", &exploitNoSpread);

            Checkbox("Rapid Fire", &exploitRapidFire);
            if (exploitRapidFire) {
                ImGui::PushItemWidth(-1);
                ImGui::SliderFloat("Rate##rpf", &exploitRapidFireValue, 0.5f, 10.f, "%.1f");
                ImGui::PopItemWidth();
            }

            ImGui::Unindent(10);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, theme::ToVec4(theme::TEXT_DIM));
        ImGui::TextUnformatted("  Some exploits may need tweaking per patch");
        ImGui::PopStyleColor();
    }

    inline void Render() {
        if (!styleReady) { SetupStyle(); styleReady = true; }

        if (GetAsyncKeyState(VK_INSERT) & 1) showMenu = !showMenu;

        if (showMenu != wasOpen) {
            wasOpen = showMenu;
            LONG st = GetWindowLong(g_overlayHwnd, GWL_EXSTYLE);
            if (showMenu) {
                SetWindowLong(g_overlayHwnd, GWL_EXSTYLE, st & ~WS_EX_TRANSPARENT);
                SetForegroundWindow(g_overlayHwnd);
            } else {
                SetWindowLong(g_overlayHwnd, GWL_EXSTYLE, st | WS_EX_TRANSPARENT);
            }
        }

        if (!showMenu) return;
        const float W = 480.f, H = 440.f;
        const float SIDEBAR = 130.f;
        const float HEADER  = 52.f;
        const float FOOTER  = 28.f;

        ImGui::SetNextWindowSize(ImVec2(W, H), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.97f);
        ImGui::Begin("##linou", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);

        ImDrawList* dl  = ImGui::GetWindowDrawList();
        ImVec2 wPos     = ImGui::GetWindowPos();
        ImVec2 wSz      = ImGui::GetWindowSize();
        dl->AddRectFilled(wPos, ImVec2(wPos.x + wSz.x, wPos.y + HEADER),
                          theme::PANEL, 12.f, ImDrawFlags_RoundCornersTop);

        ImGui::SetCursorPos(ImVec2(16, 14));
        dl->AddRectFilled(ImVec2(wPos.x + 14, wPos.y + 15),
                          ImVec2(wPos.x + 17, wPos.y + 37), theme::ACCENT, 2.f);
        ImGui::SetCursorPos(ImVec2(24, 15));
        ImGui::PushStyleColor(ImGuiCol_Text, theme::ToVec4(theme::TEXT));
        ImGui::Text("Linou");
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 4);
        ImGui::PushStyleColor(ImGuiCol_Text, theme::ToVec4(theme::TEXT_ACCENT));
        ImGui::Text("Free");
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 8);
        ImGui::PushStyleColor(ImGuiCol_Text, theme::ToVec4(theme::TEXT_DIM));
        ImGui::Text("v67.0");
        ImGui::PopStyleColor();

        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::InvisibleButton("##drag", ImVec2(wSz.x, HEADER));
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            ImVec2 np    = ImVec2(wPos.x + delta.x, wPos.y + delta.y);
            ImGui::SetWindowPos(np);
        }

        dl->AddRectFilled(ImVec2(wPos.x, wPos.y + HEADER),
                          ImVec2(wPos.x + wSz.x, wPos.y + HEADER + 1), theme::SEPARATOR);

        dl->AddRectFilled(ImVec2(wPos.x, wPos.y + HEADER + 1),
                          ImVec2(wPos.x + SIDEBAR, wPos.y + wSz.y), IM_COL32(15,15,18,255),
                          0.f);
        dl->AddRectFilled(ImVec2(wPos.x + SIDEBAR, wPos.y + HEADER + 1),
                          ImVec2(wPos.x + SIDEBAR + 1, wPos.y + wSz.y), theme::BORDER);

        float tabW = SIDEBAR - 8.f;
        float tabH = 36.f;
        float tabX = wPos.x + 4.f;
        float tabY = wPos.y + HEADER + 10.f;
        float tabGap = 4.f;

        TabButton(dl, "A", "Aimbot",   0, tabX, tabY,                  tabW, tabH);
        TabButton(dl, "V", "Visuals",  1, tabX, tabY + (tabH+tabGap),     tabW, tabH);
        TabButton(dl, "M", "Misc",     2, tabX, tabY + (tabH+tabGap)*2,   tabW, tabH);
        TabButton(dl, "E", "Exploits", 3, tabX, tabY + (tabH+tabGap)*3,   tabW, tabH);

        ImGui::SetCursorScreenPos(ImVec2(wPos.x + 4, wPos.y + wSz.y - FOOTER - 18));
        ImGui::PushStyleColor(ImGuiCol_Text, theme::ToVec4(theme::TEXT_DIM));
        ImGui::Text("free in discord.gg/linouservices");
        ImGui::PopStyleColor();

        float contentX = SIDEBAR + 10.f;
        float contentW = wSz.x - SIDEBAR - 14.f;
        float contentH = wSz.y - HEADER - FOOTER - 10.f;

        ImGui::SetCursorPos(ImVec2(contentX, HEADER + 6.f));
        ImGui::BeginChild("##content", ImVec2(contentW, contentH), false,
                          ImGuiWindowFlags_NoScrollbar);

        if      (activeTab == 0) DrawAimbotTab();
        else if (activeTab == 1) DrawVisualsTab();
        else if (activeTab == 2) DrawMiscTab();
        else if (activeTab == 3) DrawExploitsTab();

        ImGui::EndChild();

        dl->AddRectFilled(ImVec2(wPos.x, wPos.y + wSz.y - FOOTER),
                          ImVec2(wPos.x + wSz.x, wPos.y + wSz.y),
                          theme::PANEL, 12.f, ImDrawFlags_RoundCornersBottom);
        dl->AddRectFilled(ImVec2(wPos.x, wPos.y + wSz.y - FOOTER),
                          ImVec2(wPos.x + wSz.x, wPos.y + wSz.y - FOOTER + 1), theme::SEPARATOR);

        ImGui::SetCursorPos(ImVec2(contentX, wSz.y - FOOTER + 7.f));
        ImGui::PushStyleColor(ImGuiCol_Text, theme::ToVec4(theme::TEXT_DIM));
        ImGui::Text("INSERT  toggle menu");
        ImGui::PopStyleColor();

        ImGui::End();
    }
} // namespace menu <3

namespace overlay {
    inline WNDCLASSEX wc = {};

    inline LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
        if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    inline bool Create() {
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = WndProc;
        wc.hInstance     = GetModuleHandle(nullptr);
        wc.lpszClassName = L"OverlayClass";
        RegisterClassEx(&wc);

        g_overlayHwnd = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
            wc.lpszClassName, L"",
            WS_POPUP, 0, 0,
            GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
            nullptr, nullptr, wc.hInstance, nullptr);

        if (!g_overlayHwnd) return false;

        SetLayeredWindowAttributes(g_overlayHwnd, RGB(0,0,0), 0, LWA_COLORKEY);
        MARGINS m = { -1 };
        DwmExtendFrameIntoClientArea(g_overlayHwnd, &m);
        ShowWindow(g_overlayHwnd, SW_SHOWDEFAULT);
        UpdateWindow(g_overlayHwnd);

        return d3d::Init(g_overlayHwnd);
    }
}
// really accurate fps counter (no)
namespace fps {
    inline void Draw(ImDrawList* dl) {
        if (!showFpsCounter) return;
        char buf[32];
        snprintf(buf, sizeof(buf), "%.0f fps", ImGui::GetIO().Framerate);
        ImU32 col = IM_COL32(
            (int)(fpsColor[0]*255), (int)(fpsColor[1]*255),
            (int)(fpsColor[2]*255), (int)(fpsColor[3]*255));
        dl->AddText(ImVec2(11, 11), IM_COL32(0,0,0,200), buf);
        dl->AddText(ImVec2(10, 10), col, buf);
    }
}
// peak performance render loop (no)
inline void RenderLoop() {
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImDrawList* bg = ImGui::GetBackgroundDrawList();

        ActorLoop();

        fps::Draw(bg);

        menu::Render();

        ImGui::Render();
        float clear[4] = {};
        d3d::pContext->OMSetRenderTargets(1, &d3d::pRenderTarget, nullptr);
        d3d::pContext->ClearRenderTargetView(d3d::pRenderTarget, clear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        d3d::pSwapChain->Present(vsyncEnabled ? 1 : 0, 0);
    }
    d3d::Shutdown();
}
