/*
 * Copyright (C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
 * 版权(C) 2025-2026  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "UniformInjectorRegistry.h"

#include <string>
#include <unordered_map>

#include "core/config/Constants.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/math/WorldVector2D.h"
#include "core/utils/TransparentStringHash.h"
#include "core/world/AmbientLight.h"
#include "core/world/WorldContext.h"

namespace {
    void InjectResolution(const glimmer::UniformInjectContext &ctx, float *dst) {
        dst[0] = ctx.width;
        dst[1] = ctx.height;
    }

    void InjectCameraPosition(const glimmer::UniformInjectContext &ctx, float *dst) {
        if (ctx.cameraTransform == nullptr) {
            dst[0] = 0.0F;
            dst[1] = 0.0F;
            return;
        }
        const glimmer::WorldVector2D position = ctx.cameraTransform->GetPosition();
        dst[0] = position.x;
        dst[1] = position.y;
    }

    void InjectCameraSize(const glimmer::UniformInjectContext &ctx, float *dst) {
        dst[0] = ctx.width;
        dst[1] = ctx.height;
    }

    void InjectZoom(const glimmer::UniformInjectContext &ctx, float *dst) {
        dst[0] = ctx.camera != nullptr ? ctx.camera->GetZoom() : 1.0F;
    }

    void InjectTileSize(const glimmer::UniformInjectContext &, float *dst) {
        dst[0] = static_cast<float>(TILE_SIZE);
    }

    void InjectLightmapOrigin(const glimmer::UniformInjectContext &ctx, float *dst) {
        dst[0] = static_cast<float>(ctx.lightMapOriginX);
        dst[1] = static_cast<float>(ctx.lightMapOriginY);
    }

    void InjectLightmapSize(const glimmer::UniformInjectContext &ctx, float *dst) {
        dst[0] = static_cast<float>(ctx.lightMapSizeX);
        dst[1] = static_cast<float>(ctx.lightMapSizeY);
    }

    void InjectTimeOfDay(const glimmer::UniformInjectContext &ctx, float *dst) {
        dst[0] = ctx.worldContext != nullptr ? ctx.worldContext->GetTimeOfDay() : 0.0F;
    }

    void InjectAmbientColor(const glimmer::UniformInjectContext &ctx, float *dst) {
        const float timeOfDay = ctx.worldContext != nullptr ? ctx.worldContext->GetTimeOfDay() : 0.0F;
        const glimmer::AmbientLight ambient = glimmer::ComputeAmbientLight(ctx.worldContext, timeOfDay);
        dst[0] = static_cast<float>(ambient.color.r) / 255.0F;
        dst[1] = static_cast<float>(ambient.color.g) / 255.0F;
        dst[2] = static_cast<float>(ambient.color.b) / 255.0F;
        dst[3] = ambient.intensity;
    }

    void InjectMoonPhase(const glimmer::UniformInjectContext &ctx, float *dst) {
        dst[0] = ctx.worldContext != nullptr ? static_cast<float>(ctx.worldContext->GetMoonPhase()) : 0.0F;
    }

    void InjectWeatherIntensity(const glimmer::UniformInjectContext &ctx, float *dst) {
        dst[0] = ctx.worldContext != nullptr ? ctx.worldContext->GetWeatherIntensity() : 0.0F;
    }

    void InjectSkyTopColor(const glimmer::UniformInjectContext &ctx, float *dst) {
        const float timeOfDay = ctx.worldContext != nullptr ? ctx.worldContext->GetTimeOfDay() : 0.0F;
        const glimmer::SkyColors sky = glimmer::ComputeSkyColors(ctx.worldContext, timeOfDay);
        dst[0] = static_cast<float>(sky.top.r) / 255.0F;
        dst[1] = static_cast<float>(sky.top.g) / 255.0F;
        dst[2] = static_cast<float>(sky.top.b) / 255.0F;
        dst[3] = 1.0F;
    }

    void InjectSkyHorizonColor(const glimmer::UniformInjectContext &ctx, float *dst) {
        const float timeOfDay = ctx.worldContext != nullptr ? ctx.worldContext->GetTimeOfDay() : 0.0F;
        const glimmer::SkyColors sky = glimmer::ComputeSkyColors(ctx.worldContext, timeOfDay);
        dst[0] = static_cast<float>(sky.horizon.r) / 255.0F;
        dst[1] = static_cast<float>(sky.horizon.g) / 255.0F;
        dst[2] = static_cast<float>(sky.horizon.b) / 255.0F;
        dst[3] = 1.0F;
    }

    using InjectorMap = std::unordered_map<std::string, glimmer::UniformInjector,
        glimmer::TransparentStringHash, std::equal_to<> >;

    InjectorMap &GetInjectorMap() {
        static InjectorMap injectors = [] {
            InjectorMap map;
            map[std::string(glimmer::BUILTIN_RESOLUTION)] = &InjectResolution;
            map[std::string(glimmer::BUILTIN_CAMERA_POSITION)] = &InjectCameraPosition;
            map[std::string(glimmer::BUILTIN_CAMERA_SIZE)] = &InjectCameraSize;
            map[std::string(glimmer::BUILTIN_ZOOM)] = &InjectZoom;
            map[std::string(glimmer::BUILTIN_TILE_SIZE)] = &InjectTileSize;
            map[std::string(glimmer::BUILTIN_LIGHTMAP_ORIGIN)] = &InjectLightmapOrigin;
            map[std::string(glimmer::BUILTIN_LIGHTMAP_SIZE)] = &InjectLightmapSize;
            map[std::string(glimmer::BUILTIN_TIME_OF_DAY)] = &InjectTimeOfDay;
            map[std::string(glimmer::BUILTIN_AMBIENT_COLOR)] = &InjectAmbientColor;
            map[std::string(glimmer::BUILTIN_MOON_PHASE)] = &InjectMoonPhase;
            map[std::string(glimmer::BUILTIN_WEATHER_INTENSITY)] = &InjectWeatherIntensity;
            map[std::string(glimmer::BUILTIN_SKY_TOP_COLOR)] = &InjectSkyTopColor;
            map[std::string(glimmer::BUILTIN_SKY_HORIZON_COLOR)] = &InjectSkyHorizonColor;
            return map;
        }();
        return injectors;
    }
}

void glimmer::UniformInjectorRegistry::Register(const std::string_view builtin, const UniformInjector injector) {
    GetInjectorMap()[std::string(builtin)] = injector;
}

glimmer::UniformInjector glimmer::UniformInjectorRegistry::Find(const std::string_view builtin) {
    const InjectorMap &map = GetInjectorMap();
    const auto it = map.find(builtin);
    return it != map.end() ? it->second : nullptr;
}
