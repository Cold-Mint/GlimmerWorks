//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <vector>
#include "mod/resourcePack/AudioType.h"
#include "toml11/types.hpp"


namespace glimmer {
    struct Window {
        int width = 1920;
        int height = 1080;
        bool resizable = true;
        float framerate = 60;
        //uiScale
        //ui缩放
        float uiScale = 1.0F;
        bool vSync = true;
    };

    struct Mods {
        std::string dataPackPath;
        std::string resourcePackPath;
        std::vector<std::string> enabledDataPack;
        std::vector<std::string> enabledResourcePack;
        std::vector<std::string> supportedTextureFormats;
        std::vector<std::string> supportedAudioFormats;
    };

    struct Debug {
        bool displayDebugPanel;
        bool displayBox2dShape;
    };

    struct World {
        float preloadChunkRadius;
        float preloadStructureRadius;
    };

    struct AudioTrack {
        AudioType type;
        int trackCount;
        float volume;
    };

    struct Audio {
        float masterVolume;
        int channels;
        int freq;
        std::string format = "F32";
        std::vector<AudioTrack> track;
    };


    class Config {
    public:
        Window window{};
        Mods mods{};
        World world{};
        Debug debug{};
        Audio audio{};
        int configVersion = 1;

        void LoadConfig(const toml::value &configValue);
    };
}

#endif //CONFIG_H
