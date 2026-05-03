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
    class CommandHookManager;

    struct Window {
        int width = 1920;
        int height = 1080;
        bool fullscreen = false;
        // After being idle and without any operation for a certain period of time, the frame rate will start to decrease (unit: milliseconds).
        // -1 indicates that the function is turned off.
        // 闲置无操作多久后开始降帧 (单位：毫秒)，-1为关闭功能。
        int idleDelayMs;

        // The minimum rendering frame rate locked in the idle state
        // 闲置状态下锁定的最低渲染帧率
        float idleTargetFps;

        // The baseline frame rate during normal gameplay
        // 正常游玩时的基准帧率
        float normalTargetFps;

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
    };

    struct LightConfig {
        bool enable;
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

    struct Audio {
        float masterVolume;
        int channels;
        int freq;
        std::string format = "F32";
        std::vector<AudioTrack> track;
    };

    struct CommandHookResource {
        std::string hookId;
        std::string command;
        bool keyRepeat;
        std::string scancode;
        std::string eventType;
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
        std::string runtimePath;
        LightConfig light;
        std::vector<CommandHookResource> commandHooks;

        void LoadConfig(CommandHookManager *commandHookManager, const toml::value &configValue);
    };
}

#endif //CONFIG_H
