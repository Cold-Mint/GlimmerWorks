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

#include <utility>
#include <vector>

#include "LightFloodFill.h"
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
                                      return false;
                                  }
                                  return it->second->IsOpaque(layerType);
                              },
                              [this, layerType, &source](const TileVector2D &position) {
                                  SetLightContributionAt(position, layerType, source);
                              });
}

void glimmer::LightBuffer::ClearLightFromSource(const LightSource &source, const TileLayerType layerType) {
    const TileVector2D &center = source.GetCenter();
    LightFloodFill::Propagate(center, source.GetMaxRadius(),
                              [this, layerType](const TileVector2D &position) {
                                  const auto it = tileLightData_.find(position);
                                  if (it == tileLightData_.end() || it->second == nullptr) {
                                      return false;
                                  }
                                  return it->second->IsOpaque(layerType);
                              },
                              [this, layerType, &source](const TileVector2D &position) {
                                  ClearLightContributionAt(position, layerType, source);
                              });
}

void glimmer::LightBuffer::SetLightContributionAt(const TileVector2D &position, const TileLayerType layerType,
                                                  const LightSource &source) {
    const TileVector2D &center = source.GetCenter();
    const float factor = source.GetAttenuationFactor(position.x - center.x, position.y - center.y);
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
    for (const auto &[position, tileData]: tileLightData_) {
        if (tileData != nullptr) {
            tileData->ClearAllLightContributions();
        }
    }
    for (const auto &[layerType, lightSource]: sources) {
        SetLightFromSource(*lightSource, layerType);
    }
    ++revision_;
}

void glimmer::LightBuffer::SetSideLightMask(const TileVector2D position, const TileLayerType layerType,
                                            std::unique_ptr<LightMask> sideLightMask) {
    if (sideLightMask == nullptr) {
        return;
    }
    GetOrCreate(position).SetSideLightMask(layerType, std::move(sideLightMask));
    ++revision_;
}

void glimmer::LightBuffer::SetBackLightMask(const TileVector2D position, const TileLayerType layerType,
                                            std::unique_ptr<LightMask> backLightMask) {
    if (backLightMask == nullptr) {
        return;
    }
    TileLightData &tileLightData = GetOrCreate(position);
    tileLightData.SetBackLightMask(layerType, std::move(backLightMask));
    tileLightData.RecalculateLight();
    ++revision_;
}

void glimmer::LightBuffer::ClearSideLightMask(const TileVector2D &position, const TileLayerType layerType) {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return;
    }
    it->second->ClearSideLightMask(layerType);
    ++revision_;
}

void glimmer::LightBuffer::ClearBackLightMask(const TileVector2D &position, const TileLayerType layerType) {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return;
    }
    it->second->ClearBackLightMask(layerType);
    it->second->RecalculateLight();
    ++revision_;
}

void glimmer::LightBuffer::ClearSideLightMaskOnly(const TileVector2D &position, const TileLayerType layerType) {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return;
    }
    it->second->ClearSideLightMask(layerType);
    ++revision_;
}

void glimmer::LightBuffer::ClearBackLightMaskOnly(const TileVector2D &position, const TileLayerType layerType) {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return;
    }
    it->second->ClearBackLightMask(layerType);
    it->second->RecalculateLight();
    ++revision_;
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
        wasGroundOpaque = it->second->IsOpaque(TileLayerType::Ground);
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

const glimmer::Color *glimmer::LightBuffer::GetFinalLightColor(const TileVector2D position) const {
    const auto it = tileLightData_.find(position);
    if (it == tileLightData_.end() || it->second == nullptr) {
        return nullptr;
    }
    return it->second->GetFinalLightColor();
}

float glimmer::LightBuffer::GetSkyVisibility(const TileVector2D &position) const {
    const auto it = columnSkyTopY_.find(position.x);
    const int topY = it != columnSkyTopY_.end() ? it->second : (WORLD_MIN_Y - 1);
    //The topmost opaque tile itself still faces the sky, so it is lit by
    //ambient light; only tiles strictly below it are underground.
    //最顶部的不透明瓦片本身仍朝向天空，会被环境光照亮；只有严格位于其下的瓦片才属于地下。
    return position.y >= topY ? 1.0F : 0.0F;
}

void glimmer::LightBuffer::RecalculateColumnSkyTopY(const int x) {
    for (int y = WORLD_MAX_Y - 1; y >= WORLD_MIN_Y; --y) {
        const auto it = tileLightData_.find(TileVector2D(x, y));
        if (it != tileLightData_.end() && it->second != nullptr &&
            it->second->IsOpaque(TileLayerType::Ground)) {
            columnSkyTopY_[x] = y;
            return;
        }
    }
    columnSkyTopY_[x] = WORLD_MIN_Y - 1;
}

void glimmer::LightBuffer::SetTileOpaque(const TileVector2D position, const TileLayerType layerType,
                                         const bool opaque) {
    bool changed = false;
    if (opaque) {
        TileLightData &tileData = GetOrCreate(position);
        if (!tileData.IsOpaque(layerType)) {
            tileData.SetOpaque(layerType, true);
            changed = true;
        }
    } else {
        const auto it = tileLightData_.find(position);
        if (it == tileLightData_.end() || it->second == nullptr) {
            return;
        }
        if (it->second->IsOpaque(layerType)) {
            it->second->SetOpaque(layerType, false);
            changed = true;
        }
    }
    if (!changed) {
        return;
    }
    if (layerType == TileLayerType::Ground) {
        const auto it = columnSkyTopY_.find(position.x);
        const int currentTop = it != columnSkyTopY_.end() ? it->second : (WORLD_MIN_Y - 1);
        if (opaque) {
            if (position.y > currentTop) {
                columnSkyTopY_[position.x] = position.y;
            }
        } else if (position.y == currentTop) {
            RecalculateColumnSkyTopY(position.x);
        }
    }
    if (batching_) {
        batchDirty_ = true;
    } else {
        RebuildAllLight();
    }
}

void glimmer::LightBuffer::BeginBatch() {
    batching_ = true;
    batchDirty_ = false;
}

void glimmer::LightBuffer::EndBatch() {
    batching_ = false;
    if (batchDirty_) {
        batchDirty_ = false;
        RebuildAllLight();
    }
}
