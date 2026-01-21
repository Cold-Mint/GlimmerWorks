//
// Created by Cold-Mint on 2025/10/9.
//
#include "Config.h"

#include <nlohmann/json.hpp>

#include "utils/JsonUtils.h"

bool glimmer::Config::LoadConfig(const nlohmann::json *json) {
    auto &jsonObject = *json;
    configVersion = jsonObject["configVersion"].get<int>();
    window = jsonObject["window"].get<Window>();
    mods = jsonObject["mods"].get<Mods>();
    debug = jsonObject["debug"].get<Debug>();
    return true;
}
