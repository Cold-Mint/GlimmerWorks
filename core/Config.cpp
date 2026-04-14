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

template<>
struct toml::from<glimmer::HotkeyCommand> {
    static glimmer::HotkeyCommand from_toml(const value &v) {
        glimmer::HotkeyCommand cmd{};
        const auto &groupArray = toml::find<array>(v, "command_group");
        for (const auto &innerArr: groupArray) {
            auto strVec = toml::get<std::vector<std::string> >(innerArr);
            cmd.commandGroup.emplace_back(std::move(strVec));
        }
        return cmd;
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
    world.preloadLightingRadius = toml::find<float>(configValue, "world", "preload_lighting_radius");
    if (world.preloadLightingRadius > world.preloadChunkRadius) {
        world.preloadLightingRadius = world.preloadChunkRadius;
    }
    audio.channels = toml::find<int>(configValue, "audio", "channels");
    audio.masterVolume = toml::find<float>(configValue, "audio", "master_volume");
    audio.freq = toml::find<int>(configValue, "audio", "freq");
    audio.track = toml::find<std::vector<AudioTrack> >(configValue, "audio", "track");
    audio.format = toml::find<std::string>(configValue, "audio", "format");
    debug.displayDebugPanel = toml::find<bool>(configValue, "debug", "display_debug_panel");
    debug.displayBox2dShape = toml::find<bool>(configValue, "debug", "display_box2d_shape");
    debug.displayDraggableTarget = toml::find<bool>(configValue, "debug", "display_draggable_target");
    debug.displayElevationMap = toml::find<bool>(configValue, "debug", "display_elevation_map");
    debug.displayTempMap = toml::find<bool>(configValue, "debug", "display_temp_map");
    debug.displayHumidityMap = toml::find<bool>(configValue, "debug", "display_humidity_map");
    debug.displayErosionMap = toml::find<bool>(configValue, "debug", "display_erosion_map");
    debug.displayWeirdnessMap = toml::find<bool>(configValue, "debug", "display_weirdness_map");
    light.enable = toml::find<bool>(configValue, "light", "enable");
    light.exposure = toml::find<float>(configValue, "light", "exposure");
    console.maxHistoryEntries = toml::find<int>(configValue, "console", "max_history_entries");
    runtimePath = toml::find<std::string>(configValue, "runtime_path");
    command.locateMaxRadiusSearchChunks = toml::find<int>(configValue, "command", "locate_max_radius_search_chunks");
    f1 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f1");
    f2 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f2");
    f3 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f3");
    f4 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f4");
    f5 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f5");
    f6 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f6");
    f7 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f7");
    f8 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f8");
    f9 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f9");
    f10 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f10");
    f11 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f11");
    f12 = toml::find_or_default<HotkeyCommand>(configValue, "hotkey_f12");
}
