#pragma once

#include "WallpaperEngine/Application/ApplicationContext.h"

namespace WallpaperEngine::Application { class ApplicationContext; }
namespace WallpaperEngine::Audio { class AudioStream; }

namespace WallpaperEngine::Audio {
namespace Drivers {
    class AudioDriver;
    namespace Recorders { class PlaybackRecorder; }
}

class AudioContext {
public:
    explicit AudioContext (Drivers::AudioDriver& driver);
    ~AudioContext ();

    int addStream (AudioStream* stream) const;
    void removeStream (int streamId) const;
    int getFormat () const;
    int getSampleRate () const;
    int getChannels () const;
    Application::ApplicationContext& getApplicationContext () const;
    Drivers::Recorders::PlaybackRecorder& getRecorder () const;
    Drivers::AudioDriver& getDriver () const;

private:
    Drivers::AudioDriver& m_driver;
};
} // namespace WallpaperEngine::Audio