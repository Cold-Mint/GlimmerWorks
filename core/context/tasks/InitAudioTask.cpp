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
#include "InitAudioTask.h"

#include <utility>

#include "core/config/Config.h"
#include "core/context/AudioContext.h"
#include "core/context/ISystemBucket.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/resourcePack/AudioManager.h"
#include "core/mod/resourcePack/ResourcePackManager.h"
#include "SDL3_mixer/SDL_mixer.h"

bool glimmer::InitAudioTask::Run(ISystemBucket *systemBucket) {
    Config *config = systemBucket->GetConfig();
    if (config == nullptr) {
        LogCat::e(std::source_location::current(), "config_is_null", "config is nullptr");
        return false;
    }
    SDL_AudioSpec audioSpec;
    const std::string &audioFormat = config->audio.format;
    if (audioFormat == "U8") {
        audioSpec.format = SDL_AUDIO_U8;
    } else if (audioFormat == "S16") {
        audioSpec.format = SDL_AUDIO_S16;
    } else if (audioFormat == "S32") {
        audioSpec.format = SDL_AUDIO_S32;
    } else {
        audioSpec.format = SDL_AUDIO_F32;
    }

    audioSpec.channels = config->audio.channels;
    audioSpec.freq = config->audio.freq;
    LogCat::i("creating_audio_mixer", "Creating audio mixer: format={}, channels={}, freq={}", audioFormat,
              config->audio.channels, config->audio.freq);

    MIX_Mixer *mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec);
    if (mixer == nullptr) {
        LogCat::e(std::source_location::current(), "mix_create_mixer_device_failed", "MIX_CreateMixerDevice failed");
        return false;
    }
    LogCat::i("audio_mixer_created", "Audio mixer created successfully");

    ResourcePackManager *resourcePackManager = systemBucket->GetResourcePackManager();
    if (resourcePackManager == nullptr) {
        LogCat::e(std::source_location::current(), "resource_pack_manager_is_null", "ResourcePackManager is nullptr");
        return false;
    }
    LogCat::i("loading_main_menu_bgm", "Loading main menu BGM");
    AudioContext *audioContext = systemBucket->GetAudioContext();
    if (audioContext == nullptr) {
        LogCat::e(std::source_location::current(), "audio_context_is_null", "audioContext is nullptr");
        return false;
    }
    ResourceLocator *resourceLocator = systemBucket->GetResourceLocator();
    if (resourceLocator == nullptr) {
        LogCat::e(std::source_location::current(), "resource_locator_is_null", "resourceLocator is nullptr");
        return false;
    }
    audioContext->LoadMainMenuBGM(resourceLocator);
    AudioManager *audioManager = audioContext->GetAudioManager();
    if (audioManager == nullptr) {
        LogCat::e(std::source_location::current(), "audio_manager_is_null", "audioManager is nullptr");
        return false;
    }
    audioManager->SetMixer(mixer);

    LogCat::i("configuring_audio_tracks", "Configuring audio tracks: count={}", config->audio.track.size());
    for (const AudioTrack &trackConfig: config->audio.track) {
        audioManager->CreateTracks(trackConfig.type, trackConfig.trackCount);
        audioManager->SetTypeVolume(trackConfig.type, trackConfig.volume);
        LogCat::i("audio_track", "  Track: type={}, count={}, volume={}",
                  static_cast<int>(std::to_underlying(trackConfig.type)),
                  trackConfig.trackCount,
                  trackConfig.volume);
    }
    audioManager->SetMasterVolume(config->audio.masterVolume);
    LogCat::i("master_volume_set", "Master volume set to: {}", config->audio.masterVolume);

    LogCat::i("init_audio_completed", "InitAudio completed successfully");
    return true;
}

void glimmer::InitAudioTask::Rollback(ISystemBucket *) {
    // 混音器设备由 InitSDLTask 的 MIX_Quit 统一释放。
}

std::string glimmer::InitAudioTask::GetTaskName() {
    return "InitAudioTask";
}
