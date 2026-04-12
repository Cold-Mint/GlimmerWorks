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

    struct CommandConfig {
        int locateMaxRadiusSearchChunks = 2048;
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
        bool displayDraggableTarget;
        bool displayElevationMap;
        bool displayTempMap;
        bool displayHumidityMap;
        bool displayErosionMap;
        bool displayWeirdnessMap;
        bool enableLighting;
    };

    struct Console {
        int maxHistoryEntries = 100;
    };

    struct World {
        float preloadChunkRadius;
        float preloadStructureRadius;
        float preloadLightingRadius;
    };

    struct AudioTrack {
        AudioType type;
        int trackCount;
        float volume;
    };

    struct HotkeyCommand {
        std::vector<std::vector<std::string> > commandGroup;
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
        Console console{};
        CommandConfig command{};
        int configVersion = 1;
        HotkeyCommand f1{};
        HotkeyCommand f2{};
        HotkeyCommand f3{};
        HotkeyCommand f4{};
        HotkeyCommand f5{};
        HotkeyCommand f6{};
        HotkeyCommand f7{};
        HotkeyCommand f8{};
        HotkeyCommand f9{};
        HotkeyCommand f10{};
        HotkeyCommand f11{};
        HotkeyCommand f12{};
        std::string runtimePath;

        void LoadConfig(const toml::value &configValue);
    };
}

#endif //CONFIG_H
