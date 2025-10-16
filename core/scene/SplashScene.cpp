//
// Created by Cold-Mint on 2025/10/12.
//

#include "SplashScene.h"

#include "AppContext.h"
#include "../Config.h"
#include "../log/LogCat.h"

bool Glimmer::SplashScene::HandleEvent(const SDL_Event &event) {
    return false;
}

void Glimmer::SplashScene::Update(float delta) {
}

void Glimmer::SplashScene::Render(SDL_Renderer *renderer) {
    if (!renderer) return;
    if (!splashTexture) {
        splashTexture = appContext->resourcePackManager->loadTextureFromFile(
            appContext->config->mods.enabledResourcePack, *renderer, "gui/splash.png");
        if (!splashTexture) {
            LogCat::e("Failed to load splash.png");
            return;
        }
    }

    float texW = 0, texH = 0;
    if (!SDL_GetTextureSize(splashTexture, &texW, &texH)) {
        LogCat::e("SDL_GetTextureSize Error: ", SDL_GetError());
        return;
    }
    int winW = 0, winH = 0;
    if (!SDL_GetRenderOutputSize(renderer, &winW, &winH)) {
        LogCat::e("SDL_GetRenderOutputSize Error: ", SDL_GetError());
        return;
    }

    const SDL_FRect dstRect = {
        0.0F,
        0.0F,
        static_cast<float>(winW),
        static_cast<float>(winH)
    };

    if (!SDL_RenderTexture(renderer, splashTexture, nullptr, &dstRect)) {
        LogCat::e("SDL_RenderTexture Error: ", SDL_GetError());
    }
}
