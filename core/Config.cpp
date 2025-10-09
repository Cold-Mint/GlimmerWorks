//
// Created by Cold-Mint on 2025/10/9.
//
#include "Config.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "log/LogCat.h"

bool Glimmer::Config::loadConfig(const std::string &path) {
    std::ifstream configFile(path);
    if (!configFile.is_open()) {
        LogCat::e("Failed to open config.json");
        return false;
    }

    nlohmann::json jsonObject;
    try {
        configFile >> jsonObject;
    } catch (const std::exception &e) {
        LogCat::e("Failed to parse JSON: ", e.what());
        return false;
    }
    configFile.close();
    window.width = jsonObject["window"]["width"].get<int>();
    window.height = jsonObject["window"]["height"].get<int>();
    window.resizable = jsonObject["window"]["resizable"].get<bool>();
    window.framerate = jsonObject["window"]["framerate"].get<int>();
    return true;
}
