//
// Created by Cold-Mint on 2025/10/17.
//

#ifndef GLIMMERWORKS_HOMESCENE_H
#define GLIMMERWORKS_HOMESCENE_H
#include <memory>

#include "Scene.h"


namespace Glimmer {
    class HomeScene : public Scene {
        std::shared_ptr<SDL_Texture> splashTexture = nullptr;

    public:
        explicit HomeScene(AppContext *context)
            : Scene(context) {
        }

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        ~HomeScene() override;
    };
}

#endif //GLIMMERWORKS_HOMESCENE_H
