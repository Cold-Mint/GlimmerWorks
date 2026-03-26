//
// Created by coldmint on 2026/3/25.
//

#ifndef GLIMMERWORKS_AUDIOMANAGER_H
#define GLIMMERWORKS_AUDIOMANAGER_H

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
            {BGM, 1.0F},
            {AMBIENT, 1.0F},
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


#endif //GLIMMERWORKS_AUDIOMANAGER_H
