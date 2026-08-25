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
#pragma once
#include <string>
#include <vector>

#include "mod/resourcePack/AudioType.h"
#include "toml11/types.hpp"


namespace glimmer {
    class CommandHookManager;

    struct Window {
        int width = 1920;
        int height = 1080;
        bool fullscreen = false;
        //How long (in seconds) should the frame rate be reduced after being idle and without any operation? -1 indicates that the function is turned off.
        // 闲置无操作多久后开始降帧 (单位：秒)，-1为关闭功能。
        float idleDelay;

        // The minimum rendering frame rate locked in the idle state
        // 闲置状态下锁定的最低渲染帧率
        float idleTargetFps;

        // The baseline frame rate during normal gameplay
        // 正常游玩时的基准帧率
        float normalTargetFps;

        //uiScale
        //ui缩放
        float uiScale = 1.0F;
        float cameraScale = 2.0F;
        bool vSync = true;
    };

    struct MainMenuBackground {
        float targetFps;
    };

    struct AnimConfig {
        float chunkFadeinDuration = 0.35F;
        float chunkFadeInFrom = 0.0F;
        float chunkFadeInTo = 1.0F;
    };

    struct BiomeBGMConfig {
        // Player must stay in a different biome this long before the BGM switches.
        // 玩家须在新生物群系连续停留该时长后才会切换 BGM。
        float debounceSeconds = 3.0F;
    };

    struct CommandConfig {
        uint16_t locateMaxRadiusSearchChunks = 2048;
    };

    struct Mods {
        bool enableSignVerify;
        bool loadOnlyVerified;
        std::string dataPackPath;
        std::string resourcePackPath;
        std::vector<std::string> enabledDataPack;
        std::vector<std::string> enabledResourcePack;
    };

    struct Debug {
        bool displayDebugPanel;
        bool displayBox2dShape;
        bool displayDraggableTarget;
        bool displayElevationMap;
        bool displayTempMap;
        bool displayHumidityMap;
        bool displayErosionMap;
        bool displayWeirdnessMap;
    };

    struct LightConfig {
        bool enable;
    };

    /**
     * GPU lighting pass tuning (applied by the lighting fragment shader,
     * see shaders/@core/lighting.frag). Available in all build types.
     * GPU 光照通道调参（由光照片元着色器应用，见 shaders/@core/lighting.frag）。
     * 所有构建类型均可用。
     */
    struct LightingConfig {
        /**
         * Light alpha (0..255) that maps to full brightness on screen.
         * Lights at/above this value render at full intensity; lower values
         * widen the smooth falloff band around light sources.
         * 映射为全亮的光照 alpha（0..255）。达到/超过该值的光照以全强度渲染；
         * 调小该值会加宽光源周围的平滑衰减带。
         */
        float fullBrightAlpha = 12.0F;
        /**
         * Minimum visibility floor (0..1) for pixels without any light,
         * keeps entities faintly visible in complete darkness.
         * 完全无光像素的最低可见度（0..1），让实体在全黑环境中隐约可见。
         */
        float minVisibility = 0.05F;
        /**
         * Light hue tint strength (0..1): 0 = neutral gray lighting,
         * 1 = full hue filtering.
         * 光色染色强度（0..1）：0 = 中性灰光照，1 = 完整色相过滤。
         */
        float tintStrength = 0.6F;
    };

    struct Console {
        uint16_t maxHistoryEntries = 100;
    };

    struct World {
        float preloadChunkRadius;
        float preloadStructureRadius;
        float preloadLightingRadius;
        float chunkSpawnCleanInterval;
        float loadTerrainInterval;
        uint16_t loadTerrainBatch;
        float loadChunkInterval;
        uint16_t loadChunkBatch;
        float unloadChunkInterval;
        uint16_t unloadChunkBatch;
        float unloadTerrainInterval;
        uint16_t unloadTerrainBatch;
    };

    struct AudioTrack {
        AudioType type;
        int trackCount;
        float volume;
    };

    struct Audio {
        float masterVolume;
        int channels;
        int freq;
        std::string format = "F32";
        std::vector<AudioTrack> track;
    };

    struct CommandHookResource {
        std::string hookId;
        std::string command;
        bool keyRepeat;
        std::string code;
        std::string eventType;
    };


    class Config {
        uint64_t fingerprint_ = 0;
        std::unique_ptr<toml::value> configValue_ = nullptr;

    public:
        Window window{};
        Mods mods{};
        World world{};
        Audio audio{};
        Console console{};
        AnimConfig anim{};
        BiomeBGMConfig biomeBgm{};
        CommandConfig command{};
        MainMenuBackground mainMenuBackground{};
        LightingConfig lighting{};
        int configVersion = 1;
        std::string runtimePath;
        std::vector<CommandHookResource> commandHooks;
#if  !defined(NDEBUG)
        LightConfig light;
        Debug debug{};
#endif

        [[nodiscard]] uint64_t GetFingerprint() const;

        void SetConfigValue(std::unique_ptr<toml::value> configValue);

        [[nodiscard]] toml::value *GetConfigValue() const;

        bool ReloadConfig();
    };
}
