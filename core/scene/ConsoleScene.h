//
// Created by coldmint on 2025/10/13.
//

#ifndef GLIMMERWORKS_CONSOLESCENE_H
#define GLIMMERWORKS_CONSOLESCENE_H
#include <string>
#include <vector>

#include "Scene.h"

namespace Glimmer {
    class ConsoleScene : public Scene {
        bool show = false;
        char inputBuffer[256] = {};
        std::vector<std::string> messages;

    public:
        explicit ConsoleScene(AppContext *context)
            : Scene(context) {
        }

        void HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        ~ConsoleScene() override;
    };
}

#endif //GLIMMERWORKS_CONSOLESCENE_H
