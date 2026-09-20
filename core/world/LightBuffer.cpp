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
#include "LightBuffer.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "LightFloodFill.h"
#include "core/log/LogCat.h"
#include "core/math/TileVector2D.h"
#include "core/utils/LightUtils.h"


glimmer::TileLightData &glimmer::LightBuffer::GetOrCreate(const TileVector2D &position) {
    auto &slot = tileLightData_[position];
    if (slot == nullptr) {
        slot = std::make_unique<TileLightData>();
    }
    return *slot;
}

void glimmer::LightBuffer::SetLightFromSource(const LightSource &source, const TileLayerType layerType) {
    const TileVector2D &center = source.GetCenter();
    LightFloodFill::Propagate(center, source.GetMaxRadius(),
                              [this, layerType](const TileVector2D &position) {
                                  return GetLightBlockingStrength(position, layerType, LightDirection::Radial);
                              },
                              [this, layerType, &source](const TileVector2D &position, const float accumulated) {
                                  SetLightContributionAt(position, layerType, source, accumulated);
                              });
}

void glimmer::LightBuffer::ClearLightFromSource(const LightSource &source, const TileLayerType layerType) {
    const TileVector2D &center = source.GetCenter();
    LightFloodFill::Propagate(center, source.GetMaxRadius(),
                              [this, layerType](const TileVector2D &position) {
                                  return GetLightBlockingStrength(position, layerType, LightDirection::Radial);
                              },
                              [this, layerType, &source](const TileVector2D &position, float) {
                                  ClearLightContributionAt(position, layerType, source);
                              });
}

void glimmer::LightBuffer::SetLightContributionAt(const TileVector2D &position, const TileLayerType layerType,
                                                  const LightSource &source, const float accumulated) {
    const TileVector2D &center = source.GetCenter();
    const float factor = source.GetAttenuationFactor(position.x - center.x, position.y - center.y) * accumulated;
    if (factor <= 0.0F) {
        return;
    }
    const Color *emission = source.GetEmissionColor();
    auto contribution = std::make_unique<LightContribution>();
    auto lightColor = std::make_unique<Color>(
        emission->r,
        emission->g,
        emission->b,
        static_cast<uint8_t>(255.0F * factor));
    contribution->SetLightColor(std::move(lightColor));
    contribution->SetLightSource(&source);
    GetOrCreate(position).SetLightContribution(layerType, std::move(contribution));
}

void glimmer::LightBuffer::ClearLightContributionAt(const TileVector2D &position, const TileLayerType layerType,
                                                    const LightSource &source) {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return;
    }
    it->second->ClearLightContribution(layerType, &source);
}

void glimmer::LightBuffer::SetAmbientLightContributionAt(const TileVector2D &position, const TileLayerType layerType,
                                                         const LightSource &source,
                                                         std::unique_ptr<Color> lightColor) {
    if (lightColor == nullptr || lightColor->a == 0) {
        return;
    }
    auto contribution = std::make_unique<LightContribution>();
    contribution->SetLightColor(std::move(lightColor));
    contribution->SetLightSource(&source);
    GetOrCreate(position).SetLightContribution(layerType, std::move(contribution));
}

