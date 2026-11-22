#pragma once

#include "WallpaperEngine/Render/CObject.h"

namespace WallpaperEngine::Render::Wallpapers { class CScene; }

namespace WallpaperEngine::Render::Objects {
using namespace WallpaperEngine::Data::Model;

class CSound final : public CObject {
public:
    CSound (Wallpapers::CScene& scene, const Sound& sound);
    ~CSound () override;
    void render () override;
protected:
    void load ();
private:
    const Sound& m_sound;
};
} // namespace WallpaperEngine::Render::Objects