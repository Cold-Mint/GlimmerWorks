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
    const SDL_Color currentColor = GetLightColor(current);
    int maxRadius = lightSource->maxRadius;
    lightColors_[next] = SDL_Color{
        static_cast<Uint8>(
            currentColor.r - lightSource->emissionColor.r / maxRadius),
        static_cast<Uint8>(
            currentColor.g - lightSource->emissionColor.g / maxRadius),
        static_cast<Uint8>(
            currentColor.b - lightSource->emissionColor.b / maxRadius),
        static_cast<Uint8>(
            currentColor.a - lightSource->emissionColor.a / maxRadius)
    };
    return TraverseAction::Continue;
}

glimmer::TraverseAction glimmer::LightingBuffer::ClearLightPropagation(const LightSource *lightSource,
                                                                       TileVector2D current,
                                                                       const TileVector2D next) {
    if (lightSource == nullptr) {
        return TraverseAction::StopAll;
    }
    const auto it = lightColors_.find(lightSource->center);
    if (it != lightColors_.end()) {
        lightColors_.erase(lightSource->center);
    }
    lightColors_.erase(next);
    return TraverseAction::Continue;
}

std::unique_ptr<SDL_Color> glimmer::LightingBuffer::ComputeLightColorAtLightPoint(const TileVector2D position) {
    auto lightSourceIt = lightSources_.find(position);
    if (lightSourceIt == lightSources_.end()) {
        return nullptr;
    }
    auto &layerLightSourceMap = lightSourceIt->second;
    if (layerLightSourceMap.empty()) {
        return nullptr;
    }
    auto lightMaskIterator = lightMasks_.find(position);
    std::unordered_map<TileLayerType, std::unique_ptr<LightMask> > *layerLightMaskMapPtr = nullptr;
    if (lightMaskIterator != lightMasks_.end()) {
        //Pre-locate the shading information of the current position.
        //预先查找当前位置的遮照信息。
        layerLightMaskMapPtr = &lightMaskIterator->second;
    }
    auto outputLightColor = SDL_Color{};
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
                    outputLightColor = LightUtils::ApplyLightingMask(outputLightColor, lightMask->lightMaskColor);
                }
            }
        }
        auto layerLightIt = layerLightSourceMap.find(currentLayerType);
        if (layerLightIt == layerLightSourceMap.end()) {
            continue;
        }
        std::unique_ptr<LightSource> &lightSource = layerLightIt->second;
        if (lightSource == nullptr) {
            continue;
        }
        if (!hasFoundLightSource) {
            //The first time I found the source of light.
            //第一次找到光源。
            outputLightColor = lightSource->emissionColor;
            hasFoundLightSource = true;
            continue;
        }
        //It's not the first time that color mixing has been performed.
        //不是第一次找到，进行颜色混合。
        outputLightColor = LightUtils::MixLights(outputLightColor, lightSource->emissionColor);
    }
    if (hasFoundLightSource) {
        return std::make_unique<SDL_Color>(outputLightColor);
    }
    return nullptr;
}

glimmer::LightingBuffer::LightingBuffer(WorldContext *worldContext) {
    worldContext_ = worldContext;
}

void glimmer::LightingBuffer::AddLightMask(std::unique_ptr<LightMask> lightMask) {
    lightMasks_[lightMask->position][lightMask->tileLayer] = std::move(lightMask);
}

void glimmer::LightingBuffer::RemoveLightMask(const TileVector2D position) {
    const auto lightMaskIterator = lightMasks_.find(position);
    if (lightMaskIterator == lightMasks_.end()) {
        return;
    }
    lightMasks_.erase(lightMaskIterator);
}

