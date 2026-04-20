//
// Created by coldmint on 2026/4/15.
//

#include "LightingBuffer.h"

#include "LightPropagationTraverser.h"
#include "WorldContext.h"
#include "core/utils/ColorUtils.h"


glimmer::TraverseAction glimmer::LightingBuffer::ApplyLightPropagation(const LightSource *lightSource,
                                                                       TileVector2D current,
                                                                       TileVector2D next) {
    if (lightSource == nullptr) {
        return TraverseAction::StopAll;
    }
    const auto centerLightSourceIterator = lightColors_.find(lightSource->center);
    if (centerLightSourceIterator == lightColors_.end()) {
        lightColors_[lightSource->center] = lightSource->emissionColor;
    }
    const auto currentLightMaskIterator = lightMasks_.find(current);
    if (currentLightMaskIterator == lightMasks_.end()) {
        return TraverseAction::SkipDirection;
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
    const auto lightSourcesIterator = lightSources_.find(lightSource->center);
    if (lightSourcesIterator == lightSources_.end()) {
        return;
    }

    if (lightSource == nullptr) {
        return;
    }
    auto lightPtr = lightSource.get();
    lightSources_[lightPtr->center][lightPtr->tileLayer] = std::move(lightSource);
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
    const auto lightSourcesIterator = lightSources_.find(position);
    if (lightSourcesIterator == lightSources_.end()) {
        return;
    }
    const auto &lightArray = lightSourcesIterator->second;
    auto &lightUnique = lightArray[layerType];
    if (lightUnique == nullptr) {
        return;
    }
    auto lightPtr = lightUnique.get();
    const LightPropagationTraverser lightPropagationTraverser = LightPropagationTraverser(
        lightPtr->center, lightPtr->maxRadius,
        [this,lightPtr](const TileVector2D cur, const TileVector2D next) {
            return this->ClearLightPropagation(lightPtr, cur, next);
        }
    );
    lightPropagationTraverser.Start();
    lightSources_.erase(lightSourcesIterator);
}
