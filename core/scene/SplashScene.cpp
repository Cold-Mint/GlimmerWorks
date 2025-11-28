//
// Created by Cold-Mint on 2025/10/12.
//

#include "SplashScene.h"

#include "AppContext.h"
#include "HomeScene.h"
#include "../Config.h"
#include "../log/LogCat.h"
#include "include/tweeny.h"
#include "SDL3/SDL_timer.h"

bool glimmer::SplashScene::HandleEvent(const SDL_Event &event) {
    return false;
}

void glimmer::SplashScene::Update(float delta) {
    if (!animationFinished && fadeTween.progress() < 1.0f) {
        fadeTween.step(static_cast<int>(delta * 1000));
        alpha = fadeTween.peek();
    } else if (!animationFinished && fadeTween.progress() >= 1.0f) {
        animationFinished = true;
        nextSceneTime = SDL_GetTicks() + 200;
        LogCat::i("Splash fade-in animation completed");
    }
    if (!sceneJumped && SDL_GetTicks() >= nextSceneTime) {
        sceneJumped = true;
        LogCat::i("Open Home");
        appContext->sceneManager_->ChangeScene(new HomeScene(appContext));
    }
}

void glimmer::SplashScene::Render(SDL_Renderer *renderer) {
    if (!renderer) return;
    if (!splashTexture) {
        splashTexture = appContext->resourcePackManager_->LoadTextureFromFile(
            appContext->config_->mods.enabledResourcePack, *renderer, "gui/splash.png");
        if (!splashTexture) {
            LogCat::e("Failed to load splash.png");
            return;
        }
        fadeTween = tweeny::from(0.0F).to(1.0F).during(350);
        alpha = 0.0F;
        animationFinished = false;
        LogCat::i("Splash texture loaded, starting fade-in animation");
    }
    SDL_Texture *rawTex = splashTexture.get();
    float texW = 0;
    float texH = 0;
    if (!SDL_GetTextureSize(rawTex, &texW, &texH)) {
        LogCat::e("SDL_GetTextureSize Error: ", SDL_GetError());
        return;
    }
    int winW = 0;
    int winH = 0;
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

    SDL_SetTextureAlphaMod(rawTex, static_cast<Uint8>(alpha * 255));
    if (!SDL_RenderTexture(renderer, rawTex, nullptr, &dstRect)) {
        LogCat::e("SDL_RenderTexture Error: ", SDL_GetError());
    }
}
