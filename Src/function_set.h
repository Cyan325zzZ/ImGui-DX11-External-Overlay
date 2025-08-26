#pragma once
#include <windows.h>
#include <d3d11.h>
#include "../Library/imgui/imgui.h"

// 全局变量声明 | Global variable declarations
extern ID3D11Device*            g_pd3dDevice;
extern ID3D11DeviceContext*     g_pd3dDeviceContext;
extern IDXGISwapChain*          g_pSwapChain;
extern bool                     g_SwapChainOccluded;
extern UINT                     g_ResizeWidth, g_ResizeHeight;
extern ID3D11RenderTargetView*  g_mainRenderTargetView;

extern bool                     g_WindowTransparent;

// ==========================================
// D3D设备相关函数 | D3D device related functions
// ==========================================

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

// ==========================================
// 窗口相关函数 | Window related functions
// ==========================================

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND InitializeWindow(HINSTANCE hInstance, const wchar_t* title, int width, int height, float* dpi_scale = nullptr);

// ==========================================
// ImGui初始化和渲染函数 | ImGui initialization and rendering functions
// ==========================================

bool InitializeImGui(HWND hwnd, float dpi_scale);
void ShutdownImGui();
void HandleWindowMessages(bool& done);
void RenderFrame(const ImVec4& clear_color);

// ==========================================
// 字体加载函数 | Font loading functions
// ==========================================

void LoadCustomFont(ImGuiIO& io);

// ==========================================
// 透明度处理函数 | Transparency handling functions
// ==========================================

void HandleWindowTransparency(HWND hwnd);