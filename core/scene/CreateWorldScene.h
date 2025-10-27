//
// Created by Cold-Mint on 2025/10/20.
//

#ifndef GLIMMERWORKS_CREATEWORLDSCENE_H
#define GLIMMERWORKS_CREATEWORLDSCENE_H
#include "Scene.h"

namespace glimmer {
    class CreateWorldScene : public Scene {
    public:
        char world_name[128];
        char seed_str[64];

        explicit CreateWorldScene(AppContext *context)
            : Scene(context), world_name{}, seed_str{} {
        }

        void CreateWorld() const;

        static int RandomSeed();

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        ~CreateWorldScene() override = default;
    };
}

#endif //GLIMMERWORKS_CREATEWORLDSCENE_H
