//
// Created by Cold-Mint on 2025/10/12.
//

#ifndef GLIMMERWORKS_SPLASHSCENE_H
#define GLIMMERWORKS_SPLASHSCENE_H
#include <memory>

#include "Scene.h"
#include "../log/LogCat.h"
#include "include/tween.h"
#include "SDL3/SDL_timer.h"

namespace Glimmer {
    class SplashScene final : public Scene {
        std::shared_ptr<SDL_Texture> splashTexture = nullptr;
        float alpha = 0.0f;
        bool animationFinished = false;
        tweeny::tween<float> fadeTween;
        Uint64 nextSceneTime;
        bool sceneJumped = false;

    public:
        explicit SplashScene(AppContext *context)
            : Scene(context) {
            nextSceneTime = std::numeric_limits<Uint64>::max();
        }

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        ~SplashScene() override  {
            LogCat::i("SplashScene destroyed");
        };
    };
}


#endif //GLIMMERWORKS_SPLASHSCENE_H
