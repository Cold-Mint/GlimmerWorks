//
// Created by Cold-Mint on 2025/10/17.
//

#ifndef GLIMMERWORKS_HOMESCENE_H
#define GLIMMERWORKS_HOMESCENE_H
#include <vector>

#include "Scene.h"


namespace glimmer {
    struct Star {
        float x, y;
        Uint8 r, g, b;
        float size;
    };

    class HomeScene : public Scene {
        std::vector<Star> stars;
        int windowWidth = 0;
        int windowHeight = 0;

    public:
        explicit HomeScene(AppContext *context)
            : Scene(context) {
        }

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        void generateStars();

        bool OnBackPressed() override;

        ~HomeScene() override;
    };
}

#endif //GLIMMERWORKS_HOMESCENE_H
