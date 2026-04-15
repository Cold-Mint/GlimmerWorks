//
// Created by coldmint on 2026/4/15.
//

#include "LightingBuffer.h"

#include "LightPropagationTraverser.h"
#include "WorldContext.h"


glimmer::TraverseAction glimmer::LightingBuffer::StepCallback(TileVector2D current, TileVector2D next) {
    return TraverseAction::StopAll;
}

glimmer::LightingBuffer::LightingBuffer(WorldContext *worldContext) {
    worldContext_ = worldContext;
}

void glimmer::LightingBuffer::AddLightSource(std::unique_ptr<LightSource> lightSource) {
    lightSources_.emplace_back(std::move(lightSource));
    //Calculate the influence range of the newly added light source.
    //计算新增光源的影响范围。
    const LightPropagationTraverser lightPropagationTraverser = LightPropagationTraverser(
        lightSource.get(), [this](const TileVector2D cur,
                                  const TileVector2D next) {
            return this->StepCallback(cur, next);
        });
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

void glimmer::LightingBuffer::RemoveLightSource(const TileVector2D &position) {
    for (auto it = lightSources_.begin(); it != lightSources_.end();) {
        const auto &lightPtr = *it;
        // Direct deletion of null pointer
        // 空指针直接删除
        if (lightPtr == nullptr) {
            it = lightSources_.erase(it);
            continue;
        }

        // Find the target, delete it and exit.
        // 找到目标，删除并退出
        if (lightPtr->center == position) {
            lightSources_.erase(it);
            break;
        }

        ++it;
    }
}
