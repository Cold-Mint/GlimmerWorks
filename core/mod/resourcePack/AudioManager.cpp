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
        MIX_TagTrack(track, AudioTypeToTag(type));
        newTracks.push_back(track);
    }

    track_[type] = std::move(newTracks);
}

MIX_Track *glimmer::AudioManager::GetFreeTrack(const AudioType type) {
    if (!track_.contains(type)) {
        return nullptr;
    }
    for (auto *track: track_[type]) {
        if (!MIX_TrackPlaying(track)) {
            return track;
        }
    }
    return nullptr;
}

const char *glimmer::AudioManager::AudioTypeToTag(const AudioType type) {
    switch (type) {
        case BGM: return "BGM";
        case AMBIENT: return "AMBIENT";
        default: return "UNKNOWN";
    }
}

glimmer::AudioManager::AudioManager() = default;

void glimmer::AudioManager::SetMixer(MIX_Mixer *mixer) {
    mixer_ = mixer;
}

void glimmer::AudioManager::SetMasterVolume(const float volume) {
    masterVolume_ = std::clamp(volume, 0.0F, 1.0F);
    //After setting the main volume, refresh the volume of all tracks.
    //设置主音量后，刷新所有音轨的音量。
    for (const auto &typeVolume: typeVolume_) {
        SetTypeVolume(typeVolume.first, typeVolume.second);
    }
}

void glimmer::AudioManager::SetTypeVolume(const AudioType type, const float volume) {
    if (mixer_ == nullptr) {
        return;
    }
    const float clampVolume = std::clamp(volume, 0.0F, 1.0F);
    typeVolume_[type] = clampVolume;
    MIX_SetTagGain(mixer_, AudioTypeToTag(type), masterVolume_ * clampVolume);
}

void glimmer::AudioManager::TryPlayFree(const AudioType audioType, MIX_Audio *audio, const int loopsNumber) {
    if (audio == nullptr) {
        return;
    }

    MIX_Track *track = GetFreeTrack(audioType);
    if (!track) {
        LogCat::w("No free track for audio!");
        return;
    }
    MIX_SetTrackAudio(track, audio);
    const SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loopsNumber);
    MIX_PlayTrack(track, props);
    SDL_DestroyProperties(props);
}

void glimmer::AudioManager::ForcePlayReplace(const AudioType audioType, MIX_Audio *audio, const int loopsNumber) {
    if (audio == nullptr) {
        return;
    }
    MIX_Track *track = GetFreeTrack(audioType);
    if (track == nullptr) {
        const auto tracks = &track_[audioType];
        if (tracks == nullptr) {
            return;
        }
        const size_t size = tracks->size();
        if (size == 0) {
            return;
        }
        track = (*tracks)[0];
        MIX_StopTrack(track,0);
    }
    MIX_SetTrackAudio(track, audio);
    const SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loopsNumber);
    MIX_PlayTrack(track, props);
    SDL_DestroyProperties(props);
}
