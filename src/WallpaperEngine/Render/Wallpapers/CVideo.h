#pragma once

#include "WallpaperEngine/Render/CWallpaper.h"

namespace WallpaperEngine::Render::Wallpapers {
class CVideo final : public CWallpaper {
public:
    CVideo (
        const Wallpaper& wallpaper, RenderContext& context, AudioContext& audioContext,
        const WallpaperState::TextureUVsScaling& scalingMode, const uint32_t& clampMode
    );
    ~CVideo () override;

    [[nodiscard]] int getWidth () const override;
    [[nodiscard]] int getHeight () const override;
    void setPause (bool newState) override;

protected:
    void renderFrame (const glm::ivec4& viewport) override;

private:
    int m_width = 0;
    int m_height = 0;
};
} // namespace WallpaperEngine::Render::Wallpapers