# ImGui DX11 外部覆盖程序

> ⚠️ 本项目仅作学习与参考之用，后续不计划进行维护。若使用过程中遇到任何问题，请自行尝试修复，请勿提交 issue。

**简体中文** | [English](README.md)

一个基于Dear ImGui和DirectX 11的高性能外部覆盖程序。该程序提供了一个透明的、始终置顶的用户界面，可用于系统监控、调试工具等多种应用场景。

## 🌟 特性

### 核心功能
- **透明覆盖窗口**：支持完全透明的背景，可与任何应用程序叠加显示
- **DirectX 11渲染**：使用现代图形API，确保高性能渲染
- **始终置顶**：窗口自动保持在最顶层，不会被其他应用遮挡
- **鼠标穿透**：自动调整鼠标事件是否穿透窗口，避免干扰正常操作
- **性能优化**：相比于使用 SetLayeredWindowAttributes 函数的 LWA_COLORKEY 参数实现透明的方式更加节约性能开销

## 🚀 快速开始

### 环境要求
- **操作系统**：Windows 10/11 (64位)
- **运行时**：DirectX 11
- **编译器**：Visual Studio 2019 或更高版本
- **平台工具集**：v143 (Visual Studio 2022)

### 编译步骤

1. **克隆项目**
   ```bash
   git clone https://github.com/Cyan325zzZ/ImGui-DX11-External-Overlay.git
   ```

2. **使用Visual Studio打开**
   - 双击 `An external overlay program compatible with ImGui.sln` 打开解决方案
   - 在工具栏选择 `x64` 平台和 `Release` 配置
   - 编译项目

3. **输出文件**
   - 编译后的可执行文件位于：`Builds/ImGui-Overlay.exe`

### 运行程序

直接运行编译好的 `ImGui-Overlay.exe` 即可看到一个ImGui窗口覆盖在屏幕上方。

## 📁 项目结构

```
ImGui-DX11-External-Overlay/
├── An external overlay program compatible with ImGui.sln    # Visual Studio解决方案
├── Src/                                   # 源代码目录
│   ├── main.cpp                          # 程序入口点
│   ├── ui.cpp/ui.h                       # UI界面实现
│   ├── function_set.cpp/function_set.h   # 核心功能实现
│   ├── resource.h                        # 资源定义
│   └── resources.rc                      # 资源文件
├── Library/                               # 第三方库
│   ├── imgui/                            # Dear ImGui核心库
│   │   ├── imgui.cpp/.h                  # ImGui核心实现
│   │   ├── imgui_impl_dx11.cpp/.h       # DirectX11后端
│   │   ├── imgui_impl_win32.cpp/.h      # Win32后端
│   │   └── ...                           # 其他ImGui模块
│   └── fonts/                            # 字体文件
│       └── SmileySans-Oblique-3.otf    # 得意黑字体
├── Builds/                               # 编译输出目录
├── LICENSE                               # MIT许可证
├── README.md                            # 英文自述文件
└── README_CN.md                         # 中文自述文件
```

## 📝 许可证
本项目采用 [MIT许可证](LICENSE)，支持商业、修改分发、私人及专利使用。

---
<div align="center">

### ⭐ 支持项目
如果本项目对你有帮助，欢迎给个 **Star** 支持一下！

</div>
