# ImGui DX11 External Overlay

> ⚠️ This project is for educational and reference purposes only, and is not planned to be maintained. If you encounter any issues during use, please try to fix them yourself and do not submit issues.

[简体中文](README_CN.md) | **English**

A high-performance external overlay program based on Dear ImGui and DirectX 11. This program provides a transparent, always-on-top user interface that can be used for system monitoring, debugging tools, and various other application scenarios.

## 🌟 Features

### Core Features
- **Transparent Overlay Window**: Supports fully transparent backgrounds and can be overlaid on any application
- **DirectX 11 Rendering**: Uses modern graphics API to ensure high-performance rendering
- **Always On Top**: Window automatically stays on top and won't be blocked by other applications
- **Mouse Click-through**: Automatically adjusts whether mouse events pass through the window to avoid interfering with normal operations
- **Performance Optimization**: More performance-efficient compared to using the LWA_COLORKEY parameter of the SetLayeredWindowAttributes function for transparency

## 🚀 Quick Start

### Environment Requirements
- **Operating System**: Windows 10/11 (64-bit)
- **Runtime**: DirectX 11
- **Compiler**: Visual Studio 2019 or higher
- **Platform Toolset**: v143 (Visual Studio 2022)

### Compilation Steps

1. **Clone Project**
   ```bash
   git clone https://github.com/Cyan325zzZ/ImGui-DX11-External-Overlay.git
   ```

2. **Open with Visual Studio**
   - Double-click `An external overlay program compatible with ImGui.sln` to open the solution
   - Select `x64` platform and `Release` configuration in the toolbar
   - Compile the project

3. **Output Files**
   - The compiled executable is located at: `Builds/Overlay.exe`

### Run Program

Simply run the compiled `Overlay.exe` to see an ImGui window overlay at the top of the screen.

## 📁 Project Structure

```
ImGui-DX11-External-Overlay/
├── An external overlay program compatible with ImGui.sln    # Visual Studio Solution
├── Src/                                   # Source Code Directory
│   ├── main.cpp                          # Program Entry Point
│   ├── ui.cpp/ui.h                       # UI Interface Implementation
│   ├── function_set.cpp/function_set.h   # Core Function Implementation
│   ├── resource.h                        # Resource Definition
│   └── resources.rc                      # Resource File
├── Library/                               # Third-party Libraries
│   ├── imgui/                            # Dear ImGui Core Library
│   │   ├── imgui.cpp/.h                  # ImGui Core Implementation
│   │   ├── imgui_impl_dx11.cpp/.h       # DirectX11 Backend
│   │   ├── imgui_impl_win32.cpp/.h      # Win32 Backend
│   │   └── ...                           # Other ImGui Modules
│   └── fonts/                            # Font Files
│       └── SmileySans-Oblique-3.otf    # Smiley Sans Oblique Font
├── Builds/                               # Compilation Output Directory
├── LICENSE                               # MIT License
├── README.md                            # English README
└── README_CN.md                         # Chinese README
```

## 📝 License
This project adopts the [MIT License](LICENSE), supporting commercial, modified distribution, private, and patent use.

---
<div align="center">

### ⭐ Support the Project
If this project is helpful to you, please give it a **Star** to show support!

</div>
