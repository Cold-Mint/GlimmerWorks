//
// Created by coldmint on 2026/3/25.
//

#ifndef GLIMMERWORKS_AUDIOMANAGER_H
#define GLIMMERWORKS_AUDIOMANAGER_H

#include <map>
#include <vector>

#include "AudioType.h"
#include "SDL3_mixer/SDL_mixer.h"

namespace glimmer {
    class AudioManager {
        using ID = uint32_t;


        MIX_Track *GetFreeTrack(AudioType type);


        std::map<AudioType, std::vector<MIX_Track *> > track_ = {};
        MIX_Mixer *mixer_ = nullptr;

    public:
        explicit AudioManager();

        void SetMixer(MIX_Mixer *mixer);

        /**
         * PlayAudio
         * 播放音效
         * @param audioType 音效类型
         * @param audio 音效对象
         * @param loopsNumber 循环次数，设置为-1，那么无限循环。
         * @return
         */
        void PlayAudio(AudioType audioType, MIX_Audio *audio, int loopsNumber);

        void CreateTracks(AudioType type, size_t count);
    };
}


#endif //GLIMMERWORKS_AUDIOMANAGER_H
