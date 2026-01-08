//
// Created by Cold-Mint on 2025/10/25.
//

#ifndef GLIMMERWORKS_WORLDSCENE_H
#define GLIMMERWORKS_WORLDSCENE_H
#include "Scene.h"
#include "../world/WorldContext.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>


namespace glimmer {
    class WorldScene : public Scene {
        std::unique_ptr<WorldContext> worldContext_;

    public:
        explicit WorldScene(AppContext *context, std::unique_ptr<WorldContext> worldContext)
            : Scene(context) {
            worldContext_ = std::move(worldContext);
        }

        void OnFrameStart() override;

        bool HandleEvent(const SDL_Event &event) override;

        bool OnBackPressed() override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;
    };
}

#endif //GLIMMERWORKS_WORLDSCENE_H
