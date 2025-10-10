//
// Created by Cold-Mint on 2025/10/9.
//
#include "Config.h"

#include <nlohmann/json.hpp>

#include "utils/JsonUtils.h"

bool Glimmer::Config::loadConfig(const std::string &path) {
    auto jsonOpt = JsonUtils::LoadJsonFromFile(path);
    if (!jsonOpt) {
        return false;
    }
    auto &jsonObject = *jsonOpt;
    window = jsonObject["window"].get<Window>();
    mods = jsonObject["mods"].get<Mods>();
    return true;
}
