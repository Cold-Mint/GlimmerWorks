//
// Created by Cold-Mint on 2025/10/20.
//

#ifndef GLIMMERWORKS_CREATEWORLDSCENE_H
#define GLIMMERWORKS_CREATEWORLDSCENE_H
#include <string>

#include "Scene.h"

namespace glimmer {
    class CreateWorldScene : public Scene {
    public:
        std::string worldName_;
        std::string seedStr_;

        explicit CreateWorldScene(AppContext *context)
            : Scene(context) {
            const int newSeed = RandomSeed();
            seedStr_ = std::to_string(newSeed);
            worldName_ = RandomName();
        }

        void CreateWorld() const;

        static int RandomSeed();

        [[nodiscard]] std::string RandomName() const;

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        ~CreateWorldScene() override = default;
    };
}

#endif //GLIMMERWORKS_CREATEWORLDSCENE_H
