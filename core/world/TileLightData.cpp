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
#include "TileLightData.h"

#include "core/Constants.h"
#include "core/utils/LightUtils.h"
#include "src/core/vector2d.pb.h"


void glimmer::TileLightData::ApplyBackLightMask(Color* finalLightColor, TileLayerType tileLayer)
{
    const auto backLightMaskIterator = backLightMaskData_.find(tileLayer);
    if (backLightMaskIterator == backLightMaskData_.end())
    {
        return;
    }
    const auto& backLightMaskUnique = backLightMaskIterator->second;
    if (backLightMaskUnique == nullptr)
    {
        return;
    }
    const Color* lightMaskColor = backLightMaskUnique->GetLightMaskColor();
    if (lightMaskColor == nullptr)
    {
        return;
    }
    *finalLightColor = *LightUtils::ApplyLightingMask(finalLightColor, lightMaskColor,
                                                      backLightMaskUnique->GetTintFactor());
}

std::unique_ptr<glimmer::Color> glimmer::TileLightData::ComputeFinalLightColor()
{
    if (lightContributions_.empty())
    {
        return nullptr;
    }
    auto finalLightColor = std::make_unique<Color>();
    bool hasFoundLightSource = false;
    for (int i = 0; i < TILE_LAYER_TYPE_COUNT; ++i)
    {
        auto tileLayer = static_cast<TileLayerType>(1 << i);
        //Apply the shadow effect.
        //叠加阴影。
        ApplyBackLightMask(finalLightColor.get(), tileLayer);
        const auto lightContributionIterator = lightContributions_.find(tileLayer);
        if (lightContributionIterator == lightContributions_.end())
        {
            continue;
        }
        const auto& lightContributionVector = lightContributionIterator->second;
        if (lightContributionVector.empty())
        {
            continue;
        }
        for (const auto& lightContribution : lightContributionVector)
        {
            const auto lightContributionUnique = lightContribution.get();
            if (lightContributionUnique == nullptr)
            {
                continue;
            }
            const Color* lightColor = lightContributionUnique->GetLightColor();
            if (lightColor == nullptr)
            {
                continue;
            }
            if (!hasFoundLightSource)
            {
                finalLightColor->a = lightColor->a;
                finalLightColor->r = lightColor->r;
                finalLightColor->g = lightColor->g;
                finalLightColor->b = lightColor->b;
                hasFoundLightSource = true;
                continue;
            }
            finalLightColor = LightUtils::MixLights(finalLightColor.get(), lightColor);
        }
    }
    if (hasFoundLightSource)
    {
        if (finalLightColor->a == 0)
        {
            return nullptr;
        }
        return finalLightColor;
    }
    return nullptr;
}


void glimmer::TileLightData::SetLightMaskImp(
    std::unordered_map<TileLayerType, std::unique_ptr<LightMask>>& lightMaskData, TileLayerType layerType,
    std::unique_ptr<LightMask> lightMask)
{
    if (lightMask == nullptr)
    {
        return;
    }
    lightMaskData[layerType] = std::move(lightMask);
}

const glimmer::LightMask* glimmer::TileLightData::GetLightMaskImp(
    std::unordered_map<TileLayerType, std::unique_ptr<LightMask>>& lightMaskData, const TileLayerType layerType)
{
    const auto lightMaskIterator = lightMaskData.find(layerType);
    if (lightMaskIterator == lightMaskData.end())
    {
        return nullptr;
    }
    const auto& lightMaskPtr = lightMaskIterator->second;
    if (lightMaskPtr == nullptr)
    {
        return nullptr;
    }
    return lightMaskPtr.get();
}


