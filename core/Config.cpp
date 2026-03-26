//
// Created by Cold-Mint on 2025/10/9.
//
#include "Config.h"

#include "toml11/find.hpp"

template<>
struct toml::from<glimmer::AudioTrack> {
    static glimmer::AudioTrack from_toml(const value &v) {
        glimmer::AudioTrack track{};
        track.trackCount = toml::find<int>(v, "trackCount");
        track.type = static_cast<glimmer::AudioType>(toml::find<int>(v, "type"));
        track.volume = toml::find<float>(v, "volume");
        return track;
    }
};

void glimmer::Config::LoadConfig(const toml::value &configValue) {
    configVersion = toml::find<int>(configValue, "configVersion");
    window.height = toml::find<int>(configValue, "window", "height");
    window.width = toml::find<int>(configValue, "window", "width");
    window.resizable = toml::find<bool>(configValue, "window", "resizable");
    window.uiScale = toml::find<float>(configValue, "window", "uiScale");
    window.vSync = toml::find<bool>(configValue, "window", "vSync");
    window.framerate = toml::find<float>(configValue, "window", "framerate");
    mods.dataPackPath = toml::find<std::string>(configValue, "mods", "dataPackPath");
    mods.resourcePackPath = toml::find<std::string>(configValue, "mods", "resourcePackPath");
    mods.enabledDataPack = toml::find<std::vector<std::string> >(configValue, "mods", "enabledDataPack");
    mods.enabledResourcePack = toml::find<std::vector<std::string> >(configValue, "mods", "enabledResourcePack");
    mods.supportedTextureFormats = toml::find<std::vector<
        std::string> >(configValue, "mods", "supportedTextureFormats");
    mods.supportedAudioFormats = toml::find<std::vector<std::string> >(configValue, "mods", "supportedAudioFormats");
    world.preloadChunkRadius = toml::find<float>(configValue, "world", "preloadChunkRadius");
    world.preloadStructureRadius = toml::find<float>(configValue, "world", "preloadStructureRadius");
    audio.channels = toml::find<int>(configValue, "audio", "channels");
    audio.masterVolume = toml::find<float>(configValue, "audio", "masterVolume");
    audio.freq = toml::find<int>(configValue, "audio", "freq");
    audio.track = toml::find<std::vector<AudioTrack> >(configValue, "audio", "track");
    audio.format = toml::find<std::string>(configValue, "audio", "format");
    debug.displayDebugPanel = toml::find<bool>(configValue, "debug", "displayDebugPanel");
    debug.displayBox2dShape = toml::find<bool>(configValue, "debug", "displayBox2dShape");
}
