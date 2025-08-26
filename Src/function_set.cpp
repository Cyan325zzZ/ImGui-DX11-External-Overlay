#include "function_set.h"
#include "resource.h"
#include "ui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx11.h"
#include "../Library/imgui/imgui.h"
#include <windows.h>
#include <d3d11.h>

// 全局变量 | Global variables
bool g_WindowTransparent = true;

ID3D11Device*            g_pd3dDevice = nullptr;
ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
IDXGISwapChain*          g_pSwapChain = nullptr;
bool                     g_SwapChainOccluded = false;
UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

// ==========================================
// D3D设备相关函数 | D3D device related functions
// ==========================================

/**
 * @brief 创建Direct3D 11设备和交换链
 * 
 * 实现原理：
 * 1. 配置DXGI_SWAP_CHAIN_DESC结构体，设置交换链参数：
 *    - 双缓冲机制（BufferCount=2）减少画面撕裂
 *    - 设置像素格式为DXGI_FORMAT_R8G8B8A8_UNORM（32位RGBA）
 *    - 启用窗口模式切换标志ALLOW_MODE_SWITCH
 *    - 设置刷新率为60Hz
 * 2. 使用D3D11CreateDeviceAndSwapChain创建设备和交换链：
 *    - 首先尝试硬件加速（D3D_DRIVER_TYPE_HARDWARE）
 *    - 如果硬件不支持，回退到WARP软件渲染（D3D_DRIVER_TYPE_WARP）
 *    - 支持DirectX 11.0和10.0特性级别
 * 3. 创建渲染目标视图用于后续渲染
 * 
 * @param hWnd 窗口句柄，用于关联交换链
 * @return 成功返回true，失败返回false
 */
bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

/**
 * @brief 清理D3D设备和资源
 * 
 * 实现原理：
 * 1. 按逆序释放资源，避免内存泄漏：
 *    - 先清理渲染目标视图
 *    - 再释放交换链
 *    - 然后释放设备上下文
 *    - 最后释放设备本身
 * 2. 使用COM对象的Release方法，遵循COM规范
 * 3. 释放后将指针设为nullptr，避免悬垂指针
 */
void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

/**
 * @brief 创建渲染目标视图
 * 
 * 实现原理：
 * 1. 从交换链获取后台缓冲区纹理（GetBuffer）
 * 2. 配置D3D11_RENDER_TARGET_VIEW_DESC结构体：
 *    - 设置格式与后台缓冲区一致
 *    - 使用2D纹理视图维度
 * 3. 使用设备创建渲染目标视图（CreateRenderTargetView）
 * 4. 释放后台缓冲区纹理引用，避免内存泄漏
 * 5. 错误处理：如果创建失败，将渲染目标视图设为nullptr
 */
void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    HRESULT hr = g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr) || pBackBuffer == nullptr)
    {
        return;
    }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, &rtvDesc, &g_mainRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
    {
        g_mainRenderTargetView = nullptr;
    }
}

/**
 * @brief 清理渲染目标视图
 * 
 * 实现原理：
 * 1. 检查渲染目标视图是否存在
 * 2. 使用COM的Release方法释放资源
 * 3. 将指针设为nullptr，避免悬垂指针
 */
void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// ==========================================
// 窗口相关函数 | Window related functions
// ==========================================

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * @brief 窗口过程函数，处理窗口消息
 * 
 * 实现原理：
 * 1. 首先将消息传递给ImGui处理，确保ImGui能正确处理输入事件
 * 2. 根据消息类型进行相应处理：
 *    - WM_SIZE：处理窗口大小变化，记录新的宽高
 *    - WM_SYSCOMMAND：阻止系统菜单激活（Alt键）
 *    - WM_NCHITTEST：根据透明度设置决定是否让鼠标事件穿透窗口
 *    - WM_WINDOWPOSCHANGING：强制窗口置顶显示
 *    - WM_DESTROY：处理窗口关闭消息，发送退出消息
 * 3. 未处理的消息调用默认窗口过程
 * 
 * @param hWnd 窗口句柄
 * @param msg 消息类型
 * @param wParam 附加参数
 * @param lParam 附加参数
 * @return 消息处理结果
 */
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_NCHITTEST:
        if (g_WindowTransparent) {
            return HTTRANSPARENT;
        }
        else {
            break;
        }
    case WM_WINDOWPOSCHANGING:
        ((LPWINDOWPOS)lParam)->hwndInsertAfter = HWND_TOPMOST;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