void glimmer::TileLightData::SetLightContribution(const TileLayerType layerType,
                                                  std::unique_ptr<LightContribution> contribution)
{
    if (contribution == nullptr)
    {
        return;
    }
    const LightSource* targetLightSource = contribution->GetLightSource();
    if (targetLightSource == nullptr)
    {
        return;
    }
    const Color* newLightColor = contribution->GetLightColor();
    if (newLightColor == nullptr)
    {
        return;
    }

    const auto lightContributionIterator = lightContributions_.find(layerType);
    if (lightContributionIterator == lightContributions_.end())
    {
        lightContributions_[layerType].emplace_back(std::move(contribution));
        finalLightColor_ = ComputeFinalLightColor();
        return;
    }

    auto& lightContributionVector = lightContributionIterator->second;
    if (lightContributionVector.empty())
    {
        lightContributionVector.emplace_back(std::move(contribution));
        finalLightColor_ = ComputeFinalLightColor();
        return;
    }
    bool foundDuplicate = false;
    bool colorChanged = false;
    for (const auto& lightContribution : lightContributionVector)
    {
        const auto lightContributionUnique = lightContribution.get();
        if (lightContributionUnique == nullptr)
        {
            continue;
        }

        if (targetLightSource == lightContribution->GetLightSource())
        {
            foundDuplicate = true;

            if (const Color* oldLightColor = lightContributionUnique->GetLightColor(); oldLightColor == nullptr ||
                newLightColor->a != oldLightColor->a ||
                newLightColor->r != oldLightColor->r ||
                newLightColor->g != oldLightColor->g ||
                newLightColor->b != oldLightColor->b)
            {
                lightContribution->SetLightColor(contribution->TakeLightColor());
                colorChanged = true;
            }
            break;
        }
    }
    if (foundDuplicate)
    {
        if (colorChanged)
        {
            // Find duplicates, the color has changed. Only recalculate, do not add new ones.
            // 找到重复，颜色变了，只重算，不新增
            finalLightColor_ = ComputeFinalLightColor();
        }
    }
    else
    {
        //No duplicates were found. New lights were added, so a recalculation is necessary.
        //没找到重复，新增灯光，必须重算
        lightContributionVector.emplace_back(std::move(contribution));
        finalLightColor_ = ComputeFinalLightColor();
    }
}

void glimmer::TileLightData::RecalculateLight()
{
    finalLightColor_ = ComputeFinalLightColor();
}

const std::unordered_map<glimmer::TileLayerType, std::vector<std::unique_ptr<glimmer::LightContribution>>>* glimmer::
TileLightData::GetLightContributions() const
{
    return &lightContributions_;
}

const std::unordered_map<glimmer::TileLayerType, std::unique_ptr<glimmer::LightSource>>* glimmer::TileLightData::
GetLightSources() const
{
    return &lightSourceData_;
}

const std::unordered_map<glimmer::TileLayerType, std::unique_ptr<glimmer::LightMask>>* glimmer::TileLightData::
GetSideLightMasks() const
{
    return &sideLightMaskData_;
}

const std::unordered_map<glimmer::TileLayerType, std::unique_ptr<glimmer::LightMask>>* glimmer::TileLightData::
GetBackLightMasks() const
{
    return &backLightMaskData_;
}

const glimmer::LightContribution* glimmer::TileLightData::GetLightContribution(const TileLayerType layerType,
                                                                               const LightSource* targetLightSource)
{
    if (targetLightSource == nullptr)
    {
        return nullptr;
    }
    const auto lightContributionIterator = lightContributions_.find(layerType);
    if (lightContributionIterator == lightContributions_.end())
    {
        return nullptr;
    }

    const auto& lightContributionVector = lightContributionIterator->second;
    if (lightContributionVector.empty())
    {
        return nullptr;
    }
    for (const auto& lightContribution : lightContributionVector)
    {
        if (lightContribution == nullptr)
        {
            continue;
        }
        if (targetLightSource == lightContribution->GetLightSource())
        {
            return lightContribution.get();
        }
    }
    return nullptr;
}