void glimmer::LightingBuffer::AddLightSource(std::unique_ptr<LightSource> lightSource) {
    if (lightSource == nullptr) {
        return;
    }
    const auto lightSourcesIterator = lightSources_.find(lightSource->center);
    std::unordered_map<TileLayerType, std::unique_ptr<LightSource> > *tileLayerTypeMapPtr = nullptr;
    if (lightSourcesIterator == lightSources_.end()) {
        tileLayerTypeMapPtr = &lightSources_[lightSource->center];
    } else {
        tileLayerTypeMapPtr = &lightSourcesIterator->second;
        if (tileLayerTypeMapPtr->contains(lightSource->tileLayer)) {
            //The current position and the current layer already have a light source.
            //当前位置和当前图层，已经有一个光源了。
            LogCat::e("Try to add the light source again.");
            return;
        }
    }
    auto lightPtr = lightSource.get();
    std::unordered_map<TileLayerType, std::unique_ptr<LightSource> > &tileLayerTypeMap = *tileLayerTypeMapPtr;
    tileLayerTypeMap[lightPtr->tileLayer] = std::move(lightSource);
    //Set the color of the light source as the origin point.
    //设置原点的光源颜色。
    std::unique_ptr<SDL_Color> color = ComputeLightColorAtLightPoint(lightPtr->center);
    if (color == nullptr) {
        return;
    }
    lightColors_[lightPtr->center] = SDL_Color{color->r, color->g, color->b, color->a};
    const LightPropagationTraverser lightPropagationTraverser = LightPropagationTraverser(
        lightPtr->center, lightPtr->maxRadius,
        [this, lightPtr](const TileVector2D cur, const TileVector2D next) {
            return this->ApplyLightPropagation(lightPtr, cur, next);
        }
    );
    lightPropagationTraverser.Start();
}

SDL_Color glimmer::LightingBuffer::GetLightColor(const TileVector2D position) {
    const auto it = lightColors_.find(position);
    if (it == lightColors_.end()) {
        if (worldContext_ == nullptr) {
            return SDL_Color{0, 0, 0, 0};
        }
        const AppContext *appContext = worldContext_->GetAppContext();
        if (appContext == nullptr) {
            return SDL_Color{0, 0, 0, 0};
        }
        const PreloadColors *preloadColors_ = appContext->GetPreloadColors();
        if (preloadColors_ == nullptr) {
            return SDL_Color{0, 0, 0, 0};
        }
        return preloadColors_->light.defaultEmissionColor;
    }
    return it->second;
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
    const LightPropagationTraverser lightPropagationTraverser = LightPropagationTraverser(
        lightPtr->center, lightPtr->maxRadius,
        [this,lightPtr](const TileVector2D cur, const TileVector2D next) {
            return this->ClearLightPropagation(lightPtr, cur, next);
        }
    );
    lightPropagationTraverser.Start();
    lightSourcesMap.erase(lightSourcePtrIterator);
    if (lightSourcesMap.empty()) {
        lightSources_.erase(lightSourcesMapIterator);
    }
}

void glimmer::LightingBuffer::RemoveAllLightSources(const TileVector2D &position) {
    const auto lightSourcesMapIterator = lightSources_.find(position);
    if (lightSourcesMapIterator == lightSources_.end()) {
        return;
    }
    auto &lightSourcesMap = lightSourcesMapIterator->second;
    if (!lightSourcesMap.empty()) {
        for (auto &lightSourcesMapPair: lightSourcesMap) {
            const auto lightSourcePtrIterator = lightSourcesMap.find(lightSourcesMapPair.first);
            if (lightSourcePtrIterator == lightSourcesMap.end()) {
                return;
            }
            auto &lightSourceUnique = lightSourcePtrIterator->second;
            if (lightSourceUnique == nullptr) {
                return;
            }
            auto lightPtr = lightSourceUnique.get();
            const LightPropagationTraverser lightPropagationTraverser = LightPropagationTraverser(
                lightPtr->center, lightPtr->maxRadius,
                [this,lightPtr](const TileVector2D cur, const TileVector2D next) {
                    return this->ClearLightPropagation(lightPtr, cur, next);
                }
            );
            lightPropagationTraverser.Start();
            lightSourcesMap.erase(lightSourcePtrIterator);
        }
    }
    lightSources_.erase(lightSourcesMapIterator);
}
