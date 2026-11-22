# 贡献指南

感谢你对 wpe-core（Windows 版 Wallpaper Engine 场景渲染器）的关注！

## 提交 Issue

请尽量包含：
- 系统版本（Windows 10/11、显卡型号、驱动版本）
- `wpe-core.dll` 版本
- 复现步骤 + 日志/截图
- 使用的场景文件来源（不需要上传版权文件，说明类型即可）

## 提交 PR

1. Fork 本仓库
2. 创建分支：`fix/xxx` 或 `feat/xxx`
3. 保持改动最小、可编译（VS 2022 + CMake，构建方法见 README）
4. 提交前请确认代码风格与现有代码一致（`.clang-format`）
5. PR 描述写清楚：改了什么、为什么改、如何验证

## 注意事项

- 本项目**不包含**任何 Wallpaper Engine 官方资源/场景文件
- 请勿提交受版权保护的素材
- 代码需兼容 GPL-3.0（本仓库许可证）

## 开发环境

- Visual Studio 2022 Build Tools（MSVC 14.4x）
- CMake 3.20+
- Windows SDK 10.0.26100+