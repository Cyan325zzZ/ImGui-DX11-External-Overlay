#include "../Library/imgui/imgui.h"
#include "ui.h"
#include "function_set.h"
#include <windows.h>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    float dpi_scale = 1.0f;
    
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    
    HWND hwnd = InitializeWindow(hInstance, L"ImGui 外部覆盖 | ImGui external overlay", screen_width, screen_height, &dpi_scale);
    
    if (!InitializeImGui(hwnd, dpi_scale))
    {
        ::UnregisterClassW(L"ImGui Example", GetModuleHandle(nullptr));
        return 1;
    }

    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    bool done = false;

    while (!done)
    {
        HandleWindowMessages(done);
        if (done) break;

        RenderFrame(clear_color);
        HandleWindowTransparency(hwnd);
    }

    ShutdownImGui();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(L"ImGui Example", GetModuleHandle(nullptr));

    return 0;
}
