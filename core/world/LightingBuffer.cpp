//
// Created by coldmint on 2026/4/15.
//

#include "LightingBuffer.h"

#include "LightPropagationTraverser.h"
#include "core/utils/LightUtils.h"


glimmer::TraverseAction glimmer::LightingBuffer::ClearLightStepCallback(const LightSource *lightSourcePtr,
                                                                        const TileVector2D current,
                                                                        const TileVector2D next,
                                                                        const bool centerOfCircle,
                                                                        const TileLayerType layerType,
                                                                        const int rayIndex) {
    if (centerOfCircle) {
        auto currentTileLightIterator = tileLightData_.find(
            current);
        if (currentTileLightIterator != tileLightData_.end()) {
            const std::unique_ptr<TileLightData> &
                    currentTileLight =
                    currentTileLightIterator->second;
            if (currentTileLight != nullptr) {
                TileLightData *currentTileLightPtr =
                        currentTileLight.get();
                if (currentTileLightPtr != nullptr) {
                    currentTileLightPtr->ClearLightContribution(
                        layerType, lightSourcePtr, rayIndex);
                }
            }
        }
    }
    auto nextTileLightIterator = tileLightData_.find(next);
    if (nextTileLightIterator != tileLightData_.end()) {
        const std::unique_ptr<TileLightData> &
                nextTileLight =
                nextTileLightIterator->second;
        if (nextTileLight != nullptr) {
            TileLightData *nextTileLightPtr =
                    nextTileLight.get();
            if (nextTileLightPtr != nullptr) {
                nextTileLightPtr->ClearLightContribution(
                    layerType, lightSourcePtr, rayIndex);
            }
        }
    }
    return TraverseAction::Continue;
}

glimmer::TraverseAction glimmer::LightingBuffer::SetLightStepCallback(const LightSource *lightSourcePtr,
                                                                      TileVector2D current, TileVector2D next,
                                                                      bool centerOfCircle, TileLayerType layerType,
                                                                      int rayIndex) {
    const Color emissionColor = lightSourcePtr->
            GetEmissionColor();
    auto &currentTileLight = tileLightData_[current];
    if (currentTileLight == nullptr) {
        currentTileLight = std::make_unique<TileLightData>();
    }
    if (centerOfCircle) {
        //If it is the center point.
        //如果是圆心
        auto currentLightContribution = std::make_unique<
            LightContribution>();
        auto lightColor = std::make_unique
                <Color>();
        lightColor->r = emissionColor.r;
        lightColor->g = emissionColor.g;
        lightColor->b = emissionColor.b;
        lightColor->a = emissionColor.a;
        currentLightContribution->SetLightColor(
            std::move(lightColor));
        currentLightContribution->SetRayIndex(LIGHT_CONTRIBUTION_CENTER_RAY_INDEX);
        currentLightContribution->
                SetLightSource(lightSourcePtr);
        currentTileLight->SetLightContribution(
            layerType, std::move(currentLightContribution));
    }

    auto &nextTileLight = tileLightData_[next];
    if (nextTileLight == nullptr) {
        nextTileLight = std::make_unique<TileLightData>();
    }
    const LightContribution *currentLightContribution =
            currentTileLight->
            GetLightContribution(layerType, lightSourcePtr);
    const Color *currentColor = currentLightContribution->
            GetLightColor();
    auto nextLightContribution = std::make_unique<
        LightContribution>();
    int maxRadius = lightSourcePtr->GetMaxRadius();
    auto nextColor = std::make_unique<Color>(
        static_cast<uint8_t>(std::max(
            0, currentColor->r -
               emissionColor.r /
               maxRadius)),
        static_cast<uint8_t>(std::max(
            0, currentColor->g - emissionColor.g /
               maxRadius)),
        static_cast<uint8_t>(std::max(
            0, currentColor->b - emissionColor.b /
               maxRadius)),
        static_cast<uint8_t>(std::max(
            0, currentColor->a - emissionColor.a /
               maxRadius)));

    const LightMask *lightMask = nextTileLight->GetLightMask(
        layerType);
    bool applyLightMask = false;
    if (lightMask != nullptr) {
        const Color *lightMaskColor = lightMask->
                GetLightMaskColor();
        if (lightMaskColor != nullptr) {
            nextLightContribution->SetLightColor(
                LightUtils::ApplyLightingMask(
                    currentColor, lightMaskColor));
            applyLightMask = true;
        }
    }
    if (!applyLightMask) {
        nextLightContribution->SetLightColor(
            std::move(nextColor));
    }
    nextLightContribution->SetLightSource(lightSourcePtr);
    nextLightContribution->SetRayIndex(rayIndex);
    nextTileLight->SetLightContribution(
        layerType, std::move(nextLightContribution));
    return TraverseAction::Continue;
}


