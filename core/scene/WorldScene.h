//
// Created by Cold-Mint on 2025/10/25.
//

#ifndef GLIMMERWORKS_WORLDSCENE_H
#define GLIMMERWORKS_WORLDSCENE_H
#include "Scene.h"
#include "../log/LogCat.h"
#include "../world/WorldGenerator.h"
#include "../world/WorldContext.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
namespace glimmer {
    class WorldScene : public Scene {
        WorldContext *worldContext;

    public:
        explicit WorldScene(AppContext *context, WorldContext *wc, WorldGenerator *wg)
            : Scene(context) {
            worldContext = wc;
        }

        void OnFrameStart() override;

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        ~WorldScene() override {
            delete worldContext;
        };
    };
}

#endif //GLIMMERWORKS_WORLDSCENE_H