void glimmer::LightBuffer::RebuildAmbientLight() {
    const Color *backColor = backLightSource_.GetEmissionColor();
    const Color *skyColor = skyLightSource_.GetEmissionColor();
    for (const auto &[position, tileData]: tileLightData_) {
        if (tileData == nullptr) {
            continue;
        }
        for (int i = 0; i < TILE_LAYER_TYPE_COUNT; ++i) {
            const auto layerType = static_cast<TileLayerType>(1 << i);
            tileData->ClearLightContribution(layerType, &backLightSource_);
            tileData->ClearLightContribution(layerType, &skyLightSource_);
        }
    }
    for (const auto &[position, tileData]: tileLightData_) {
        if (tileData == nullptr) {
            continue;
        }
        for (int i = 0; i < TILE_LAYER_TYPE_COUNT; ++i) {
            const auto layerType = static_cast<TileLayerType>(1 << i);
            //背光（z 方向）照所有图层，各层用各自的背光遮照遮挡。
            if (backColor != nullptr && backColor->a > 0) {
                const LightMask *backMask = tileData->GetLightMask(layerType, LightDirection::Backward);
                std::unique_ptr<Color> color;
                if (backMask != nullptr) {
                    color = LightUtils::ApplyLightingMask(backColor, backMask->GetLightMaskColor(),
                                                          backMask->GetTintFactor());
                } else {
                    color = std::make_unique<Color>(backColor->r, backColor->g, backColor->b, backColor->a);
                }
                SetAmbientLightContributionAt(position, layerType, backLightSource_, std::move(color));
            }
            //天光（y 负方向）仅地面层，被侧面遮照遮挡。
            if (layerType == TileLayerType::Ground && skyColor != nullptr && skyColor->a > 0) {
                const float transmittance = GetSkyTransmittance(position);
                if (transmittance > 0.0F) {
                    auto color = std::make_unique<Color>(
                        skyColor->r, skyColor->g, skyColor->b,
                        static_cast<uint8_t>(std::clamp(static_cast<float>(skyColor->a) * transmittance, 0.0F, 255.0F)));
                    SetAmbientLightContributionAt(position, layerType, skyLightSource_, std::move(color));
                }
            }
        }
    }
}

void glimmer::LightBuffer::RebuildAllLight() {
    std::vector<std::pair<TileLayerType, const LightSource *> > sources;
    for (const auto &[position, tileData]: tileLightData_) {
        if (tileData == nullptr) {
            continue;
        }
        const auto *lightSources = tileData->GetLightSources();
        if (lightSources == nullptr) {
            continue;
        }
        for (const auto &[layerType, lightSource]: *lightSources) {
            if (lightSource != nullptr) {
                sources.emplace_back(layerType, lightSource.get());
            }
        }
    }
    for (const auto &[id, entry]: dynamicLights_) {
        if (entry.lightSource != nullptr) {
            sources.emplace_back(entry.layer, entry.lightSource.get());
        }
    }
    for (const auto &[position, tileData]: tileLightData_) {
        if (tileData != nullptr) {
            tileData->ClearAllLightContributions();
        }
    }
    for (const auto &[layerType, lightSource]: sources) {
        SetLightFromSource(*lightSource, layerType);
    }
    RebuildAmbientLight();
    ++revision_;
    LogCat::d("light_buffer_rebuild_all", "Rebuilt all light: source count={}, revision={}", sources.size(),
              revision_);
}

void glimmer::LightBuffer::SetLightMask(const TileVector2D &position, const TileLayerType layerType,
                                        const LightDirection direction,
                                        std::unique_ptr<LightMask> lightMask) {
    if (lightMask == nullptr) {
        return;
    }
    //Normalize the light direction to a mask slot: Backward keeps its own back
    //mask, while Radial/Downward share the side (sky) mask.
    //将光照方向归一化到遮照槽：背光(Backward)用背光遮照，点光/天光(Radial/Downward)共用侧面遮照。
    const LightDirection maskDirection = direction == LightDirection::Backward ? LightDirection::Backward : LightDirection::Downward;
    TileLightData &tileLightData = GetOrCreate(position);
    const LightMask *oldMask = tileLightData.GetLightMask(layerType, maskDirection);
    const float oldStrength = oldMask != nullptr ? oldMask->GetBlockingStrength() : 0.0F;
    const float newStrength = lightMask->GetBlockingStrength();
    tileLightData.SetLightMask(layerType, maskDirection, std::move(lightMask));
    ++revision_;
    //Only the side mask on the Ground layer participates in sky occlusion.
    //只有地面层的侧面遮照参与天光遮挡。
    if (maskDirection == LightDirection::Downward && layerType == TileLayerType::Ground) {
        if (oldStrength > 0.0F != newStrength > 0.0F) {
            UpdateColumnSkyOccluder(position, newStrength > 0.0F);
        }
    }
    if (oldStrength != newStrength) {
        MarkLightDirty();
    }
}