/**
 * @brief 初始化窗口
 * 
 * 实现原理：
 * 1. 启用DPI感知，确保在高DPI显示器上正确显示
 * 2. 获取主显示器的DPI缩放比例，用于适配不同分辨率的屏幕
 * 3. 注册窗口类：
 *    - 设置类样式为CS_CLASSDC（类设备上下文）
 *    - 关联窗口过程函数WndProc
 * 4. 创建无边框弹出式窗口：
 *    - 使用WS_EX_TRANSPARENT和WS_EX_LAYERED扩展样式实现透明效果
 *    - 窗口大小根据DPI缩放比例调整
 * 5. 设置窗口属性：
 *    - 设置透明度为255（完全不透明）
 *    - 设置窗口置顶显示
 * 
 * @param hInstance 应用程序实例句柄
 * @param title 窗口标题
 * @param width 窗口宽度
 * @param height 窗口高度
 * @param dpi_scale DPI缩放比例指针，用于返回计算后的缩放值
 * @return 创建成功的窗口句柄
 */
HWND InitializeWindow(HINSTANCE hInstance, const wchar_t* title, int width, int height, float* dpi_scale)
{
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
    
    if (dpi_scale) *dpi_scale = main_scale;

    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    
    int scaled_width = (int)(width * main_scale);
    int scaled_height = (int)(height * main_scale);
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    int pos_x = (screen_width - scaled_width) / 2;
    int pos_y = (screen_height - scaled_height) / 2;
    
    HWND hwnd = ::CreateWindowExW(WS_EX_TRANSPARENT | WS_EX_LAYERED, wc.lpszClassName, title, 
                                   WS_POPUP, pos_x, pos_y, 
                                   scaled_width, scaled_height, 
                                   nullptr, nullptr, wc.hInstance, nullptr);
    
    ::SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    ::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    
    return hwnd;
}

// ==========================================
// ImGui初始化和渲染函数 | ImGui initialization and rendering functions
// ==========================================

/**
 * @brief 初始化ImGui
 * 
 * 实现原理：
 * 1. 创建D3D11设备作为渲染后端
 * 2. 启用窗口的Alpha合成功能，支持透明背景
 * 3. 显示并更新窗口
 * 4. 创建ImGui上下文：
 *    - 检查版本兼容性
 *    - 禁用ini文件保存（io.IniFilename = nullptr）
 * 5. 设置ImGui样式：
 *    - 使用深色主题
 *    - 根据DPI缩放调整所有UI元素大小
 * 6. 初始化平台相关的后端：
 *    - Win32平台输入处理
 *    - D3D11渲染后端
 * 7. 加载自定义字体（中文字体）
 * 
 * @param hwnd 窗口句柄
 * @param dpi_scale DPI缩放比例
 * @return 初始化成功返回true，失败返回false
 */
bool InitializeImGui(HWND hwnd, float dpi_scale)
{
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        return false;
    }

    ImGui_ImplWin32_EnableAlphaCompositing(hwnd);
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.ScaleAllSizes(dpi_scale);
    style.FontScaleDpi = dpi_scale;

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    LoadCustomFont(io);
    
    return true;
}

/**
 * @brief 关闭ImGui并清理资源
 * 
 * 实现原理：
 * 1. 按正确顺序清理ImGui相关资源：
 *    - 先清理D3D11渲染后端
 *    - 再清理Win32平台后端
 *    - 最后销毁ImGui上下文
 * 2. 清理D3D设备资源（调用CleanupDeviceD3D）
 * 3. 遵循后进先出的清理顺序，避免资源泄漏
 */
void ShutdownImGui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
}

/**
 * @brief 处理窗口消息
 * 
 * 实现原理：
 * 1. 使用PeekMessage非阻塞方式获取消息队列中的消息
 * 2. 使用TranslateMessage和DispatchMessage处理消息
 * 3. 检查是否为WM_QUIT消息，如果是则设置退出标志
 * 4. 循环处理所有待处理的消息，确保消息及时处理
 * 
 * @param done 退出标志引用，收到WM_QUIT时设为true
 */
void HandleWindowMessages(bool& done)
{
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            done = true;
    }
}

/**
 * @brief 渲染一帧画面
 * 
 * 实现原理：
 * 1. 处理窗口遮挡状态：
 *    - 检查交换链是否被遮挡（DXGI_STATUS_OCCLUDED）
 *    - 如果被遮挡，短暂休眠后返回
 * 2. 处理窗口大小变化：
 *    - 检查是否需要调整缓冲区大小
 *    - 清理旧渲染目标，调整缓冲区大小，重新创建渲染目标
 * 3. 开始新的ImGui帧：
 *    - 更新D3D11后端
 *    - 更新Win32后端
 *    - 开始新的ImGui帧
 * 4. 渲染UI：
 *    - 调用RenderUI()渲染实际UI内容
 *    - 提交ImGui绘制数据
 * 5. 执行渲染：
 *    - 设置渲染目标
 *    - 清除渲染目标为指定颜色
 *    - 执行ImGui绘制命令
 * 6. 呈现画面：
 *    - 调用Present显示渲染结果
 *    - 更新遮挡状态标志
 * 
 * @param clear_color 清除颜色（RGBA格式）
 */
