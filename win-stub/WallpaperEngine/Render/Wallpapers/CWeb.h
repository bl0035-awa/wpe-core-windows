#pragma once

#include "WallpaperEngine/Render/CWallpaper.h"
#include "WallpaperEngine/WebBrowser/WebBrowserContext.h"

namespace WallpaperEngine::Render::Wallpapers {
class CWeb final : public CWallpaper {
public:
    CWeb (
        const Wallpaper& wallpaper, RenderContext& context, AudioContext& audioContext,
        WebBrowser::WebBrowserContext& browserContext,
        const WallpaperState::TextureUVsScaling& scalingMode, const uint32_t& clampMode
    );
    ~CWeb () override;

    [[nodiscard]] int getWidth () const override;
    [[nodiscard]] int getHeight () const override;
    void setSize (int width, int height);
    void updateMouse (const glm::ivec4& viewport);

protected:
    void renderFrame (const glm::ivec4& viewport) override;

private:
    int m_width = 0;
    int m_height = 0;
};
} // namespace WallpaperEngine::Render::Wallpapers