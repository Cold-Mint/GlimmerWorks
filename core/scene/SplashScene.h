//
// Created by Cold-Mint on 2025/10/12.
//

#ifndef GLIMMERWORKS_SPLASHSCENE_H
#define GLIMMERWORKS_SPLASHSCENE_H
#include "Scene.h"
#include "include/tween.h"

namespace Glimmer {
    class SplashScene final : public Scene {
        SDL_Texture *splashTexture = nullptr;
        float alpha = 0.0f;
        bool animationFinished = false;
        tweeny::tween<float> fadeTween;

    public:
        explicit SplashScene(AppContext *context)
            : Scene(context) {
        }

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        ~SplashScene() override {
            if (splashTexture != nullptr) {
                SDL_DestroyTexture(splashTexture);
            }
        }
    };
}


#endif //GLIMMERWORKS_SPLASHSCENE_H
