#pragma once

#include "WallpaperEngine/Media/MediaSource.h"

namespace WallpaperEngine::Media {
class DBusMediaSource final : public MediaSource {
public:
    explicit DBusMediaSource (std::chrono::milliseconds updateInterval);
    ~DBusMediaSource () override;
    void update () override;
    void performUpdate () override;
};
} // namespace WallpaperEngine::Media