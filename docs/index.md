# wpe-core（Windows）— Wallpaper Engine 场景渲染器

Wallpaper Engine 场景（scene）渲染内核的 **Windows 移植版**，导出为 `wpe-core.dll`，提供 C API 供任意宿主程序集成。

基于 [linux-wallpaperengine](https://github.com/Almamu/linux-wallpaperengine)（GPL-3.0）移植。

## 功能

- Wallpaper Engine 场景包（`.pkg` / 项目目录）解析与渲染
- 材质 / 粒子 / 脚本（JavaScript）支持
- GPU 抓帧导出（BGR/BGRA）
- 窗口直出模式（可选）

## 快速开始

```bash
git clone https://github.com/bl0035-awa/wpe-core-windows.git
cd wpe-core-windows
# 构建说明见 README
```

构建方法、导出的 C API 列表、依赖说明详见 [README](https://github.com/bl0035-awa/wpe-core-windows#readme)。

## 问题反馈

若遇到问题，请到 [Issues](https://github.com/bl0035-awa/wpe-core-windows/issues) 提交。

## 许可证

GPL-3.0（见 [LICENSE](https://github.com/bl0035-awa/wpe-core-windows/blob/main/LICENSE)）