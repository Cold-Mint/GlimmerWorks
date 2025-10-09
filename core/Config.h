//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef CONFIG_H
#define CONFIG_H
#include <string>

namespace Glimmer {
    struct Window {
        int width = 1920;
        int height = 1080;
        bool resizable = true;
        int framerate = 60;
    };


    class Config {
        Config() = default;

    public:
        Window window{};

        bool loadConfig(const std::string &path);

        static Config &getInstance() {
            static Config instance;
            return instance;
        }
    };
}

#endif //CONFIG_H
