//
// Created by coldmint on 2026/4/15.
//

#include "LightingBuffer.h"

#include "LightPropagationTraverser.h"
#include "WorldContext.h"
#include "core/utils/ColorUtils.h"
#include "core/utils/LightUtils.h"


glimmer::TraverseAction glimmer::LightingBuffer::ApplyLightPropagation(const LightSource *lightSource,
                                                                       TileVector2D current,
                                                                       TileVector2D next) {
    if (lightSource == nullptr) {
        return TraverseAction::StopAll;
    }
    const TileLayerType tileLayerType = lightSource->GetTileLayerType();
    const SDL_Color *currentColor = GetLayerLightColor(current, tileLayerType);
    if (currentColor == nullptr) {
        return TraverseAction::SkipDirection;
    }
    int maxRadius = lightSource->GetMaxRadius();
    const SDL_Color &emissionColor = lightSource->GetEmissionColor();
    auto newNextColor = std::make_unique<SDL_Color>(
        static_cast<Uint8>(std::max(0, currentColor->r - emissionColor.r / maxRadius)),
        static_cast<Uint8>(std::max(0, currentColor->g - emissionColor.g / maxRadius)),
        static_cast<Uint8>(std::max(0, currentColor->b - emissionColor.b / maxRadius)),
        static_cast<Uint8>(std::max(0, currentColor->a - emissionColor.a / maxRadius)));
    const SDL_Color *nextColor = GetLayerLightColor(next, tileLayerType);
    if (nextColor != nullptr) {
        newNextColor = LightUtils::MixLights(newNextColor.get(), nextColor);
    }
    auto nextColorPtr = newNextColor.get();
    const SDL_Color *nextColorMask = GetLayerLightMaskColor(next, tileLayerType);
    if (nextColorMask == nullptr) {
        layerLightColors_[next][tileLayerType] = std::move(newNextColor);
    } else {
        layerLightColors_[next][tileLayerType] = LightUtils::ApplyLightingMask(currentColor, nextColorMask);
    }

    std::unique_ptr<SDL_Color> totalLightColor = ComputeTotalLightColorFromLayers(next);
    if (totalLightColor == nullptr || totalLightColor->a == 0) {
        totalLightColor_.erase(next);
        return TraverseAction::Continue;
    }
    totalLightColor_[next] = std::move(totalLightColor);
    if (nextColorPtr->a == 0) {
        return TraverseAction::SkipDirection;
    }
    return TraverseAction::Continue;
}

glimmer::TraverseAction glimmer::LightingBuffer::ClearLightPropagation(const LightSource *lightSource,
                                                                       TileVector2D current,
                                                                       const TileVector2D next) {
    if (lightSource == nullptr) {
        return TraverseAction::StopAll;
    }
    auto layerLightColorIt = layerLightColors_.find(next);
    if (layerLightColorIt == layerLightColors_.end()) {
        return TraverseAction::Continue;
    }
    layerLightColorIt->second.erase(lightSource->GetTileLayerType());
    totalLightColor_[next] = ComputeTotalLightColorFromLayers(next);
    return TraverseAction::Continue;
}

std::unique_ptr<SDL_Color> glimmer::LightingBuffer::
ComputeTotalLightColorFromLayers(const TileVector2D position) {
    const auto layerLightColorIt = layerLightColors_.find(position);
    if (layerLightColorIt == layerLightColors_.end()) {
        return nullptr;
    }
    auto &layerLightColorMap = layerLightColorIt->second;
    if (layerLightColorMap.empty()) {
        return nullptr;
    }
    const auto lightMaskIterator = lightMasks_.find(position);
    std::unordered_map<TileLayerType, std::unique_ptr<LightMask> > *layerLightMaskMapPtr = nullptr;
    if (lightMaskIterator != lightMasks_.end()) {
        //Pre-locate the shading information of the current position.
        //预先查找当前位置的遮照信息。
        layerLightMaskMapPtr = &lightMaskIterator->second;
    }
    std::unique_ptr<SDL_Color> outputLightColor = nullptr;
    bool hasFoundLightSource = false;
    for (int i = 0; i < TILE_LAYER_TYPE_COUNT; ++i) {
        auto currentLayerType = static_cast<TileLayerType>(1 << i);
        if (hasFoundLightSource && layerLightMaskMapPtr != nullptr) {
            //If a light source was found during the last cycle, then the light source needs to be blocked.
            //如果在上次循环，找到了光源，那么需要对光源施加遮挡。
            //For example: If a luminous wall is placed with opaque blocks in front of it, then some of the light will be blocked.
            //例如：发光墙壁，前面放置了不透光的方块，那么会挡住一部分光线。
            auto layerMaskIt = layerLightMaskMapPtr->find(currentLayerType);
            if (layerMaskIt != layerLightMaskMapPtr->end()) {
                //Found the cover.
                //找到了遮挡。
                std::unique_ptr<LightMask> &lightMask = layerMaskIt->second;
                if (lightMask != nullptr) {
                    outputLightColor =
                            LightUtils::ApplyLightingMask(outputLightColor.get(), &lightMask->GetLightMaskColor());
                }
            }
        }
        auto layerLightIt = layerLightColorMap.find(currentLayerType);
        if (layerLightIt == layerLightColorMap.end()) {
            continue;
        }
        std::unique_ptr<SDL_Color> &lightColorUniquePtr = layerLightIt->second;
        if (lightColorUniquePtr == nullptr) {
            continue;
        }
        SDL_Color *light = lightColorUniquePtr.get();
        if (!hasFoundLightSource) {
            //The first time I found the source of light.
            //第一次找到光源。
            outputLightColor = std::make_unique<SDL_Color>(light->r, light->g, light->b, light->a);
            hasFoundLightSource = true;
            continue;
        }
        //It's not the first time that color mixing has been performed.
        //不是第一次找到，进行颜色混合。
        outputLightColor = LightUtils::MixLights(outputLightColor.get(), light);
    }
    if (hasFoundLightSource) {
        return outputLightColor;
    }
    return nullptr;
}

