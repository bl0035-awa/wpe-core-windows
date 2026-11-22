// Windows 移植版 stub：Linux 专用平台层（音频/网页/DBus/视频墙纸）的空实现
#include <chrono>
#include <cstdint>

#include "WallpaperEngine/Audio/AudioContext.h"
#include "WallpaperEngine/Audio/AudioStream.h"
#include "WallpaperEngine/Audio/Drivers/AudioDriver.h"
#include "WallpaperEngine/Audio/Drivers/SDLAudioDriver.h"
#include "WallpaperEngine/WebBrowser/WebBrowserContext.h"
#include "WallpaperEngine/Media/DBusMediaSource.h"
#include "WallpaperEngine/Render/Objects/CSound.h"
#include "WallpaperEngine/Render/Wallpapers/CVideo.h"
#include "WallpaperEngine/Render/Wallpapers/CWeb.h"

namespace WallpaperEngine::Audio {

// ---- AudioContext ----
AudioContext::AudioContext (Drivers::AudioDriver& driver) : m_driver (driver) { }
AudioContext::~AudioContext () = default;

int AudioContext::addStream (AudioStream* stream) const { return 0; }
void AudioContext::removeStream (int streamId) const { }
int AudioContext::getFormat () const { return 0; }
int AudioContext::getSampleRate () const { return 48000; }
int AudioContext::getChannels () const { return 2; }
Application::ApplicationContext& AudioContext::getApplicationContext () const {
    return this->m_driver.getApplicationContext ();
}
Drivers::Recorders::PlaybackRecorder& AudioContext::getRecorder () const { return this->m_driver.getRecorder (); }
Drivers::AudioDriver& AudioContext::getDriver () const { return this->m_driver; }

namespace Drivers {

// AudioDriver 实现见 src/WallpaperEngine/Audio/Drivers/AudioDriver.cpp

// ---- SDLAudioDriver ----
SDLAudioDriver::SDLAudioDriver (
    Application::ApplicationContext& applicationContext, Detectors::AudioPlayingDetector& detector,
    Recorders::PlaybackRecorder& recorder
) : AudioDriver (applicationContext, detector, recorder) { }
SDLAudioDriver::~SDLAudioDriver () = default;
int SDLAudioDriver::addStream (AudioStream* stream) { return 0; }
void SDLAudioDriver::removeStream (int streamId) { }
int SDLAudioDriver::getFormat () const { return 0; }
int SDLAudioDriver::getSampleRate () const { return 48000; }
int SDLAudioDriver::getChannels () const { return 2; }

} // namespace Drivers
} // namespace WallpaperEngine::Audio

// ---- WebBrowserContext ----
namespace WallpaperEngine::WebBrowser {
WebBrowserContext::WebBrowserContext (Application::WallpaperApplication& wallpaperApplication)
    : m_wallpaperApplication (wallpaperApplication) { }
WebBrowserContext::~WebBrowserContext () = default;
} // namespace WallpaperEngine::WebBrowser

// ---- DBusMediaSource ----
namespace WallpaperEngine::Media {
DBusMediaSource::DBusMediaSource (std::chrono::milliseconds updateInterval) : MediaSource (updateInterval) { }
DBusMediaSource::~DBusMediaSource () = default;
void DBusMediaSource::update () { }
void DBusMediaSource::performUpdate () { }
} // namespace WallpaperEngine::Media

// ---- CSound ----
namespace WallpaperEngine::Render::Objects {
CSound::CSound (Wallpapers::CScene& scene, const Sound& sound) : CObject (scene, sound), m_sound (sound) { }
CSound::~CSound () = default;
void CSound::render () { }
void CSound::load () { }
} // namespace WallpaperEngine::Render::Objects

// ---- CVideo ----
namespace WallpaperEngine::Render::Wallpapers {
CVideo::CVideo (
    const Wallpaper& wallpaper, RenderContext& context, AudioContext& audioContext,
    const WallpaperState::TextureUVsScaling& scalingMode, const uint32_t& clampMode
) : CWallpaper (wallpaper, context, audioContext, scalingMode, clampMode) { }
CVideo::~CVideo () = default;
int CVideo::getWidth () const { return 0; }
int CVideo::getHeight () const { return 0; }
void CVideo::setPause (bool newState) { }
void CVideo::renderFrame (const glm::ivec4& viewport) { }

// ---- CWeb ----
CWeb::CWeb (
    const Wallpaper& wallpaper, RenderContext& context, AudioContext& audioContext,
    WebBrowser::WebBrowserContext& browserContext,
    const WallpaperState::TextureUVsScaling& scalingMode, const uint32_t& clampMode
) : CWallpaper (wallpaper, context, audioContext, scalingMode, clampMode) { }
CWeb::~CWeb () = default;
int CWeb::getWidth () const { return 0; }
int CWeb::getHeight () const { return 0; }
void CWeb::setSize (int width, int height) { m_width = width; m_height = height; }
void CWeb::updateMouse (const glm::ivec4& viewport) { }
void CWeb::renderFrame (const glm::ivec4& viewport) { }
} // namespace WallpaperEngine::Render::Wallpapers