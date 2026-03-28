//
// Created by Cold-Mint on 2025/10/9.
//
#include "Config.h"

#include "toml11/find.hpp"

template<>
struct toml::from<glimmer::AudioTrack> {
    static glimmer::AudioTrack from_toml(const value &v) {
        glimmer::AudioTrack track{};
        track.trackCount = toml::find<int>(v, "track_count");
        track.type = static_cast<glimmer::AudioType>(toml::find<int>(v, "type"));
        track.volume = toml::find<float>(v, "volume");
        return track;
    }
};

void glimmer::Config::LoadConfig(const toml::value &configValue) {
    configVersion = toml::find<int>(configValue, "config_version");
    window.height = toml::find<int>(configValue, "window", "height");
    window.width = toml::find<int>(configValue, "window", "width");
    window.resizable = toml::find<bool>(configValue, "window", "resizable");
    window.uiScale = toml::find<float>(configValue, "window", "ui_scale");
    window.vSync = toml::find<bool>(configValue, "window", "vsync");
    window.framerate = toml::find<float>(configValue, "window", "framerate");
    mods.dataPackPath = toml::find<std::string>(configValue, "mods", "data_pack_path");
    mods.resourcePackPath = toml::find<std::string>(configValue, "mods", "resource_pack_path");
    mods.enabledDataPack = toml::find<std::vector<std::string> >(configValue, "mods", "enabled_data_pack");
    mods.enabledResourcePack = toml::find<std::vector<std::string> >(configValue, "mods", "enabled_resource_pack");
    mods.supportedTextureFormats = toml::find<std::vector<
        std::string> >(configValue, "mods", "supported_texture_formats");
    mods.supportedAudioFormats = toml::find<std::vector<std::string> >(configValue, "mods", "supported_audio_formats");
    world.preloadChunkRadius = toml::find<float>(configValue, "world", "preload_chunk_radius");
    world.preloadStructureRadius = toml::find<float>(configValue, "world", "preload_structure_radius");
    audio.channels = toml::find<int>(configValue, "audio", "channels");
    audio.masterVolume = toml::find<float>(configValue, "audio", "master_volume");
    audio.freq = toml::find<int>(configValue, "audio", "freq");
    audio.track = toml::find<std::vector<AudioTrack> >(configValue, "audio", "track");
    audio.format = toml::find<std::string>(configValue, "audio", "format");
    debug.displayDebugPanel = toml::find<bool>(configValue, "debug", "display_debug_panel");
    debug.displayBox2dShape = toml::find<bool>(configValue, "debug", "display_box2d_shape");
}
