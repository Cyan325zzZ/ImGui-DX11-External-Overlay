#include "../Library/imgui/imgui.h"
#include "ui.h"
#include <windows.h>

void RenderUI()
{
    ImGui::Begin("Drawer panel");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    
    HWND hwnd = FindWindowW(L"ImGui Example", nullptr);
    if (hwnd)
    {
        RECT rect;
        if (GetWindowRect(hwnd, &rect))
        {
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            ImGui::Text("窗口分辨率: %d x %d", width, height);
        }
    }
    
    ImGui::Text("你好，世界!");
    ImGui::End();
}