#pragma once
// Windows 移植版 stub：视频播放（MPV）不支持
#include <cstdint>
#include <memory>

namespace WallpaperEngine::VideoPlayback::MPV {
class MemoryStreamProtocol {
public:
    MemoryStreamProtocol (const void* data, uint32_t size) { }
    ~MemoryStreamProtocol () = default;
};
using MemoryStreamProtocolUniquePtr = std::unique_ptr<MemoryStreamProtocol>;
} // namespace WallpaperEngine::VideoPlayback::MPV
