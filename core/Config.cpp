//
// Created by Cold-Mint on 2025/10/9.
//
#include "Config.h"

#include <nlohmann/json.hpp>

#include "utils/JsonUtils.h"

bool glimmer::Config::loadConfig(const VirtualFileSystem *virtualFileSystem, const std::string &path) {
    auto jsonOpt = JsonUtils::LoadJsonFromFile(virtualFileSystem, path);
    if (!jsonOpt) {
        return false;
    }
    auto &jsonObject = *jsonOpt;
    window = jsonObject["window"].get<Window>();
    mods = jsonObject["mods"].get<Mods>();
    return true;
}
