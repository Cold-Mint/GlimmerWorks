//
// Created by Cold-Mint on 2025/10/12.
//

#ifndef GLIMMERWORKS_SPLASHSCENE_H
#define GLIMMERWORKS_SPLASHSCENE_H
#include "Scene.h"

namespace Glimmer {
    class SplashScene final : public Scene {
    public:
        explicit SplashScene(AppContext *context)
            : Scene(context) {
        }

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        ~SplashScene() override = default;
    };
}


#endif //GLIMMERWORKS_SPLASHSCENE_H