void glimmer::LightingBuffer::UpdateAllLightsInRadius(const TileLayerType layerType, const TileVector2D center,
                                                      const int radius) {
    const int squaredRadius = radius * radius;
    for (auto &[pos, layerMap]: lightSources_) {
        const int distSq = pos.DistanceSquared(center);
        if (distSq > squaredRadius) {
            continue;
        }
        auto layerIt = layerMap.find(layerType);
        if (layerIt == layerMap.end()) {
            continue;
        }
        auto &lightSourcePtr = layerIt->second;
        if (lightSourcePtr == nullptr) {
            continue;
        }
        LightPropagationTraverser lightPropagationTraverser(
            lightSourcePtr->GetCenter(),
            lightSourcePtr->GetMaxRadius(),
            [this, lightPtr = lightSourcePtr.get()](const TileVector2D cur, const TileVector2D next) {
                return this->ApplyLightPropagation(lightPtr, cur, next);
            }
        );

        lightPropagationTraverser.Start();
    }
}


glimmer::LightingBuffer::LightingBuffer(WorldContext *worldContext) {
    worldContext_ = worldContext;
}

void glimmer::LightingBuffer::AddLightMask(std::unique_ptr<LightMask> lightMask) {
    if (lightMask == nullptr) {
        return;
    }
    const LightMask *lightMaskPtr = lightMask.get();
    lightMasks_[lightMask->GetPosition()][lightMask->GetTileLayer()] = std::move(lightMask);
    UpdateAllLightsInRadius(lightMaskPtr->GetTileLayer(), lightMaskPtr->GetPosition(), CHUNK_SIZE);
}

void glimmer::LightingBuffer::RemoveLightMask(TileLayerType layerType, const TileVector2D &position) {
    const auto lightMaskIterator = lightMasks_.find(position);
    if (lightMaskIterator == lightMasks_.end()) {
        return;
    }
    auto &layerMaskMap = lightMaskIterator->second;
    const auto layerMaskIterator = layerMaskMap.find(layerType);
    if (layerMaskIterator == layerMaskMap.end()) {
        return;
    }
    const std::unique_ptr<LightMask> &lightMask = layerMaskIterator->second;
    if (lightMask == nullptr) {
        return;
    }
    const LightMask *lightMaskPtr = lightMask.get();
    UpdateAllLightsInRadius(lightMaskPtr->GetTileLayer(), lightMaskPtr->GetPosition(), CHUNK_SIZE);
    layerMaskMap.erase(layerMaskIterator);
    if (layerMaskMap.empty()) {
        lightMasks_.erase(lightMaskIterator);
    }
}

void glimmer::LightingBuffer::RemoveAllLightMask(const TileVector2D &position) {
    const auto lightMaskIterator = lightMasks_.find(position);
    if (lightMaskIterator == lightMasks_.end()) {
        return;
    }
    auto &lightMaskMap = lightMaskIterator->second;
    if (!lightMaskMap.empty()) {
        for (auto &lightSourcesMapPair: lightMaskMap) {
            RemoveLightSource(lightSourcesMapPair.first, position);
        }
    }
}

void glimmer::LightingBuffer::AddLightSource(std::unique_ptr<LightSource> lightSource) {
    if (lightSource == nullptr) {
        return;
    }
    const TileVector2D &center = lightSource->GetCenter();
    const TileLayerType &tileLayer = lightSource->GetTileLayerType();
    const SDL_Color &emissionColor = lightSource->GetEmissionColor();
    const auto lightSourcesIterator = lightSources_.find(center);
    if (lightSourcesIterator != lightSources_.end()) {
        std::unordered_map<TileLayerType, std::unique_ptr<LightSource> > &tileLayerTypeMapPtr = lightSourcesIterator->
                second;
        if (tileLayerTypeMapPtr.contains(tileLayer)) {
            //The current position and the current layer already have a light source.
            //当前位置和当前图层，已经有一个光源了。
            LogCat::e("Try to add the light source again.");
            return;
        }
    }
    auto lightPtr = lightSource.get();
    lightSources_[center][tileLayer] = std::move(lightSource);
    layerLightColors_[center][tileLayer] = std::make_unique<SDL_Color>(
        emissionColor.r, emissionColor.g, emissionColor.b, emissionColor.a);
    //Set the color of the light source as the origin point.
    //设置原点的光源颜色。
    const std::unique_ptr<SDL_Color> color = ComputeTotalLightColorFromLayers(center);
    if (color == nullptr) {
        totalLightColor_.erase(center);
    } else {
        totalLightColor_[center] = std::make_unique<SDL_Color>(color->r, color->g, color->b, color->a);
    }
    UpdateAllLightsInRadius(tileLayer, center, lightPtr->GetMaxRadius());
}

