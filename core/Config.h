//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <vector>

namespace Glimmer {
    struct Window {
        int width = 1920;
        int height = 1080;
        bool resizable = true;
        float framerate = 60;
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
        bool loadConfig(const std::string &path);
    };
}

#endif //CONFIG_H