std::vector<const glimmer::LightContribution*> glimmer::TileLightData::GetLightContributionVector(
    const TileLayerType layerType)
{
    std::vector<const LightContribution*> result = {};
    const auto lightContributionIterator = lightContributions_.find(layerType);
    if (lightContributionIterator == lightContributions_.end())
    {
        return result;
    }

    const auto& lightContributionVector = lightContributionIterator->second;
    if (lightContributionVector.empty())
    {
        return result;
    }

    for (const auto& lightContribution : lightContributionVector)
    {
        if (lightContribution == nullptr)
        {
            continue;
        }
        result.emplace_back(lightContribution.get());
    }
    return result;
}

void glimmer::TileLightData::SetLightSource(const TileLayerType layerType, std::unique_ptr<LightSource> lightSource)
{
    if (lightSource == nullptr)
    {
        return;
    }
    lightSourceData_[layerType] = std::move(lightSource);
}

void glimmer::TileLightData::SetSideLightMask(const TileLayerType layerType, std::unique_ptr<LightMask> lightMask)
{
    SetLightMaskImp(sideLightMaskData_, layerType, std::move(lightMask));
}

void glimmer::TileLightData::SetBackLightMask(const TileLayerType layerType, std::unique_ptr<LightMask> lightMask)
{
    SetLightMaskImp(backLightMaskData_, layerType, std::move(lightMask));
}

const glimmer::LightMask* glimmer::TileLightData::GetSideLightMask(const TileLayerType layerType)
{
    return GetLightMaskImp(sideLightMaskData_, layerType);
}

const glimmer::LightMask* glimmer::TileLightData::GetBackLightMask(const TileLayerType layerType)
{
    return GetLightMaskImp(backLightMaskData_, layerType);
}


const glimmer::LightSource* glimmer::TileLightData::GetLightSource(const TileLayerType layerType)
{
    auto lightSourceIterator = lightSourceData_.find(layerType);
    if (lightSourceIterator == lightSourceData_.end())
    {
        return nullptr;
    }
    const auto& lightMaskPtr = lightSourceIterator->second;
    if (lightMaskPtr == nullptr)
    {
        return nullptr;
    }
    return lightMaskPtr.get();
}

void glimmer::TileLightData::ClearSideLightMask(const TileLayerType layerType)
{
    sideLightMaskData_.erase(layerType);
}

void glimmer::TileLightData::ClearBackLightMask(const TileLayerType layerType)
{
    backLightMaskData_.erase(layerType);
}


void glimmer::TileLightData::ClearLightSource(const TileLayerType layerType)
{
    lightSourceData_.erase(layerType);
}

const glimmer::Color* glimmer::TileLightData::GetFinalLightColor() const
{
    return finalLightColor_.get();
}

void glimmer::TileLightData::ClearLightContribution(const TileLayerType layerType, const LightSource* lightSource,
                                                    const int rayIndex)
{
    if (lightSource == nullptr)
    {
        return;
    }
    const auto lightContributionIterator = lightContributions_.find(layerType);
    if (lightContributionIterator == lightContributions_.end())
    {
        return;
    }
    auto& lightContributionVector = lightContributionIterator->second;
    if (lightContributionVector.empty())
    {
        return;
    }
    bool hasRemoved = false;
    for (auto iter = lightContributionVector.cbegin(); iter != lightContributionVector.cend();)
    {
        const auto& lightContribution = *iter;
        if (lightContribution == nullptr)
        {
            ++iter;
            continue;
        }

        if (lightSource == lightContribution->GetLightSource())
        {
            if (lightContribution->GetRayIndex() == LIGHT_CONTRIBUTION_CENTER_RAY_INDEX
                || lightContribution->GetRayIndex() == rayIndex)
            {
                lightContributionVector.erase(iter);
                hasRemoved = true;
            }
            break;
        }
        ++iter;
    }
    if (hasRemoved)
    {
        finalLightColor_ = ComputeFinalLightColor();
    }
}
