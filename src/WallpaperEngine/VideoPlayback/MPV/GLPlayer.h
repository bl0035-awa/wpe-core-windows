#pragma once
// Windows 移植版 stub：视频播放（MPV）不支持
#include "MemoryStreamProtocol.h"
#include <memory>
#include <cstdint>

namespace WallpaperEngine::Render { class RenderContext; class Camera; }

namespace WallpaperEngine::VideoPlayback::MPV {
class GLPlayer {
public:
    GLPlayer () = default;
    GLPlayer (Render::RenderContext& context, uint32_t outputTexture, MemoryStreamProtocolUniquePtr stream, int64_t baseWidth, int64_t baseHeight) { }
    ~GLPlayer () = default;
    void setMuted () { }
    void setVolume (float v) { }
    void setUntimed () { }
    void update () { }
    void incrementUsageCount () { }
    void decrementUsageCount () { }
    void render (Render::Camera* camera = nullptr) { }
    bool isPlaying () const { return false; }
};

using GLPlayerUniquePtr = std::unique_ptr<GLPlayer>;
using GLPlayerSharedPtr = std::shared_ptr<GLPlayer>;
} // namespace WallpaperEngine::VideoPlayback::MPV