void glimmer::LightBuffer::ClearLightMask(const TileVector2D &position, const TileLayerType layerType,
                                          const LightDirection direction) {
    const LightDirection maskDirection = direction == LightDirection::Backward ? LightDirection::Backward : LightDirection::Downward;
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return;
    }
    const LightMask *oldMask = it->second->GetLightMask(layerType, maskDirection);
    const float oldStrength = oldMask != nullptr ? oldMask->GetBlockingStrength() : 0.0F;
    it->second->ClearLightMask(layerType, maskDirection);
    ++revision_;
    if (maskDirection == LightDirection::Downward && layerType == TileLayerType::Ground && oldStrength > 0.0F) {
        UpdateColumnSkyOccluder(position, false);
    }
    if (oldStrength != 0.0F) {
        MarkLightDirty();
    }
}

void glimmer::LightBuffer::ClearTileLightData(const TileVector2D &position) {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end()) {
        return;
    }
    bool wasGroundOpaque = false;
    if (it->second != nullptr) {
        const auto *lightSources = it->second->GetLightSources();
        std::vector<TileLayerType> layerTypesToClear;
        layerTypesToClear.reserve(lightSources->size());
        for (const auto &[layerType, lightSource]: *lightSources) {
            if (lightSource != nullptr) {
                layerTypesToClear.push_back(layerType);
            }
        }
        for (const auto layerType: layerTypesToClear) {
            ClearLightSource(position, layerType);
        }
        wasGroundOpaque = it->second->GetLightBlockingStrength(TileLayerType::Ground, LightDirection::Downward) > 0.0F;
    }
    tileLightData_.erase(position);
    if (wasGroundOpaque) {
        UpdateColumnSkyOccluder(position, false);
    }
    ++revision_;
}

const glimmer::TileLightData *glimmer::LightBuffer::GetTileLightData(const TileVector2D &position) const {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return nullptr;
    }
    return it->second.get();
}

void glimmer::LightBuffer::SetLightSource(const TileVector2D &position, const TileLayerType layerType,
                                          std::unique_ptr<LightSource> lightSource) {
    if (lightSource == nullptr) {
        return;
    }
    LightSource *lightSourcePtr = lightSource.get();
    SetLightFromSource(*lightSourcePtr, layerType);
    const Color *emission = lightSourcePtr->GetEmissionColor();
    LogCat::d("light_buffer_set_light_source",
              "Set light source: position=({}, {}), layer={}, radius={}, emission rgba=({},{},{},{})",
              position.x, position.y, static_cast<int>(layerType), lightSourcePtr->GetMaxRadius(),
              static_cast<int>(emission->r), static_cast<int>(emission->g),
              static_cast<int>(emission->b), static_cast<int>(emission->a));
    GetOrCreate(position).SetLightSource(layerType, std::move(lightSource));
    ++revision_;
}

void glimmer::LightBuffer::ClearLightSource(const TileVector2D &position, const TileLayerType layerType) {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return;
    }
    const LightSource *lightSourcePtr = it->second->GetLightSource(layerType);
    if (lightSourcePtr == nullptr) {
        return;
    }
    ClearLightFromSource(*lightSourcePtr, layerType);
    it->second->ClearLightSource(layerType);
    ++revision_;
}

glimmer::Color glimmer::LightBuffer::GetFinalLightColor(const TileVector2D &position) const {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return ComputeAmbientLightColor(position);
    }
    const TileLightData *tileData = it->second.get();
    const Color *point = tileData->GetFinalLightColor();
    if (point == nullptr) {
        return Color{};
    }
    Color result{};
    result.r = point->r;
    result.g = point->g;
    result.b = point->b;
    result.a = point->a;
    return result;
}

