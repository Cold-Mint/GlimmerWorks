/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "Config.h"

#include "CommandHookManager.h"
#include "toml11/find.hpp"

template <>
struct toml::from<glimmer::AudioTrack>
{
    static glimmer::AudioTrack from_toml(const value& v)
    {
        glimmer::AudioTrack track{};
        track.trackCount = toml::find<int>(v, "track_count");
        track.type = static_cast<glimmer::AudioType>(toml::find<int>(v, "type"));
        track.volume = toml::find<float>(v, "volume");
        return track;
    }
};


template <>
struct toml::from<glimmer::CommandHookResource>
{
    static glimmer::CommandHookResource from_toml(const value& v)
    {
        glimmer::CommandHookResource commandHookResource{};
        commandHookResource.hookId = toml::find<std::string>(v, "hook_id");
        commandHookResource.command = toml::find<std::string>(v, "command");
        commandHookResource.keyRepeat = toml::find_or<bool>(v, "key_repeat", false);
        commandHookResource.code = toml::find<std::string>(v, "code");
        commandHookResource.eventType = toml::find<std::string>(v, "event_type");
        return commandHookResource;
    }
};


glimmer::Config::~Config()
{
    onConfigChanged_.clear();
}

size_t glimmer::Config::RegisterOnConfigChanged(const bool fireImmediately,
                                                std::unique_ptr<std::function<void(const Config*)>> onConfigChanged)
{
    if (onConfigChanged == nullptr)
    {
        return INVALID_CONFIG_CALL_BACK;
    }
    if (fireImmediately)
    {
        (*onConfigChanged)(this);
    }
    auto id = nextConfigChangedId_++;
    onConfigChanged_.emplace_back(id, std::move(onConfigChanged));
    return id;
}

void glimmer::Config::UnregisterOnConfigChanged(size_t id)
{
    if (id == INVALID_CONFIG_CALL_BACK)
    {
        return;
    }
    std::erase_if(onConfigChanged_, [id](auto& pair)
    {
        return pair.first == id;
    });
}

void glimmer::Config::LoadConfig(const toml::value& configValue)
{
    configVersion = toml::find<int>(configValue, "config_version");
    window.height = toml::find<int>(configValue, "window", "height");
    window.width = toml::find<int>(configValue, "window", "width");
    window.fullscreen = toml::find<bool>(configValue, "window", "fullscreen");
    window.uiScale = toml::find<float>(configValue, "window", "ui_scale");
    window.cameraScale = toml::find<float>(configValue, "window", "camera_scale");
    window.vSync = toml::find<bool>(configValue, "window", "vsync");
    window.idleDelay = toml::find<float>(configValue, "window", "idle_delay");
    window.idleTargetFps = toml::find<float>(configValue, "window", "idle_target_fps");
    window.normalTargetFps = toml::find<float>(configValue, "window", "normal_target_fps");
    mods.enableSignVerify = toml::find<bool>(configValue, "mods", "enable_sign_verify");
    mods.loadOnlyVerified = toml::find<bool>(configValue, "mods", "load_only_verified");
    mods.dataPackPath = toml::find<std::string>(configValue, "mods", "data_pack_path");
    mods.resourcePackPath = toml::find<std::string>(configValue, "mods", "resource_pack_path");
    mods.enabledDataPack = toml::find<std::vector<std::string>>(configValue, "mods", "enabled_data_pack");
    mods.enabledResourcePack = toml::find<std::vector<std::string>>(configValue, "mods", "enabled_resource_pack");
    mods.supportedTextureFormats = toml::find<std::vector<
        std::string>>(configValue, "mods", "supported_texture_formats");
    mods.supportedAudioFormats = toml::find<std::vector<std::string>>(configValue, "mods", "supported_audio_formats");
    world.preloadChunkRadius = toml::find<float>(configValue, "world", "preload_chunk_radius");
    world.preloadStructureRadius = toml::find<float>(configValue, "world", "preload_structure_radius");
    world.preloadLightingRadius = toml::find<float>(configValue, "world", "preload_lighting_radius");
    if (world.preloadLightingRadius > world.preloadChunkRadius)
    {
        world.preloadLightingRadius = world.preloadChunkRadius;
    }
    world.chunkSpawnCleanInterval = toml::find<float>(configValue, "world", "chunk_spawn_clean_interval");
    world.loadTerrainInterval = toml::find<float>(configValue, "world", "load_terrain_interval");
    world.loadTerrainBatch = toml::find<uint16_t>(configValue, "world", "load_terrain_batch");
    world.loadChunkInterval = toml::find<float>(configValue, "world", "load_chunk_interval");
    world.loadChunkBatch = toml::find<uint16_t>(configValue, "world", "load_chunk_batch");
    world.unloadChunkInterval = toml::find<float>(configValue, "world", "unload_chunk_interval");
    world.unloadChunkBatch = toml::find<uint16_t>(configValue, "world", "unload_chunk_batch");
    world.unloadTerrainInterval = toml::find<float>(configValue, "world", "unload_terrain_interval");
    world.unloadTerrainBatch = toml::find<uint16_t>(configValue, "world", "unload_terrain_batch");
    audio.channels = toml::find<int>(configValue, "audio", "channels");
    audio.masterVolume = toml::find<float>(configValue, "audio", "master_volume");
    audio.freq = toml::find<int>(configValue, "audio", "freq");
    audio.track = toml::find<std::vector<AudioTrack>>(configValue, "audio", "track");
    audio.format = toml::find<std::string>(configValue, "audio", "format");
    console.maxHistoryEntries = toml::find<uint16_t>(configValue, "console", "max_history_entries");
    runtimePath = toml::find<std::string>(configValue, "runtime_path");
    command.locateMaxRadiusSearchChunks = toml::find<uint16_t>(configValue, "command",
                                                               "locate_max_radius_search_chunks");
    commandHooks = toml::find<std::vector<CommandHookResource>>(configValue, "command_hooks");
    anim.chunkFadeinDuration = toml::find<float>(configValue, "animation", "chunk_fadein_duration");
    anim.chunkFadeInFrom = toml::find<float>(configValue, "animation", "chunk_fadein_from");
    anim.chunkFadeInTo = toml::find<float>(configValue, "animation", "chunk_fadein_to");
#if  !defined(NDEBUG)
    debug.displayDebugPanel = toml::find<bool>(configValue, "debug", "display_debug_panel");
    debug.displayBox2dShape = toml::find<bool>(configValue, "debug", "display_box2d_shape");
    debug.displayDraggableTarget = toml::find<bool>(configValue, "debug", "display_draggable_target");
    debug.displayElevationMap = toml::find<bool>(configValue, "debug", "display_elevation_map");
    debug.displayTempMap = toml::find<bool>(configValue, "debug", "display_temp_map");
    debug.displayHumidityMap = toml::find<bool>(configValue, "debug", "display_humidity_map");
    debug.displayErosionMap = toml::find<bool>(configValue, "debug", "display_erosion_map");
    debug.displayWeirdnessMap = toml::find<bool>(configValue, "debug", "display_weirdness_map");
    light.enable = toml::find<bool>(configValue, "light", "enable");
#endif
    for (auto& item : onConfigChanged_)
    {
        auto& callbackFunc = item.second;
        if (callbackFunc)
        {
            (*callbackFunc)(this);
        }
    }
}
