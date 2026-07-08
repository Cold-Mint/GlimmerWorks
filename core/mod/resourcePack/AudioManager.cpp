/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "AudioManager.h"

#include <algorithm>
#include "core/log/LogCat.h"


void glimmer::AudioManager::CreateTracks(const AudioType type, const size_t count)
{
    if (track_.contains(type))
    {
        for (auto t : track_[type])
        {
            MIX_DestroyTrack(t);
        }
        track_[type].clear();
    }
    std::vector<MIX_Track*> newTracks;
    for (size_t i = 0; i < count; ++i)
    {
        MIX_Track* newTrack = MIX_CreateTrack(mixer_);
        if (!newTrack)
        {
            LogCat::e("MIX_CreateTrack failed: ", SDL_GetError());
            continue;
        }
        MIX_TagTrack(newTrack, AudioTypeToTag(type));
        newTracks.push_back(newTrack);
    }

    track_[type] = std::move(newTracks);
}

MIX_Track* glimmer::AudioManager::GetFreeTrack(const AudioType type)
{
    if (!track_.contains(type))
    {
        return nullptr;
    }
    for (auto track : track_[type])
    {
        if (!MIX_TrackPlaying(track))
        {
            return track;
        }
    }
    return nullptr;
}

const char* glimmer::AudioManager::AudioTypeToTag(const AudioType type)
{
    switch (type)
    {
    case AudioType::BGM: return "BGM";
    case AudioType::AMBIENT: return "AMBIENT";
    default: return "UNKNOWN";
    }
}

glimmer::AudioManager::AudioManager() = default;

void glimmer::AudioManager::SetMixer(MIX_Mixer* mixer)
{
    mixer_ = mixer;
}

void glimmer::AudioManager::SetMasterVolume(const float volume)
{
    masterVolume_ = std::clamp(volume, 0.0F, 1.0F);
    //After setting the main volume, refresh the volume of all tracks.
    //设置主音量后，刷新所有音轨的音量。
    for (const auto& [type, typeVolume] : typeVolume_)
    {
        SetTypeVolume(type, typeVolume);
    }
}

void glimmer::AudioManager::SetTypeVolume(const AudioType type, const float volume)
{
    if (mixer_ == nullptr)
    {
        return;
    }
    const float clampVolume = std::clamp(volume, 0.0F, 1.0F);
    typeVolume_[type] = clampVolume;
    MIX_SetTagGain(mixer_, AudioTypeToTag(type), masterVolume_ * clampVolume);
}

void glimmer::AudioManager::TryPlayFree(const AudioType audioType, MIX_Audio* audio, const int loopsNumber)
{
    if (audio == nullptr)
    {
        return;
    }

    MIX_Track* track = GetFreeTrack(audioType);
    if (!track)
    {
        LogCat::w("No free track for audio!");
        return;
    }
    MIX_SetTrackAudio(track, audio);
    const SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loopsNumber);
    MIX_PlayTrack(track, props);
    SDL_DestroyProperties(props);
}

void glimmer::AudioManager::ForcePlayReplace(const AudioType audioType, MIX_Audio* audio, const int loopsNumber)
{
    if (audio == nullptr)
    {
        return;
    }
    MIX_Track* track = GetFreeTrack(audioType);
    if (track == nullptr)
    {
        const auto tracks = &track_[audioType];
        if (tracks == nullptr)
        {
            return;
        }
        const size_t size = tracks->size();
        if (size == 0)
        {
            return;
        }
        track = (*tracks)[0];
        MIX_StopTrack(track, 0);
    }
    MIX_SetTrackAudio(track, audio);
    const SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loopsNumber);
    MIX_PlayTrack(track, props);
    SDL_DestroyProperties(props);
}