const SDL_Color *glimmer::LightingBuffer::GetTotalLightColor(const TileVector2D position) {
    const auto it = totalLightColor_.find(position);
    if (it == totalLightColor_.end()) {
        return nullptr;
    }
    const std::unique_ptr<SDL_Color> &sdlColor = it->second;
    if (sdlColor == nullptr) {
        return nullptr;
    }
    return sdlColor.get();
}

const SDL_Color *
glimmer::LightingBuffer::GetLayerLightColor(const TileVector2D position, const TileLayerType layerType) {
    const auto it = layerLightColors_.find(position);
    if (it == layerLightColors_.end()) {
        return nullptr;
    }
    const std::unordered_map<TileLayerType, std::unique_ptr<SDL_Color> > &unorderedMap = it->second;
    if (unorderedMap.empty()) {
        return nullptr;
    }
    const auto layerIt = unorderedMap.find(layerType);
    if (layerIt == unorderedMap.end()) {
        return nullptr;
    }
    const std::unique_ptr<SDL_Color> &sdlColor = layerIt->second;
    if (sdlColor == nullptr) {
        return nullptr;
    }
    return sdlColor.get();
}

const SDL_Color *glimmer::LightingBuffer::GetLayerLightMaskColor(const TileVector2D position,
                                                                 const TileLayerType layerType) {
    const auto it = lightMasks_.find(position);
    if (it == lightMasks_.end()) {
        return nullptr;
    }
    const std::unordered_map<TileLayerType, std::unique_ptr<LightMask> > &unorderedMap = it->second;
    if (unorderedMap.empty()) {
        return nullptr;
    }
    const auto layerIt = unorderedMap.find(layerType);
    if (layerIt == unorderedMap.end()) {
        return nullptr;
    }
    const std::unique_ptr<LightMask> &lightMask = layerIt->second;
    if (lightMask == nullptr) {
        return nullptr;
    }
    return &lightMask->GetLightMaskColor();
}

void glimmer::LightingBuffer::RemoveLightSource(const TileLayerType layerType, const TileVector2D &position) {
    const auto lightSourcesMapIterator = lightSources_.find(position);
    if (lightSourcesMapIterator == lightSources_.end()) {
        return;
    }
    auto &lightSourcesMap = lightSourcesMapIterator->second;
    const auto lightSourcePtrIterator = lightSourcesMap.find(layerType);
    if (lightSourcePtrIterator == lightSourcesMap.end()) {
        return;
    }
    auto &lightSourceUnique = lightSourcePtrIterator->second;
    if (lightSourceUnique == nullptr) {
        return;
    }
    auto lightPtr = lightSourceUnique.get();
    auto maxRadius = lightPtr->GetMaxRadius();
    auto tileLayerType = lightPtr->GetTileLayerType();
    const LightPropagationTraverser lightPropagationTraverser(
        position, maxRadius,
        [this,lightPtr](const TileVector2D cur, const TileVector2D next) {
            return this->ClearLightPropagation(lightPtr, cur, next);
        }
    );
    lightPropagationTraverser.Start();
    lightSourcesMap.erase(lightSourcePtrIterator);
    if (lightSourcesMap.empty()) {
        lightSources_.erase(lightSourcesMapIterator);
    }
    const auto layerLightColor = layerLightColors_.find(position);
    if (layerLightColor != layerLightColors_.end()) {
        auto &layerLightMap = layerLightColor->second;
        layerLightMap.erase(layerType);
        if (layerLightMap.empty()) {
            layerLightColors_.erase(layerLightColor);
        }
    }
    std::unique_ptr<SDL_Color> color = ComputeTotalLightColorFromLayers(position);
    if (color == nullptr) {
        totalLightColor_.erase(position);
    } else {
        totalLightColor_[position] = std::make_unique<SDL_Color>(color->r, color->g, color->b, color->a);
    }
    UpdateAllLightsInRadius(tileLayerType, position, maxRadius);
}

void glimmer::LightingBuffer::RemoveAllLightSources(const TileVector2D &position) {
    const auto lightSourcesMapIterator = lightSources_.find(position);
    if (lightSourcesMapIterator == lightSources_.end()) {
        return;
    }
    auto &lightSourcesMap = lightSourcesMapIterator->second;
    if (!lightSourcesMap.empty()) {
        for (auto &lightSourcesMapPair: lightSourcesMap) {
            RemoveLightSource(lightSourcesMapPair.first, position);
        }
    }
}
