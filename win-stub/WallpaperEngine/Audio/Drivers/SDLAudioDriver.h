#pragma once

#include "WallpaperEngine/Audio/Drivers/AudioDriver.h"

namespace WallpaperEngine::Audio::Drivers {
class SDLAudioDriver final : public AudioDriver {
public:
    SDLAudioDriver (
        Application::ApplicationContext& applicationContext, Detectors::AudioPlayingDetector& detector,
        Recorders::PlaybackRecorder& recorder
    );
    ~SDLAudioDriver () override;

    int addStream (AudioStream* stream) override;
    void removeStream (int streamId) override;
    int getFormat () const override;
    int getSampleRate () const override;
    int getChannels () const override;
};
}