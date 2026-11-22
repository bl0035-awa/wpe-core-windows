#pragma once

namespace WallpaperEngine::Application { class WallpaperApplication; }

namespace WallpaperEngine::WebBrowser {
class WebBrowserContext {
public:
    explicit WebBrowserContext (Application::WallpaperApplication& wallpaperApplication);
    ~WebBrowserContext ();
private:
    Application::WallpaperApplication& m_wallpaperApplication;
};
} // namespace WallpaperEngine::WebBrowser