void RenderFrame(const ImVec4& clear_color)
{
    if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
    {
        CleanupRenderTarget();
        g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
        g_ResizeWidth = g_ResizeHeight = 0;
        CreateRenderTarget();
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    RenderUI();
    ImGui::Render();

    const float clear_color_with_alpha[4] = { clear_color.x, clear_color.y, clear_color.z, clear_color.w };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(1, 0);
}

// ==========================================
// 字体加载函数 | Font loading functions
// ==========================================

/**
 * @brief 从资源加载自定义字体
 * 
 * 实现原理：
 * 1. 使用Windows资源API查找字体资源：
 *    - FindResourceA：查找指定ID的字体资源
 *    - SizeofResource：获取资源大小
 *    - LoadResource：加载资源到内存
 *    - LockResource：获取资源数据指针
 * 2. 将字体数据复制到ImGui管理的内存：
 *    - 使用IM_ALLOC分配内存
 *    - 使用memcpy复制字体数据
 * 3. 使用ImGui的AddFontFromMemoryTTF从内存加载字体：
 *    - 设置字体大小为16.0f
 *    - 使用GetGlyphRangesChineseFull获取中文字符范围
 * 4. 错误处理：如果资源查找或加载失败，直接返回
 * 
 * @param io ImGuiIO引用，用于添加字体
 */
void LoadCustomFont(ImGuiIO& io)
{
    HRSRC hRes = FindResourceA(NULL, MAKEINTRESOURCEA(IDR_FONT_SMILEY), "MYFONT");
    if (!hRes) return;

    DWORD dataSize = SizeofResource(nullptr, hRes);
    HGLOBAL hGlobal = LoadResource(nullptr, hRes);
    void* pData = hGlobal ? LockResource(hGlobal) : nullptr;
    if (!pData || !dataSize) return;

    void* fontDataCopy = IM_ALLOC(dataSize);
    if (fontDataCopy) {
        memcpy(fontDataCopy, pData, dataSize);
        io.Fonts->AddFontFromMemoryTTF(fontDataCopy, dataSize, 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    }
}

// ==========================================
// 透明度处理函数 | Transparency handling functions
// ==========================================

/**
 * @brief 处理窗口透明度，实现鼠标悬停时显示/隐藏
 * 
 * 实现原理：
 * 1. 获取当前鼠标位置和窗口矩形区域
 * 2. 计算鼠标相对于窗口的相对坐标，更新ImGui的鼠标位置
 * 3. 判断鼠标是否悬停在窗口上：
 *    - 检查鼠标是否在窗口矩形内
 *    - 检查是否有ImGui窗口或控件被悬停
 * 4. 根据悬停状态动态调整窗口透明度：
 *    - 鼠标悬停时：移除WS_EX_TRANSPARENT样式，窗口可交互
 *    - 鼠标未悬停时：添加WS_EX_TRANSPARENT样式，鼠标事件穿透窗口
 * 5. 使用SetWindowLongPtr修改窗口扩展样式
 * 6. 缓存上次的透明度状态，避免重复设置
 * 
 * @param hwnd 窗口句柄
 */
void HandleWindowTransparency(HWND hwnd)
{
    static bool lastTransparent = true;
    POINT mousePos;
    RECT windowRect;

    if (!::GetCursorPos(&mousePos) || !::GetWindowRect(hwnd, &windowRect))
        return;

    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)(mousePos.x - windowRect.left), (float)(mousePos.y - windowRect.top));

    bool isHovered = (mousePos.x >= windowRect.left && mousePos.x <= windowRect.right &&
                     mousePos.y >= windowRect.top && mousePos.y <= windowRect.bottom) &&
                    (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByPopup) || 
                     ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) || 
                     ImGui::IsAnyItemHovered());

    bool shouldBeTransparent = !isHovered;
    if (shouldBeTransparent != lastTransparent)
    {
        ::SetWindowLongPtr(hwnd, GWL_EXSTYLE, 
                          shouldBeTransparent ? (WS_EX_TRANSPARENT | WS_EX_LAYERED) : WS_EX_LAYERED);
        ::SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
        lastTransparent = shouldBeTransparent;
        g_WindowTransparent = shouldBeTransparent;
    }
}