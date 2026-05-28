//
// Created by Cold-Mint on 2026/5/10.
//

#include "ParallaxBackgroundSystem.h"

#include "SDL3/SDL.h"
#include "core/Constants.h"
#include "core/ecs/component/ParallaxBackgroundComponent.h"
#include "core/world/WorldContext.h"

glimmer::ParallaxBackgroundSystem::ParallaxBackgroundSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<ParallaxBackgroundComponent>();
}

void glimmer::ParallaxBackgroundSystem::Render(SDL_Renderer *renderer) {
    if (worldContext_ == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return;
    }
    ParallaxBackgroundComponent *parallaxBackgroundComponent = worldContext_->GetParallaxBackgroundComponent();
    if (parallaxBackgroundComponent == nullptr) {
        return;
    }
    SDL_Texture *texture = parallaxBackgroundComponent->GetTexture(resourceLocator);
    if (texture == nullptr) {
        return;
    }
    SDL_Window *window = appContext->GetWindow();
    if (window == nullptr) {
        return;
    }
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_FRect destRect = {
        0.0F,
        0.0F,
        static_cast<float>(w),
        static_cast<float>(h)
    };
    SDL_RenderTexture(renderer, texture, nullptr, &destRect);
}

std::string glimmer::ParallaxBackgroundSystem::GetName() {
    return "glimmer.ParallaxBackgroundSystem";
}

uint8_t glimmer::ParallaxBackgroundSystem::GetRenderOrder() {
    return RENDER_ORDER_PARALLAX_BACKGROUND;
}
