#pragma once

#include "WallpaperEngine/Application/ApplicationContext.h"
#include "WallpaperEngine/Audio/Drivers/Detectors/AudioPlayingDetector.h"
#include "WallpaperEngine/Audio/Drivers/Recorders/PlaybackRecorder.h"

namespace WallpaperEngine::Application { class ApplicationContext; }
namespace WallpaperEngine::Audio {
class AudioStream;
namespace Drivers {
    namespace Detectors { class AudioPlayingDetector; }
    namespace Recorders { class PlaybackRecorder; }
}
}

namespace WallpaperEngine::Audio::Drivers {
class AudioDriver {
public:
    explicit AudioDriver (
        Application::ApplicationContext& applicationContext, Detectors::AudioPlayingDetector& detector,
        Recorders::PlaybackRecorder& recorder
    );
    virtual ~AudioDriver () = default;

    virtual int addStream (AudioStream* stream) = 0;
    virtual void removeStream (int streamId) = 0;
    virtual void update ();
    virtual int getFormat () const = 0;
    virtual int getSampleRate () const = 0;
    virtual int getChannels () const = 0;
    Application::ApplicationContext& getApplicationContext () const;
    [[nodiscard]] Detectors::AudioPlayingDetector& getAudioDetector () const;
    [[nodiscard]] Recorders::PlaybackRecorder& getRecorder () const;

private:
    Application::ApplicationContext& m_applicationContext;
    Detectors::AudioPlayingDetector& m_detector;
    Recorders::PlaybackRecorder& m_recorder;
};
}