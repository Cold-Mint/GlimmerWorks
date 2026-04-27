//
// Created by coldmint on 2026/3/28.
//

#include "FloatingTextSystem.h"

#include "core/Constants.h"
#include "core/ecs/component/FloatingTextComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/world/WorldContext.h"

glimmer::FloatingTextSystem::FloatingTextSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<FloatingTextComponent>();
    RequireComponent<Transform2DComponent>();
}

void glimmer::FloatingTextSystem::Update(float delta) {
    if (worldContext_ == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const CameraComponent *cameraComponent = worldContext_->GetCameraComponent();
    const Transform2DComponent *cameraTransform2D = worldContext_->GetCameraTransform2D();
    if (cameraComponent == nullptr) {
        return;
    }
    if (cameraTransform2D == nullptr) {
        return;
    }
    const std::vector<GameEntity::ID> floatingTextEntityList = worldContext_->GetEntityIDWithComponents<
        Transform2DComponent, FloatingTextComponent>();
    if (floatingTextEntityList.empty()) {
        return;
    }
    uint64_t now = SDL_GetTicks();

    for (const uint32_t floatingTextEntity: floatingTextEntityList) {
        auto floatingTextComponent = worldContext_->GetComponent<FloatingTextComponent>(
            floatingTextEntity);
        const Transform2DComponent *transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(
            floatingTextEntity);
        if (floatingTextComponent == nullptr || transform2DComponent == nullptr) {
            continue;
        }
        if (!cameraComponent->
            IsPointInViewport(cameraTransform2D->GetPosition(), transform2DComponent->GetPosition())) {
            continue;
        }
        auto &tween = floatingTextComponent->GetTween();
        tween.step(delta);
        floatingTextComponent->SetAlpha(fabs(tween.peek()));
        if (now > floatingTextComponent->GetExpireTime()) {
            worldContext_->RemoveEntity(floatingTextEntity);
        }
    }
}

void glimmer::FloatingTextSystem::Render(SDL_Renderer *renderer) {
    if (worldContext_ == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const CameraComponent *cameraComponent = worldContext_->GetCameraComponent();
    const Transform2DComponent *cameraTransform2D = worldContext_->GetCameraTransform2D();
    if (cameraComponent == nullptr) {
        return;
    }
    if (cameraTransform2D == nullptr) {
        return;
    }
    const std::vector<GameEntity::ID> floatingTextEntity = worldContext_->GetEntityIDWithComponents<
        Transform2DComponent, FloatingTextComponent>();
    for (const uint32_t floatingText: floatingTextEntity) {
        auto floatingTextComponent = worldContext_->GetComponent<FloatingTextComponent>(
            floatingText);
        const Transform2DComponent *transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(
            floatingText);
        if (floatingTextComponent == nullptr || transform2DComponent == nullptr) {
            continue;
        }
        if (!cameraComponent->
            IsPointInViewport(cameraTransform2D->GetPosition(), transform2DComponent->GetPosition())) {
            continue;
        }
        CameraVector2D camera2D = cameraComponent->GetViewPortPosition(cameraTransform2D->GetPosition(),
                                                                       transform2DComponent->GetPosition());
        std::string &text = floatingTextComponent->GetText();
        if (text.empty()) {
            continue;
        }
        if (floatingTextComponent->GetAlpha() <= 0.01F) {
            continue;
        }
        SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(appContext->GetFont(), text.c_str(), text.length(),
                                                              appContext->GetPreloadColors()->textColor.ToSDLColor(),
                                                              0);
        if (surface == nullptr) {
            continue;
        }
        if (SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface)) {
            auto textW = static_cast<float>(surface->w);
            auto textH = static_cast<float>(surface->h);
            SDL_FRect dst = {
                camera2D.x,
                camera2D.y,
                textW,
                textH
            };

            SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(floatingTextComponent->GetAlpha() * 255));
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);
    }
}

uint8_t glimmer::FloatingTextSystem::GetRenderOrder() {
    return RENDER_ORDER_FLOATING_TEXT;
}

std::string glimmer::FloatingTextSystem::GetName() {
    return "glimmer.FloatingTextSystem";
}
