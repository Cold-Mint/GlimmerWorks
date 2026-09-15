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
                                  const auto it = tileLightData_.find(position);
                                  if (it == tileLightData_.end() || it->second == nullptr) {
                                      return 1.0F;
                                  }
                                  return it->second->GetSideLightTransmission(layerType);
                              },
                              [this, layerType, &source](const TileVector2D &position, const float accumulated) {
                                  SetLightContributionAt(position, layerType, source, accumulated);
                              });
}

void glimmer::LightBuffer::ClearLightFromSource(const LightSource &source, const TileLayerType layerType) {
    const TileVector2D &center = source.GetCenter();
    LightFloodFill::Propagate(center, source.GetMaxRadius(),
                              [this, layerType](const TileVector2D &position) {
                                  const auto it = tileLightData_.find(position);
                                  if (it == tileLightData_.end() || it->second == nullptr) {
                                      return 1.0F;
                                  }
                                  return it->second->GetSideLightTransmission(layerType);
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
    ++revision_;
    LogCat::d("light_buffer_rebuild_all", "Rebuilt all light: source count={}, revision={}", sources.size(),
              revision_);
}

void glimmer::LightBuffer::SetSideLightMask(const TileVector2D position, const TileLayerType layerType,
                                            std::unique_ptr<LightMask> sideLightMask) {
    if (sideLightMask == nullptr) {
        return;
    }
    TileLightData &tileLightData = GetOrCreate(position);
    const LightMask *oldMask = tileLightData.GetSideLightMask(layerType);
    const float oldStrength = oldMask != nullptr ? oldMask->GetBlockingStrength() : 0.0F;
    const float newStrength = sideLightMask->GetBlockingStrength();
    tileLightData.SetSideLightMask(layerType, std::move(sideLightMask));
    ++revision_;
    if (oldStrength != newStrength) {
        MarkLightDirty();
    }
}

void glimmer::LightBuffer::SetBackLightMask(const TileVector2D position, const TileLayerType layerType,
                                            std::unique_ptr<LightMask> backLightMask) {
    if (backLightMask == nullptr) {
        return;
    }
    TileLightData &tileLightData = GetOrCreate(position);
    const LightMask *oldMask = tileLightData.GetBackLightMask(layerType);
    const float oldStrength = oldMask != nullptr ? oldMask->GetBlockingStrength() : 0.0F;
    const float newStrength = backLightMask->GetBlockingStrength();
    tileLightData.SetBackLightMask(layerType, std::move(backLightMask));
    tileLightData.RecalculateLight();
    ++revision_;
    if (oldStrength != newStrength) {
        MarkLightDirty();
    }
    if (layerType == TileLayerType::Ground) {
        UpdateColumnSkyTopY(position, oldStrength, newStrength);
    }
}

void glimmer::LightBuffer::ClearSideLightMask(const TileVector2D &position, const TileLayerType layerType) {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return;
    }
    const LightMask *oldMask = it->second->GetSideLightMask(layerType);
    const float oldStrength = oldMask != nullptr ? oldMask->GetBlockingStrength() : 0.0F;
    it->second->ClearSideLightMask(layerType);
    ++revision_;
    if (oldStrength != 0.0F) {
        MarkLightDirty();
    }
}

void glimmer::LightBuffer::ClearBackLightMask(const TileVector2D &position, const TileLayerType layerType) {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return;
    }
    const LightMask *oldMask = it->second->GetBackLightMask(layerType);
    const float oldStrength = oldMask != nullptr ? oldMask->GetBlockingStrength() : 0.0F;
    it->second->ClearBackLightMask(layerType);
    it->second->RecalculateLight();
    ++revision_;
    if (oldStrength != 0.0F) {
        MarkLightDirty();
    }
    if (layerType == TileLayerType::Ground) {
        UpdateColumnSkyTopY(position, oldStrength, 0.0F);
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
        wasGroundOpaque = it->second->GetBackLightBlockingStrength(TileLayerType::Ground) >= 1.0F;
    }
    tileLightData_.erase(position);
    if (wasGroundOpaque) {
        const auto skyIt = columnSkyTopY_.find(position.x);
        if (skyIt != columnSkyTopY_.end() && skyIt->second == position.y) {
            RecalculateColumnSkyTopY(position.x);
        }
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

void glimmer::LightBuffer::SetLightSource(const TileVector2D position, const TileLayerType layerType,
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

void glimmer::LightBuffer::ClearLightSource(const TileVector2D position, const TileLayerType layerType) {
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

glimmer::Color glimmer::LightBuffer::GetFinalLightColor(const TileVector2D position) const {
    const auto it = tileLightData_.find(position);
    const TileLightData *tileData = (it != tileLightData_.end()) ? it->second.get() : nullptr;
    const Color *point = tileData != nullptr ? tileData->GetFinalLightColor() : nullptr;
    const float srcR = point != nullptr ? static_cast<float>(point->r) / 255.0F : 0.0F;
    const float srcG = point != nullptr ? static_cast<float>(point->g) / 255.0F : 0.0F;
    const float srcB = point != nullptr ? static_cast<float>(point->b) / 255.0F : 0.0F;
    const float srcA = point != nullptr ? static_cast<float>(point->a) / 255.0F : 0.0F;

    //Screen ambient light from the background layer, attenuated by the
    //background wall's back-light blocking strength.
    //来自背景层的屏幕光，被背景墙背光挡光强度衰减。
    const float backOpacity = tileData != nullptr
                                  ? tileData->GetBackLightBlockingStrength(TileLayerType::BackGround)
                                  : 0.0F;
    const float screenScale = static_cast<float>(screenLight_.a) / 255.0F * (1.0F - backOpacity);

    //Sky ambient light from above, attenuated by depth below the column's
    //opaque ceiling using inverse-square falloff.
    //来自上方的天光，按列天花板以下深度做平方反比衰减。
    const float skyScale = (static_cast<float>(skyLight_.a) / 255.0F) * GetSkyFactor(position);

    const float totalR = srcR * srcA + static_cast<float>(screenLight_.r) / 255.0F * screenScale +
                         static_cast<float>(skyLight_.r) / 255.0F * skyScale;
    const float totalG = srcG * srcA + static_cast<float>(screenLight_.g) / 255.0F * screenScale +
                         static_cast<float>(skyLight_.g) / 255.0F * skyScale;
    const float totalB = srcB * srcA + static_cast<float>(screenLight_.b) / 255.0F * screenScale +
                         static_cast<float>(skyLight_.b) / 255.0F * skyScale;
    const float totalA = std::max(srcA, std::max(screenScale, skyScale));

    Color result{};
    if (totalA <= 0.0F) {
        return result;
    }
    result.r = static_cast<uint8_t>(std::min(255.0F, totalR / totalA * 255.0F));
    result.g = static_cast<uint8_t>(std::min(255.0F, totalG / totalA * 255.0F));
    result.b = static_cast<uint8_t>(std::min(255.0F, totalB / totalA * 255.0F));
    result.a = static_cast<uint8_t>(std::min(255.0F, totalA * 255.0F));
    return result;
}

void glimmer::LightBuffer::SetDynamicLight(const uint64_t id, const TileVector2D position,
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

void glimmer::LightBuffer::SetAmbientLight(const Color &screenLight, const Color &skyLight, const int skyMaxDepth) {
    const int maxDepth = skyMaxDepth > 0 ? skyMaxDepth : SKY_HEIGHT;
    if (screenLight_.r == screenLight.r && screenLight_.g == screenLight.g &&
        screenLight_.b == screenLight.b && screenLight_.a == screenLight.a &&
        skyLight_.r == skyLight.r && skyLight_.g == skyLight.g &&
        skyLight_.b == skyLight.b && skyLight_.a == skyLight.a &&
        skyMaxDepth_ == maxDepth) {
        return;
    }
    screenLight_ = screenLight;
    skyLight_ = skyLight;
    skyMaxDepth_ = maxDepth;
    ++revision_;
    LogCat::d("light_buffer_set_ambient",
              "SetAmbientLight: screen rgba=({},{},{},{}), sky rgba=({},{},{},{}), skyMaxDepth={}",
              static_cast<int>(screenLight_.r), static_cast<int>(screenLight_.g),
              static_cast<int>(screenLight_.b), static_cast<int>(screenLight_.a),
              static_cast<int>(skyLight_.r), static_cast<int>(skyLight_.g),
              static_cast<int>(skyLight_.b), static_cast<int>(skyLight_.a), skyMaxDepth_);
}

float glimmer::LightBuffer::GetSkyFactor(const TileVector2D &position) const {
    const auto it = columnSkyTopY_.find(position.x);
    const int topY = it != columnSkyTopY_.end() ? it->second : WORLD_MIN_Y - 1;
    const int depth = topY - position.y;
    if (depth <= 0) {
        return 1.0F;
    }
    if (skyMaxDepth_ <= 0) {
        return 0.0F;
    }
    const float normalized = static_cast<float>(depth) / static_cast<float>(skyMaxDepth_);
    return std::clamp(1.0F / (1.0F + normalized * normalized * 4.0F), 0.0F, 1.0F);
}

int glimmer::LightBuffer::GetColumnSkyTopY(const int x) const {
    const auto it = columnSkyTopY_.find(x);
    return it != columnSkyTopY_.end() ? it->second : WORLD_MIN_Y - 1;
}

uint64_t glimmer::LightBuffer::GetRevision() const {
    return revision_;
}

void glimmer::LightBuffer::RecalculateColumnSkyTopY(const int x) {
    for (int y = WORLD_MAX_Y - 1; y >= WORLD_MIN_Y; --y) {
        const auto it = tileLightData_.find(TileVector2D(x, y));
        if (it != tileLightData_.end() && it->second != nullptr &&
            it->second->GetBackLightBlockingStrength(TileLayerType::Ground) > 0.0F) {
            columnSkyTopY_[x] = y;
            return;
        }
    }
    columnSkyTopY_[x] = WORLD_MIN_Y - 1;
}

void glimmer::LightBuffer::MarkLightDirty() {
    if (batching_) {
        batchDirty_ = true;
    } else {
        RebuildAllLight();
    }
}

void glimmer::LightBuffer::UpdateColumnSkyTopY(const TileVector2D &position, const float oldStrength,
                                               const float newStrength) {
    const bool wasBlocking = oldStrength > 0.0F;
    const bool nowBlocks = newStrength > 0.0F;
    if (wasBlocking == nowBlocks) {
        return;
    }
    if (nowBlocks) {
        const auto it = columnSkyTopY_.find(position.x);
        const int currentTop = it != columnSkyTopY_.end() ? it->second : WORLD_MIN_Y - 1;
        if (position.y > currentTop) {
            columnSkyTopY_[position.x] = position.y;
        }
    } else {
        const auto it = columnSkyTopY_.find(position.x);
        if (it != columnSkyTopY_.end() && it->second == position.y) {
            RecalculateColumnSkyTopY(position.x);
        }
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
