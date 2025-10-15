//
// Created by Cold-Mint on 2025/10/13.
//

#ifndef GLIMMERWORKS_CONSOLESCENE_H
#define GLIMMERWORKS_CONSOLESCENE_H
#include <array>
#include <string>
#include <vector>

#include "Scene.h"

namespace Glimmer {
    class ConsoleScene : public Scene {
        bool show = false;
        bool focusNextFrame = false;
        std::array<char, 256> inputBuffer = {};
        std::vector<std::string> messages;

        void addMessage(const std::string &message);

    public:
        explicit ConsoleScene(AppContext *context)
            : Scene(context) {
        }

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;


        void Render(SDL_Renderer *renderer) override;

        ~ConsoleScene() override;
    };
}

#endif //GLIMMERWORKS_CONSOLESCENE_H
