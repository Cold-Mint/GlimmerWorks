//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <vector>

#include "nlohmann/json_fwd.hpp"
#include "vfs/VirtualFileSystem.h"

namespace glimmer {
    struct Window {
        int width = 1920;
        int height = 1080;
        bool resizable = true;
        float framerate = 60;
        //ui缩放
        float uiScale = 1.0F;
    };

    struct Mods {
        std::string dataPackPath;
        std::string resourcePackPath;
        std::vector<std::string> enabledDataPack;
        std::vector<std::string> enabledResourcePack;
    };

    struct Debug {
        bool displayDebugPanel;
        bool displayBox2dShape;
    };


    class Config {
    public:
        Window window{};
        Mods mods{};
        Debug debug{};
        int configVersion = 1;

        bool LoadConfig(const nlohmann::json& json);
    };
}

#endif //CONFIG_H
