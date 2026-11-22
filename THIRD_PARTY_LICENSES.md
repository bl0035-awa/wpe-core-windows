# 第三方组件许可证

本项目（wpe-core）基于 [linux-wallpaperengine](https://github.com/Almamu/linux-wallpaperengine)（GPL-3.0）移植，并使用以下第三方组件：

## 直接依赖（3rdparty/）

| 组件 | 许可证 | 说明 |
|---|---|---|
| GLFW | zlib/libpng | 窗口/输入（glfw-3.4.bin.WIN64） |
| GLEW | MIT / BSD | OpenGL 扩展加载（glew-2.2.0） |
| SDL2 | zlib | 跨平台底层（SDL2-2.30.9） |
| FreeType | FTL / GPLv2 | 字体渲染（freetype-windows-binaries） |
| zlib | zlib | 压缩（zlib-1.3.1） |
| LZ4 | BSD-2-Clause | 压缩（lz4-1.9.4 / 1.10.0） |
| GLM | MIT | 数学库（glm-1.0.1） |

## 源码内嵌组件（src/External/）

| 组件 | 许可证 |
|---|---|
| glslang-WallpaperEngine | BSD-3-Clause / MIT |
| SPIRV-Cross-WallpaperEngine | Apache-2.0 |
| nlohmann/json | MIT |
| stb | MIT / Public Domain |
| MimeTypes | MIT |
| kissfft | BSD-3-Clause |
| argparse | MIT |
| Catch2 | BSL-1.0 |
| quickjs | MIT |

## 许可证文本位置

- 各组件目录内保留原始 `LICENSE` / `COPYING` 文件
- 本项目整体遵循 **GPL-3.0**（见根目录 `LICENSE`）

> 注：GPL-3.0 与上述所有许可证兼容。