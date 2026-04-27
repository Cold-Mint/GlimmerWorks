//
// Created by coldmint on 2026/4/24.
//

#include "TileLightData.h"

#include "core/Constants.h"
#include "core/utils/LightUtils.h"


std::unique_ptr<glimmer::Color> glimmer::TileLightData::ComputeFinalLightColor() {
    if (lightContributions_.empty()) {
        return nullptr;
    }
    auto finalLightColor = std::make_unique<Color>();
    bool hasFoundLightSource = false;
    for (int i = 0; i < TILE_LAYER_TYPE_COUNT; ++i) {
        auto tileLayer = static_cast<TileLayerType>(1 << i);
        //Apply the shadow effect.
        //叠加阴影。
        if (hasFoundLightSource) {
            const auto lightMaskIterator = lightMaskData_.find(tileLayer);
            if (lightMaskIterator == lightMaskData_.end()) {
                continue;
            }
            const auto &lightMaskUnique = lightMaskIterator->second;
            if (lightMaskUnique == nullptr) {
                continue;
            }
            const Color *lightMaskColor = lightMaskUnique->GetLightMaskColor();
            if (lightMaskColor == nullptr) {
                continue;
            }
            finalLightColor = LightUtils::ApplyLightingMask(finalLightColor.get(), lightMaskColor);
        }
        const auto lightContributionIterator = lightContributions_.find(tileLayer);
        if (lightContributionIterator == lightContributions_.end()) {
            continue;
        }
        const auto &lightContributionVector = lightContributionIterator->second;
        if (lightContributionVector.empty()) {
            continue;
        }
        for (const auto &lightContribution: lightContributionVector) {
            const auto lightContributionUnique = lightContribution.get();
            if (lightContributionUnique == nullptr) {
                continue;
            }
            const Color *lightColor = lightContributionUnique->GetLightColor();
            if (lightColor == nullptr) {
                continue;
            }
            if (!hasFoundLightSource) {
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
    if (hasFoundLightSource) {
        if (finalLightColor->a == 0) {
            return nullptr;
        }
        return finalLightColor;
    }
    return nullptr;
}

void glimmer::TileLightData::SetLightContribution(const TileLayerType layerType,
                                                  std::unique_ptr<LightContribution> contribution) {
    if (contribution == nullptr) {
        return;
    }
    const LightSource *targetLightSource = contribution->GetLightSource();
    if (targetLightSource == nullptr) {
        return;
    }
    const Color *newLightColor = contribution->GetLightColor();
    if (newLightColor == nullptr) {
        return;
    }

    const auto lightContributionIterator = lightContributions_.find(layerType);
    if (lightContributionIterator == lightContributions_.end()) {
        lightContributions_[layerType].emplace_back(std::move(contribution));
        finalLightColor_ = ComputeFinalLightColor();
        return;
    }

    auto &lightContributionVector = lightContributionIterator->second;
    if (lightContributionVector.empty()) {
        lightContributionVector.emplace_back(std::move(contribution));
        finalLightColor_ = ComputeFinalLightColor();
        return;
    }
    bool foundDuplicate = false;
    bool colorChanged = false;
    for (const auto &lightContribution: lightContributionVector) {
        const auto lightContributionUnique = lightContribution.get();
        if (lightContributionUnique == nullptr) {
            continue;
        }

        if (targetLightSource == lightContribution->GetLightSource()) {
            foundDuplicate = true;
            const Color *oldLightColor = lightContributionUnique->GetLightColor();
            if (oldLightColor == nullptr ||
                newLightColor->a != oldLightColor->a ||
                newLightColor->r != oldLightColor->r ||
                newLightColor->g != oldLightColor->g ||
                newLightColor->b != oldLightColor->b) {
                lightContribution->SetLightColor(contribution->TakeLightColor());
                colorChanged = true;
            }
            break;
        }
    }
    if (foundDuplicate) {
        if (colorChanged) {
            // Find duplicates, the color has changed. Only recalculate, do not add new ones.
            // 找到重复，颜色变了，只重算，不新增
            finalLightColor_ = ComputeFinalLightColor();
        }
    } else {
        //No duplicates were found. New lights were added, so a recalculation is necessary.
        //没找到重复，新增灯光，必须重算
        lightContributionVector.emplace_back(std::move(contribution));
        finalLightColor_ = ComputeFinalLightColor();
    }
}

const glimmer::LightContribution *glimmer::TileLightData::GetLightContribution(const TileLayerType layerType,
                                                                               const LightSource *targetLightSource) {
    if (targetLightSource == nullptr) {
        return nullptr;
    }
    const auto lightContributionIterator = lightContributions_.find(layerType);
    if (lightContributionIterator == lightContributions_.end()) {
        return nullptr;
    }

    auto &lightContributionVector = lightContributionIterator->second;
    if (lightContributionVector.empty()) {
        return nullptr;
    }
    for (const auto &lightContribution: lightContributionVector) {
        if (lightContribution == nullptr) {
            continue;
        }
        if (targetLightSource == lightContribution->GetLightSource()) {
            return lightContribution.get();
        }
    }
    return nullptr;
}

std::vector<const glimmer::LightContribution *> glimmer::TileLightData::GetLightContributionVector(
    const TileLayerType layerType) {
    std::vector<const LightContribution *> result = {};
    const auto lightContributionIterator = lightContributions_.find(layerType);
    if (lightContributionIterator == lightContributions_.end()) {
        return result;
    }

    auto &lightContributionVector = lightContributionIterator->second;
    if (lightContributionVector.empty()) {
        return result;
    }

    for (const auto &lightContribution: lightContributionVector) {
        if (lightContribution == nullptr) {
            continue;
        }
        result.emplace_back(lightContribution.get());
    }
    return result;
}

void glimmer::TileLightData::SetLightSource(const TileLayerType layerType, std::unique_ptr<LightSource> lightSource) {
    if (lightSource == nullptr) {
        return;
    }
    lightSourceData_[layerType] = std::move(lightSource);
}


void glimmer::TileLightData::SetLightMask(const TileLayerType layerType, std::unique_ptr<LightMask> lightMask) {
    if (lightMask == nullptr) {
        return;
    }
    lightMaskData_[layerType] = std::move(lightMask);
}

const glimmer::LightMask *glimmer::TileLightData::GetLightMask(TileLayerType layerType) {
    auto lightMaskIterator = lightMaskData_.find(layerType);
    if (lightMaskIterator == lightMaskData_.end()) {
        return nullptr;
    }
    auto &lightMaskPtr = lightMaskIterator->second;
    if (lightMaskPtr == nullptr) {
        return nullptr;
    }
    return lightMaskPtr.get();
}

const glimmer::LightSource *glimmer::TileLightData::GetLightSource(TileLayerType layerType) {
    auto lightSourceIterator = lightSourceData_.find(layerType);
    if (lightSourceIterator == lightSourceData_.end()) {
        return nullptr;
    }
    auto &lightMaskPtr = lightSourceIterator->second;
    if (lightMaskPtr == nullptr) {
        return nullptr;
    }
    return lightMaskPtr.get();
}


void glimmer::TileLightData::ClearLightMask(const TileLayerType layerType) {
    lightMaskData_.erase(layerType);
}

void glimmer::TileLightData::ClearLightSource(const TileLayerType layerType) {
    lightSourceData_.erase(layerType);
}

const glimmer::Color *glimmer::TileLightData::GetFinalLightColor() const {
    return finalLightColor_.get();
}

void glimmer::TileLightData::ClearLightContribution(const TileLayerType layerType, const LightSource *lightSource,
                                                    const int rayIndex) {
    if (lightSource == nullptr) {
        return;
    }
    const auto lightContributionIterator = lightContributions_.find(layerType);
    if (lightContributionIterator == lightContributions_.end()) {
        return;
    }
    auto &lightContributionVector = lightContributionIterator->second;
    if (lightContributionVector.empty()) {
        return;
    }
    bool hasRemoved = false;
    for (auto iter = lightContributionVector.begin(); iter != lightContributionVector.end(); ++iter) {
        const auto &lightContribution = *iter;
        if (lightContribution == nullptr) {
            continue;
        }

        if (lightSource == lightContribution->GetLightSource()) {
            //lightContribution->GetRayIndex() == -1
            //-1 represents the origin.
            //-1代表原点。
            if (lightContribution->GetRayIndex() == LIGHT_CONTRIBUTION_CENTER_RAY_INDEX || lightContribution->
                GetRayIndex() == rayIndex) {
                lightContributionVector.erase(iter);
                hasRemoved = true;
            }
            break;
        }
    }
    if (hasRemoved) {
        finalLightColor_ = ComputeFinalLightColor();
    }
}
