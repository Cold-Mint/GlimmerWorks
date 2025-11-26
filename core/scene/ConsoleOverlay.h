//
// Created by Cold-Mint on 2025/10/13.
//

#ifndef GLIMMERWORKS_CONSOLESCENE_H
#define GLIMMERWORKS_CONSOLESCENE_H
#include <array>
#include <string>
#include <vector>

#include "imgui.h"
#include "Scene.h"

namespace glimmer {
    /**
     * ConsoleOverlay
     * 控制台叠加层
     *
     */
    class ConsoleOverlay : public Scene {
        bool show_ = false;
        bool focusNextFrame_ = false;
        std::array<char, 256> inputBuffer_ = {};
        std::vector<std::string> messages_;
        int lastCursorPos_ = 0;

        void addMessage(const std::string &message);

        static int InputCallback(const ImGuiInputTextCallbackData *data);

    public:
        explicit ConsoleOverlay(AppContext *context)
            : Scene(context) {
        }

        void SetLastCursorPos(int cursorPos);

        [[nodiscard]] int GetLastCursorPos() const;

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;


        void Render(SDL_Renderer *renderer) override;

        ~ConsoleOverlay() override;
    };
}

#endif //GLIMMERWORKS_CONSOLESCENE_H
