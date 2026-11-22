#pragma once
#include "WallpaperEngine/Audio/Drivers/Detectors/AudioPlayingDetector.h"
namespace WallpaperEngine::Audio::Drivers::Detectors {
class PulseAudioPlayingDetector : public AudioPlayingDetector {
public:
    PulseAudioPlayingDetector (Application::ApplicationContext& appContext, const Render::Drivers::Detectors::FullScreenDetector& fs)
        : AudioPlayingDetector (appContext, fs) { }
    ~PulseAudioPlayingDetector () override = default;
};
}