#pragma once
#include \"WallpaperEngine/Audio/Drivers/Recorders/PlaybackRecorder.h\"
#pragma once
namespace WallpaperEngine::Audio::Drivers::Recorders {
class PulseAudioPlaybackRecorder : public PlaybackRecorder {
public:
    PulseAudioPlaybackRecorder () = default;
    ~PulseAudioPlaybackRecorder () override = default;
};
}