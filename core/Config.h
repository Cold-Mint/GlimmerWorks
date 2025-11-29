//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <vector>

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


    class Config {
    public:
        Window window{};
        Mods mods{};

        bool loadConfig(const VirtualFileSystem *virtualFileSystem, const std::string &path);
    };
}

#endif //CONFIG_H