glimmer::Color glimmer::LightBuffer::ComputeAmbientLightColor(const TileVector2D &position) const {
    const Color *backColor = backLightSource_.GetEmissionColor();
    const Color *skyColor = skyLightSource_.GetEmissionColor();
    Color result{};
    bool hasLight = false;
    if (backColor != nullptr && backColor->a > 0) {
        result = *backColor;
        hasLight = true;
    }
    if (skyColor != nullptr && skyColor->a > 0) {
        const float transmittance = GetSkyTransmittance(position);
        if (transmittance > 0.0F) {
            const Color sky(
                skyColor->r,
                skyColor->g,
                skyColor->b,
                static_cast<uint8_t>(std::clamp(static_cast<float>(skyColor->a) * transmittance, 0.0F, 255.0F)));
            if (hasLight) {
                result = *LightUtils::MixLights(&result, &sky);
            } else {
                result = sky;
                hasLight = true;
            }
        }
    }
    if (!hasLight) {
        return Color{};
    }
    return result;
}

void glimmer::LightBuffer::SetDynamicLight(const uint64_t id, const TileVector2D &position,
                                           const TileLayerType layerType,
                                           std::unique_ptr<LightSource> lightSource) {
    if (lightSource == nullptr) {
        RemoveDynamicLight(id);
        return;
    }
    const auto it = dynamicLights_.find(id);
    if (it != dynamicLights_.end()) {
        const DynamicLightEntry &old = it->second;
        const LightSource *oldSource = old.lightSource.get();
        const LightSource *newSource = lightSource.get();
        const Color *oldColor = oldSource->GetEmissionColor();
        const Color *newColor = newSource->GetEmissionColor();
        const bool same = old.position == position && old.layer == layerType &&
                          oldSource->GetMaxRadius() == newSource->GetMaxRadius() &&
                          oldColor != nullptr && newColor != nullptr &&
                          oldColor->GetFingerprint() == newColor->GetFingerprint();
        if (same) {
            return;
        }
        ClearLightFromSource(*oldSource, old.layer);
        dynamicLights_.erase(it);
    }
    SetLightFromSource(*lightSource, layerType);
    DynamicLightEntry entry{position, layerType, std::move(lightSource)};
    dynamicLights_.emplace(id, std::move(entry));
    ++revision_;
    LogCat::d("light_buffer_set_dynamic_light", "Set dynamic light: id={}, position=({}, {})", id, position.x,
              position.y);
}

void glimmer::LightBuffer::RemoveDynamicLight(const uint64_t id) {
    const auto it = dynamicLights_.find(id);
    if (it == dynamicLights_.end()) {
        return;
    }
    if (it->second.lightSource != nullptr) {
        ClearLightFromSource(*it->second.lightSource, it->second.layer);
    }
    dynamicLights_.erase(it);
    ++revision_;
    LogCat::d("light_buffer_remove_dynamic_light", "Removed dynamic light: id={}", id);
}

float glimmer::LightBuffer::GetLightBlockingStrength(const TileVector2D &position, const TileLayerType layerType,
                                                     const LightDirection direction) const {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return 0.0F;
    }
    const LightDirection maskDirection = direction == LightDirection::Backward ? LightDirection::Backward : LightDirection::Downward;
    return it->second->GetLightBlockingStrength(layerType, maskDirection);
}

