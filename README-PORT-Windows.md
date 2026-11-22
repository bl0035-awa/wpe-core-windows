# Windows 移植进度（linux-wallpaperengine → Windows 场景渲染）

## 已完成（2026-11-22）
1. **构建环境**：CMake 4.4 + MSVC 14.44（VS BuildTools C++）+ vcpkg（短路径 C:\vcpkg，解决 D8050）
2. **依赖**：GLFW 3.4（lib-vc2022）/ GLEW 2.2 / SDL2 2.30.9 / FreeType 预编译；zlib 1.3.1 / lz4 1.10.0 / glm 1.0.1 源码；glslang/SPIRV-Cross/kissfft/argparse/quickjs 子模块
3. **CMakeLists.txt**：Windows 分支（CMakeLists-linux.txt 保存原版）；链接脚本 `scripts/build-win.bat` 思路
4. **编译成功**：`build-win/Release/wpe-core.dll`（11.7MB，WINDOWS_EXPORT_ALL_SYMBOLS 导出 .lib）+ wallpaperengine-win.exe
5. **源码移植修改**（关键）：
   - GLFW driver：注册键 "windows"、手动调用 registerGLFWOpenGLDriver()、windows.h、Sleep、去 X11 expose
   - Steam/FileSystem：USERPROFILE、Windows workshop 路径、绝对路径直读
   - Container：resolveAdapterForFile 用 generic_string()（修复 Windows 反斜杠无限递归）
   - PackageAdapter：generic_string 比较路径
   - stubs.cpp + win-stub 头：Audio（AudioContext/AudioDriver/SDLAudioDriver/PlaybackRecorder/AudioPlayingDetector）、WebBrowserContext、DBusMediaSource、CSound、CVideo、CWeb、GLPlayer/MemoryStreamProtocol（MPV stub）
   - 其他：TypeCaster <typeinfo>、CallStack stub、unistd/poll/sys 头 ifdef、GNU ?: 修复、stbi path.string()、SIGKILL ifdef、CTexture MPV ifdef
6. **运行验证**：
   - 程序启动 ✓ 项目 JSON 解析 ✓ pkg 内文件读取 ✓
   - setupOutput（GLFW 驱动创建）✓ setupAudio ✓ prepareOutputs 进行中
   - **剩余错误**：`Cannot decompress texture data, LZ4_decompress_safe returned an error`（.tex 纹理解压）

## 下一步（待继续）
1. 修纹理 LZ4 解码：对比 WE .tex 格式（TEXB0001-0004）与 Linux 行为；检查 `uncompressedSize/compressedSize` 与数据前缀；或改用 LZ4_decompress_safe_usingDict / 检查 pkg 内数据偏移
2. 渲染验证：`--screenshot out.png --screenshot-delay 3 -w 0x0x1280x720 <project.json>`
3. 抓帧导出接口（DLL C API）

## 复现命令
```bat
cd /d "<你的工程目录>"
set PATH=C:\Program Files\CMake\bin;%PATH%
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set LIB=%LIB%;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x64
cmake -S . -B build-win -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DENABLE_OPT=OFF
cmake --build build-win --config Release --parallel 8
```
运行：`set HOME=%USERPROFILE%` 后 `build-win\Release\wallpaperengine-win.exe --dump-structure <project.json>`
