//
// Created by coldmint on 2026/3/25.
//

#include "AudioManager.h"

#include "core/log/LogCat.h"


void glimmer::AudioManager::CreateTracks(const AudioType type, const size_t count) {
    if (track_.contains(type)) {
        for (auto *track: track_[type]) {
            MIX_DestroyTrack(track);
        }
        track_[type].clear();
    }
    std::vector<MIX_Track *> newTracks;
    for (size_t i = 0; i < count; ++i) {
        MIX_Track *track = MIX_CreateTrack(mixer_);
        if (!track) {
            LogCat::e("MIX_CreateTrack failed: ", SDL_GetError());
            continue;
        }
        newTracks.push_back(track);
    }

    track_[type] = std::move(newTracks);
}

MIX_Track *glimmer::AudioManager::GetFreeTrack(const AudioType type) {
    if (!track_.contains(type)) return nullptr;
    for (auto *track: track_[type]) {
        if (!MIX_TrackPlaying(track)) {
            return track;
        }
    }
    return nullptr;
}

glimmer::AudioManager::AudioManager() = default;

void glimmer::AudioManager::SetMixer(MIX_Mixer *mixer) {
    mixer_ = mixer;
}

void glimmer::AudioManager::PlayAudio(const AudioType audioType, MIX_Audio *audio, const int loopsNumber) {
    if (!audio) return;

    MIX_Track *track = GetFreeTrack(audioType);
    if (!track) {
        LogCat::w("No free track for audio!");
        return;
    }
    MIX_SetTrackAudio(track, audio);
    const SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loopsNumber);
    MIX_PlayTrack(track, props);
}