void glimmer::LightingBuffer::SetLightMask(const TileVector2D position, const TileLayerType layerType,
                                           std::unique_ptr<LightMask> lightMask) {
    tileLightData_[position]->SetLightMask(layerType, std::move(lightMask));
    auto tileLightDataIterator = tileLightData_.find(position);
    if (tileLightDataIterator == tileLightData_.end()) {
        return;
    }
    const auto &tileLightDataPtr = tileLightDataIterator->second;
    if (tileLightDataPtr == nullptr) {
        return;
    }
    const std::vector<const LightContribution *> lightContributionVector = tileLightDataPtr->
            GetLightContributionVector(layerType);
    for (auto lightContribution: lightContributionVector) {
        if (lightContribution == nullptr) {
            continue;
        }
        auto lightSourcePtr = lightContribution->GetLightSource();
        if (lightSourcePtr == nullptr) {
            continue;
        }
        LightPropagationTraverser lightPropagationTraverser(lightSourcePtr->GetCenter(), lightSourcePtr->GetMaxRadius(),
                                                            [this, layerType, lightSourcePtr](
                                                        const TileVector2D current, const TileVector2D next,
                                                        bool centerOfCircle,
                                                        int rayIndex) -> TraverseAction {
                                                                return SetLightStepCallback(
                                                                    lightSourcePtr, current, next, centerOfCircle,
                                                                    layerType, rayIndex);
                                                            });
        lightPropagationTraverser.PropagateSingleRay(lightContribution->GetRayIndex());
    }
}

void glimmer::LightingBuffer::ClearLightMask(const TileVector2D &position, const TileLayerType layerType) {
    auto tileLightDataIterator = tileLightData_.find(position);
    if (tileLightDataIterator == tileLightData_.end()) {
        return;
    }
    const auto &tileLightDataPtr = tileLightDataIterator->second;
    if (tileLightDataPtr == nullptr) {
        return;
    }
    const std::vector<const LightContribution *> lightContributionVector = tileLightDataPtr->
            GetLightContributionVector(layerType);
    for (auto lightContribution: lightContributionVector) {
        if (lightContribution == nullptr) {
            continue;
        }
        auto lightSourcePtr = lightContribution->GetLightSource();
        if (lightSourcePtr == nullptr) {
            continue;
        }
        LightPropagationTraverser lightPropagationTraverser(lightSourcePtr->GetCenter(), lightSourcePtr->GetMaxRadius(),
                                                            [this, layerType, lightSourcePtr](
                                                        const TileVector2D current, const TileVector2D next,
                                                        bool centerOfCircle,
                                                        int rayIndex) -> TraverseAction {
                                                                return ClearLightStepCallback(
                                                                    lightSourcePtr, current, next, centerOfCircle,
                                                                    layerType, rayIndex);
                                                            });
        lightPropagationTraverser.PropagateSingleRay(lightContribution->GetRayIndex());
    }
    tileLightDataPtr->ClearLightMask(layerType);
}

void glimmer::LightingBuffer::ClearTileLightData(const TileVector2D &position) {
    tileLightData_.erase(position);
}

void glimmer::LightingBuffer::SetLightSource(const TileVector2D position, const TileLayerType layerType,
                                             std::unique_ptr<LightSource> lightSource) {
    if (lightSource == nullptr) {
        return;
    }
    LightSource *lightSourcePtr = lightSource.get();
    //Add the contribution of lighting centered around the light source.
    //以光源为中心添加光照贡献。
    LightPropagationTraverser lightPropagationTraverser(lightSourcePtr->GetCenter(), lightSourcePtr->GetMaxRadius(),
                                                        [this, layerType, lightSourcePtr](
                                                    const TileVector2D current, const TileVector2D next,
                                                    bool centerOfCircle,
                                                    int rayIndex) -> TraverseAction {
                                                            return SetLightStepCallback(
                                                                lightSourcePtr, current, next, centerOfCircle,
                                                                layerType, rayIndex);
                                                        });
    lightPropagationTraverser.PropagateAllRays();
    tileLightData_[position]->SetLightSource(layerType, std::move(lightSource));
}

void glimmer::LightingBuffer::ClearLightSource(const TileVector2D position, const TileLayerType layerType) {
    auto tileLightDataIterator = tileLightData_.find(position);
    if (tileLightDataIterator == tileLightData_.end()) {
        return;
    }
    const auto &tileLightDataPtr = tileLightDataIterator->second;
    if (tileLightDataPtr == nullptr) {
        return;
    }
    const LightSource *lightSourcePtr = tileLightDataPtr->GetLightSource(layerType);
    if (lightSourcePtr == nullptr) {
        return;
    }

    LightPropagationTraverser lightPropagationTraverser(lightSourcePtr->GetCenter(), lightSourcePtr->GetMaxRadius(),
                                                        [this, layerType, lightSourcePtr](
                                                    TileVector2D current, TileVector2D next,
                                                    bool centerOfCircle,
                                                    int rayIndex) -> TraverseAction {
                                                            return ClearLightStepCallback(
                                                                lightSourcePtr, current, next, centerOfCircle,
                                                                layerType, rayIndex);
                                                        });
    lightPropagationTraverser.PropagateAllRays();
    tileLightDataPtr->ClearLightSource(layerType);
}

const glimmer::Color *glimmer::LightingBuffer::GetFinalLightColor(const TileVector2D position) {
    const auto tileLightDataIterator = tileLightData_.find(position);
    if (tileLightDataIterator == tileLightData_.end()) {
        return nullptr;
    }
    const std::unique_ptr<TileLightData> &tileLightDataUniquePtr = tileLightDataIterator->second;
    if (tileLightDataUniquePtr == nullptr) {
        return nullptr;
    }
    return tileLightDataUniquePtr->GetFinalLightColor();
}