float glimmer::LightBuffer::GetSkyTransmittance(const TileVector2D &position) const {
    const auto it = columnSkyOccluders_.find(position.x);
    if (it == columnSkyOccluders_.end()) {
        //No sky-blocking tile in this column: full sky light reaches every tile.
        //该列没有阻挡天光的瓦片，全天光到达每个瓦片。
        return 1.0F;
    }
    //Sky light is a directional light cast from the top of the sky downward,
    //so it is accumulated from the highest occluder down to the queried tile.
    //天光是从天空顶部自上而下照射的方向光，因此从最高遮挡瓦片向下累积到查询瓦片。
    //Transmittance below one 1/255 step is treated as invisible: the final
    //alpha is quantized to uint8_t, so such a value already rounds to 0.
    //低于 1/255 的透射率视为不可见：最终 alpha 被量化为 uint8_t，该值已四舍五入为 0。
    constexpr float MIN_TRANSMITTANCE = 1.0F / 255.0F;
    float transmittance = 1.0F;
    for (auto occluderIt = it->second.rbegin(); occluderIt != it->second.rend(); ++occluderIt) {
        if (*occluderIt <= position.y) {
            break;
        }
        const TileVector2D occluder(position.x, *occluderIt);
        const float occlusion = GetLightBlockingStrength(occluder, TileLayerType::Ground, LightDirection::Downward);
        transmittance *= 1.0F - occlusion;
        if (transmittance <= MIN_TRANSMITTANCE) {
            return 0.0F;
        }
    }
    return transmittance;
}

int glimmer::LightBuffer::GetColumnSkyTopY(const int x) const {
    const auto it = columnSkyOccluders_.find(x);
    if (it == columnSkyOccluders_.end() || it->second.empty()) {
        return WORLD_MIN_Y - 1;
    }
    return *it->second.rbegin();
}

uint64_t glimmer::LightBuffer::GetRevision() const {
    return revision_;
}

const glimmer::Color *glimmer::LightBuffer::GetBackLightColor() const {
    return backLightSource_.GetEmissionColor();
}

const glimmer::Color *glimmer::LightBuffer::GetSkyLightColor() const {
    return skyLightSource_.GetEmissionColor();
}

const std::unordered_map<uint64_t, glimmer::DynamicLightEntry> *glimmer::LightBuffer::GetDynamicLights() const {
    return &dynamicLights_;
}

void glimmer::LightBuffer::UpdateColumnSkyOccluder(const TileVector2D &position, const bool nowBlocks) {
    auto &occluders = columnSkyOccluders_[position.x];
    if (nowBlocks) {
        occluders.insert(position.y);
    } else {
        occluders.erase(position.y);
    }
    if (occluders.empty()) {
        columnSkyOccluders_.erase(position.x);
    }
}

void glimmer::LightBuffer::MarkLightDirty() {
    if (batching_) {
        batchDirty_ = true;
    } else {
        RebuildAllLight();
    }
}

void glimmer::LightBuffer::BeginBatch() {
    LogCat::d("light_buffer_begin_batch", "light buffer begin batch");
    batching_ = true;
    batchDirty_ = false;
}

void glimmer::LightBuffer::EndBatch() {
    LogCat::d("light_buffer_end_batch", "light buffer end batch");
    batching_ = false;
    if (batchDirty_) {
        batchDirty_ = false;
        RebuildAllLight();
    }
}

void glimmer::LightBuffer::SetLightColor(const Color &backLight, const Color &skyLight) {
    const uint64_t newBackLightFingerprint = backLight.GetFingerprint();
    const uint64_t newSkyLightFingerprint = skyLight.GetFingerprint();
    const Color *backColor = backLightSource_.GetEmissionColor();
    const Color *skyColor = skyLightSource_.GetEmissionColor();
    if (backColor != nullptr && skyColor != nullptr &&
        backColor->GetFingerprint() == newBackLightFingerprint && skyColor->GetFingerprint() ==
        newSkyLightFingerprint) {
        return;
    }
    backLightSource_.SetEmissionColor(backLight);
    skyLightSource_.SetEmissionColor(skyLight);
    RebuildAmbientLight();
    ++revision_;
    LogCat::d("light_buffer_set_light_color",
              "SetLightColor: backLight rgba=({},{},{},{}), skyLight rgba=({},{},{},{})",
              static_cast<int>(backLight.r), static_cast<int>(backLight.g),
              static_cast<int>(backLight.b), static_cast<int>(backLight.a),
              static_cast<int>(skyLight.r), static_cast<int>(skyLight.g),
              static_cast<int>(skyLight.b), static_cast<int>(skyLight.a));
}
