# wpe-core（Windows）— Wallpaper Engine 场景渲染器

![License](https://img.shields.io/badge/license-GPL--3.0-blue)
![Language](https://img.shields.io/badge/language-C%2B%2B-00599C?logo=cplusplus)
![Platform](https://img.shields.io/badge/platform-Windows-0078D6?logo=windows)
![Build](https://img.shields.io/badge/build-CMake-064F8C?logo=cmake)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3%2B-5586A4?logo=opengl)


Wallpaper Engine 场景（scene）渲染内核的 Windows 移植版，导出为 `wpe-core.dll`，提供 C API 供任意宿主程序（启动器、桌面工具等）集成。

基于 [linux-wallpaperengine](https://github.com/Almamu/linux-wallpaperengine)（GPL-3.0）移植。

## 组件

| 文件 | 说明 |
|---|---|
| `src/` | 渲染器源码（场景解析 / 渲染 / 材质 / 粒子） |
| `src/wpe_api.cpp` | 导出的 C API（创建 / 渲染 / 抓帧 / 窗口模式） |
| `win-stub/` | Windows 平台桩（替代 Linux 依赖） |
| `3rdparty/` | 第三方依赖（glfw / glew / SDL2 / freetype / zlib / lz4 / glm） |
| `CMakeLists.txt` | CMake 构建脚本（Windows 分支） |
| `wpe-core.rc` | DLL 版本信息 |

## 构建（Windows）

前置：Visual Studio 2022 Build Tools（MSVC）、CMake 3.20+、Windows SDK 10

```bat
set PATH=C:\Program Files\CMake\bin;%PATH%
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set LIB=%LIB%;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x64
cmake -S . -B build-win -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DENABLE_OPT=OFF
cmake --build build-win --config Release --parallel 8
```

产物：`build-win/Release/wpe-core.dll`

## 导出的 C API

```c
WpeHandle* wpe_create(const char* projectPath, int width, int height);
void       wpe_render(WpeHandle* handle);
bool       wpe_copy_frame(WpeHandle* handle, void* buffer, size_t size);
int        wpe_get_width(WpeHandle* handle);
int        wpe_get_height(WpeHandle* handle);
unsigned   wpe_get_wallpaper_fbo(WpeHandle* handle);
int        wpe_get_wallpaper_width(WpeHandle* handle);
int        wpe_get_wallpaper_height(WpeHandle* handle);
void       wpe_destroy(WpeHandle* handle);

/* 窗口直出模式（可选） */
WpeHandle* wpe_create_windowed(const char* projectPath, void* parentHwnd, int x, int y, int w, int h);
void       wpe_render_windowed(WpeHandle* handle);
void       wpe_move_window(WpeHandle* handle, int x, int y, int w, int h);
void       wpe_set_window_visible(WpeHandle* handle, bool visible);
```

## 运行依赖

`glew32.dll`、`glfw3.dll`、`SDL2.dll`、`freetype.dll` 需与 `wpe-core.dll` 同目录（或系统 PATH）。

## 已知限制

> **AI 生成声明**
>
> 本仓库的 Windows 移植代码（平台适配、构建配置、C API 封装等）由 AI 辅助生成与修改，可能存在未覆盖的边界情况。使用前请自行审查代码，并在目标环境充分测试。
>
> 原始渲染器内核来自 [linux-wallpaperengine](https://github.com/Almamu/linux-wallpaperengine)（GPL-3.0），非 AI 生成。
- 需在支持 OpenGL 3.3+ 的显卡上运行
- 含中文文件名的场景包（pkg）解析可能失败
- 60fps 上限受场景复杂度与显卡性能影响

## 贡献

见 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 许可证

- 本项目遵循 **GPL-3.0**（见 `LICENSE`，移植自 linux-wallpaperengine）
- 第三方组件许可证见 [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md)

- 移植自 linux-wallpaperengine（**GPL-3.0**），本仓库同样遵循 GPL-3.0
- Wallpaper Engine 场景格式解析为兼容实现，需自行准备合法来源的场景文件