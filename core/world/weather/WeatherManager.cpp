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
#include "WeatherManager.h"

#include <map>
#include <utility>

#include "WeatherConditionProcessors.h"
#include "core/context/AppContext.h"
#include "core/context/ModContext.h"
#include "core/mod/Resource.h"
#include "core/mod/dataPack/SkyElementRegistry.h"
#include "core/mod/dataPack/WeatherConditionRegistry.h"
#include "core/mod/dataPack/WeatherRegistry.h"
#include "core/utils/RandomUtils.h"
#include "core/world/Dimension.h"
#include "core/world/WorldContext.h"

glimmer::WeatherManager::WeatherManager(WorldContext *worldContext, Dimension *dimension)
    : worldContext_(worldContext), dimension_(dimension) {
    conditionProcessorManager_.AddProcessor(std::make_unique<TimeOfDayWeatherCondition>());
    conditionProcessorManager_.AddProcessor(std::make_unique<MoonPhaseWeatherCondition>());
    conditionProcessorManager_.AddProcessor(std::make_unique<WeatherIntensityWeatherCondition>());
}

float glimmer::WeatherManager::PickNextIntensity() const {
    if (dimension_ == nullptr || dimension_->GetDimensionResource() == nullptr) {
        return 0.0F;
    }
    const auto &levels = dimension_->GetDimensionResource()->weatherIntensityLevels;
    if (levels.empty()) {
        return 0.0F;
    }
    float totalWeight = 0.0F;
    for (const auto &level: levels) {
        totalWeight += level.weight;
    }
    if (totalWeight <= 0.0F) {
        return levels.front().value;
    }
    float roll = RandomUtils::Random<float>(0.0F, totalWeight);
    for (const auto &level: levels) {
        roll -= level.weight;
        if (roll <= 0.0F) {
            return level.value;
        }
    }
    return levels.back().value;
}

float glimmer::WeatherManager::PickNextDuration() const {
    if (dimension_ == nullptr || dimension_->GetDimensionResource() == nullptr) {
        return 60.0F;
    }
    const auto *resource = dimension_->GetDimensionResource();
    return RandomUtils::Random<float>(resource->weatherIntensityMinDuration, resource->weatherIntensityMaxDuration);
}

void glimmer::WeatherManager::Update(const float delta) {
    if (worldContext_ == nullptr || !worldContext_->IsRuning()) {
        return;
    }
    if (delta <= 0.0F) {
        return;
    }
    weatherTimer_ -= delta;
    if (weatherTimer_ <= 0.0F) {
        weatherIntensity_ = PickNextIntensity();
        weatherTimer_ = PickNextDuration();
    }
}

float glimmer::WeatherManager::GetWeatherIntensity() const {
    return weatherIntensity_;
}

float glimmer::WeatherManager::GetWeatherTimer() const {
    return weatherTimer_;
}

void glimmer::WeatherManager::SetWeatherIntensity(const float intensity) {
    weatherIntensity_ = intensity;
}

void glimmer::WeatherManager::SetWeatherTimer(const float timer) {
    weatherTimer_ = timer;
}

bool glimmer::WeatherManager::MatchesWeather(const WeatherContext &ctx, const WeatherResource *weather) const {
    if (weather == nullptr || worldContext_ == nullptr || worldContext_->GetAppContext() == nullptr) {
        return false;
    }
    ModContext *modContext = worldContext_->GetAppContext()->GetModContext();
    if (modContext == nullptr) {
        return false;
    }
    WeatherConditionRegistry *conditionRegistry = modContext->GetWeatherConditionRegistry();
    if (conditionRegistry == nullptr) {
        return false;
    }
    for (const auto &conditionRef: weather->conditions) {
        WeatherConditionResource *condition = conditionRegistry->Find(conditionRef.GetPackageId(),
                                                                      conditionRef.GetResourceKey());
        if (condition == nullptr) {
            return false;
        }
        IWeatherCondition *processor = conditionProcessorManager_.FindProcessor(
            static_cast<WeatherConditionType>(condition->type));
        if (processor == nullptr) {
            return false;
        }
        if (!processor->Match(ctx, condition)) {
            return false;
        }
    }
    return true;
}

std::vector<const glimmer::SkyElementResource *> glimmer::WeatherManager::GetActiveElements() {
    std::vector<const SkyElementResource *> result;
    if (worldContext_ == nullptr || dimension_ == nullptr || dimension_->GetDimensionResource() == nullptr) {
        return result;
    }
    AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr || appContext->GetModContext() == nullptr) {
        return result;
    }
    ModContext *modContext = appContext->GetModContext();
    WeatherRegistry *weatherRegistry = modContext->GetWeatherRegistry();
    SkyElementRegistry *skyElementRegistry = modContext->GetSkyElementRegistry();
    if (weatherRegistry == nullptr || skyElementRegistry == nullptr) {
        return result;
    }

    WeatherContext ctx;
    ctx.timeOfDay = dimension_->GetTimeOfDay();
    ctx.moonPhase = worldContext_->GetMoonPhase();
    ctx.weatherIntensity = weatherIntensity_;

    std::map<std::pair<uint8_t, uint8_t>, const SkyElementResource *> grouped;
    for (const auto &weatherRef: dimension_->GetDimensionResource()->weathers) {
        WeatherResource *weather = weatherRegistry->Find(weatherRef.GetPackageId(), weatherRef.GetResourceKey());
        if (weather == nullptr || !MatchesWeather(ctx, weather)) {
            continue;
        }
        for (const auto &elementRef: weather->elements) {
            SkyElementResource *element = skyElementRegistry->Find(elementRef.GetPackageId(),
                                                                   elementRef.GetResourceKey());
            if (element == nullptr) {
                continue;
            }
            const auto key = std::make_pair(element->slot, element->animationType);
            const auto it = grouped.find(key);
            if (it == grouped.end() || element->priority >= it->second->priority) {
                grouped[key] = element;
            }
        }
    }

    result.reserve(grouped.size());
    for (const auto &[key, element]: grouped) {
        (void) key;
        result.push_back(element);
    }
    return result;
}
