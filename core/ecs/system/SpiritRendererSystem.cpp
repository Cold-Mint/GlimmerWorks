//
// Created by Cold-Mint on 2026/3/3.
//

#include "SpiritRendererSystem.h"

#include "core/ecs/component/SpiritRendererComponent.h"
#include "core/world/WorldContext.h"

glimmer::SpiritRendererSystem::SpiritRendererSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<SpiritRendererComponent>();
    RequireComponent<Transform2DComponent>();
}

void glimmer::SpiritRendererSystem::Render(SDL_Renderer *renderer) {
    const AppContext *appContext = worldContext_->GetAppContext();
    const ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    const CameraComponent *cameraComponent = worldContext_->GetCameraComponent();
    const Transform2DComponent *cameraTransform2D = worldContext_->GetCameraTransform2D();
    if (cameraComponent == nullptr || cameraTransform2D == nullptr) {
        return;
    }

    const std::vector<GameEntity::ID> spiritRendererEntity = worldContext_->GetEntityIDWithComponents<
        Transform2DComponent, SpiritRendererComponent>();
    for (const uint32_t spiritRenderer: spiritRendererEntity) {
        auto spiritRendererComponent = worldContext_->GetComponent<SpiritRendererComponent>(
            spiritRenderer);
        const Transform2DComponent *transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(
            spiritRenderer);
        if (spiritRendererComponent == nullptr || transform2DComponent == nullptr) {
            continue;
        }
        SDL_Texture *sdlTexture = spiritRendererComponent->GetTexture(resourceLocator);
        WorldVector2D worldVector2d = transform2DComponent->GetPosition() + spiritRendererComponent->GetPosition();
        SDL_FRect worldVectorRect = {
            worldVector2d.x,
            worldVector2d.y,
            static_cast<float>(sdlTexture->w),
            static_cast<float>(sdlTexture->h)
        };
        if (cameraComponent->IsRectInViewport(cameraTransform2D->GetPosition(), &worldVectorRect)) {
            CameraVector2D cameraVector2d = cameraComponent->GetViewPortPosition(
                cameraTransform2D->GetPosition(), worldVector2d);
            SDL_FRect dstrect = {
                cameraVector2d.x, cameraVector2d.y, static_cast<float>(sdlTexture->w),
                static_cast<float>(sdlTexture->h)
            };
            SDL_RenderTexture(renderer, sdlTexture, nullptr, &dstrect);
        }
    }
}

uint8_t glimmer::SpiritRendererSystem::GetRenderOrder() {
    return RENDER_ORDER_SPIRIT_RENDERER;
}

std::string glimmer::SpiritRendererSystem::GetName() {
    return "glimmer.SpiritRendererSystem";
}
