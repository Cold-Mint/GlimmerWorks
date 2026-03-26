//
// Created by Cold-Mint on 2025/10/17.
//

#ifndef GLIMMERWORKS_HOMESCENE_H
#define GLIMMERWORKS_HOMESCENE_H
#include <memory>
#include <vector>
#include "Scene.h"
#include "core/Hyperlink.h"
#include "SDL3_mixer/SDL_mixer.h"

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
        std::vector<Hyperlink> hyperlinks;
        std::string copyright;
        std::shared_ptr<MIX_Audio> homeBGM_;

        static std::string GetCopyrightString();

    public:
        explicit HomeScene(AppContext *context);

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        void generateStars();

        bool OnBackPressed() override;

        ~HomeScene() override;
    };
}

#endif //GLIMMERWORKS_HOMESCENE_H
