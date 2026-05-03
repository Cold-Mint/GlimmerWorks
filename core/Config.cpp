//
// Created by Cold-Mint on 2025/10/9.
//
#include "Config.h"

#include "CommandHookManager.h"
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
struct toml::from<glimmer::CommandHookResource> {
    static glimmer::CommandHookResource from_toml(const value &v) {
        glimmer::CommandHookResource commandHookResource{};
        commandHookResource.hookId = toml::find<std::string>(v, "hook_id");
        commandHookResource.command = toml::find<std::string>(v, "command");
        commandHookResource.keyRepeat = toml::find_or<bool>(v, "key_repeat", false);
        commandHookResource.code = toml::find<std::string>(v, "code");
        commandHookResource.eventType = toml::find<std::string>(v, "event_type");
        return commandHookResource;
    }
};


void glimmer::Config::LoadConfig(CommandHookManager *commandHookManager, const toml::value &configValue) {
    configVersion = toml::find<int>(configValue, "config_version");
    window.height = toml::find<int>(configValue, "window", "height");
    window.width = toml::find<int>(configValue, "window", "width");
    window.fullscreen = toml::find<bool>(configValue, "window", "fullscreen");
    window.uiScale = toml::find<float>(configValue, "window", "ui_scale");
    window.vSync = toml::find<bool>(configValue, "window", "vsync");
    window.idleDelayMs = toml::find<int>(configValue, "window", "idle_delay_ms");
    window.idleTargetFps = toml::find<float>(configValue, "window", "idle_target_fps");
    window.normalTargetFps = toml::find<float>(configValue, "window", "normal_target_fps");
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
    console.maxHistoryEntries = toml::find<int>(configValue, "console", "max_history_entries");
    runtimePath = toml::find<std::string>(configValue, "runtime_path");
    command.locateMaxRadiusSearchChunks = toml::find<int>(configValue, "command", "locate_max_radius_search_chunks");
    commandHooks = toml::find<std::vector<CommandHookResource> >(configValue, "command_hooks");
    commandHookManager->LoadHookFromConfig(commandHooks);
}
