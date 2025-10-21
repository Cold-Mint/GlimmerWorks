//
// Created by Cold-Mint on 2025/10/19.
//

#ifndef GLIMMERWORKS_SAVEDGAMESSCENE_H
#define GLIMMERWORKS_SAVEDGAMESSCENE_H
#include "Scene.h"


namespace Glimmer {
    class SavedGamesScene : public Scene {
    public:
        explicit SavedGamesScene(AppContext *context)
            : Scene(context) {
        }

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        ~SavedGamesScene() override = default;
    };
}


#endif //GLIMMERWORKS_SAVEDGAMESSCENE_H
