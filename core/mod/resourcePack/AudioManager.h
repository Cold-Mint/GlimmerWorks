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
#pragma once

#include <unordered_map>
#include <vector>

#include "AudioType.h"
#include "SDL3_mixer/SDL_mixer.h"

namespace glimmer {
    class AudioManager {
        using ID = uint32_t;


        std::unordered_map<AudioType, std::vector<MIX_Track *> > track_ = {};

        float masterVolume_ = 1.0F;
        std::unordered_map<AudioType, float> typeVolume_ = {
            {AudioType::BGM, 1.0F},
            {AudioType::AMBIENT, 1.0F},
        };
        MIX_Mixer *mixer_ = nullptr;

        /**
         * Audio type to Tag
         * 音频类型到Tag
         * @param type
         * @return
         */
        static const char *AudioTypeToTag(AudioType type);

        /**
         * GetFreeTrack
         * 获取空闲的音轨
         * @param type
         * @return
         */
        MIX_Track *GetFreeTrack(AudioType type);

    public:
        explicit AudioManager();

        void SetMixer(MIX_Mixer *mixer);


        /**
         * Set Master Volume
         * 设置总音量
         * The total volume will affect all types of audio.
         * 总音量会影响所有类型的音频。
         * @param volume
         */
        void SetMasterVolume(float volume);

        /**
         * Set the volume of a certain type
         * 设置某个类型的音量
         * @param type
         * @param volume
         */
        void SetTypeVolume(AudioType type, float volume);

        /**
         * TryPlayFree
         * 尝试使用空闲音轨播放
         * @param audioType 音轨类型
         * @param audio 音频文件
         * @param loopsNumber 循环次数
         */
        void TryPlayFree(AudioType audioType, MIX_Audio *audio, int loopsNumber);

        /**
         * 强制播放并替换音频
         * 如果有空闲音轨道那么使用它，否则替换当前播放的音频
         * @param audioType
         * @param audio
         * @param loopsNumber
         */
        void ForcePlayReplace(AudioType audioType, MIX_Audio *audio, int loopsNumber);

        /**
         * Create track
         * 创建音轨
         * @param type
         * @param count
         */
        void CreateTracks(AudioType type, size_t count);
    };
}
