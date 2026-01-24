//
// Created by Cold-Mint on 2025/10/9.
//
#include "Config.h"

#include "cmake-build-debug/_deps/toml11-src/include/toml11/find.hpp"

void glimmer::Config::LoadConfig(const toml::value &configValue) {
    configVersion = toml::find<int>(configValue, "configVersion");
    window.height = toml::find<int>(configValue, "window", "height");
    window.width = toml::find<int>(configValue, "window", "width");
    window.resizable = toml::find<bool>(configValue, "window", "resizable");
    window.uiScale = toml::find<float>(configValue, "window", "uiScale");
    window.framerate = toml::find<float>(configValue, "window", "framerate");
    mods.dataPackPath = toml::find<std::string>(configValue, "mods", "dataPackPath");
    mods.resourcePackPath = toml::find<std::string>(configValue, "mods", "resourcePackPath");
    mods.enabledDataPack = toml::find<std::vector<std::string> >(configValue, "mods", "enabledDataPack");
    mods.enabledResourcePack = toml::find<std::vector<std::string> >(configValue, "mods", "enabledResourcePack");
    debug.displayDebugPanel = toml::find<bool>(configValue, "debug", "displayDebugPanel");
    debug.displayBox2dShape = toml::find<bool>(configValue, "debug", "displayBox2dShape");
}
