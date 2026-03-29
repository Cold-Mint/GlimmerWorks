//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef APP_H
#define APP_H
#include "scene/AppContext.h"


namespace glimmer {
    class App {
        SDL_Window *window;
        bool initSDLSuccess_;
        bool initSDLMixSuccess_;
        bool initSDLTtfSuccess_;
        SDL_Renderer *renderer_;
        AppContext *appContext_;
        uint64_t lastTime_ = 0;
        MIX_Mixer *mixer_;
        std::array<size_t, 12> commandIndexArray_ = {};

        void RendererUiMessage();

    public:
        ~App();

        explicit App(AppContext *ac);

        bool Init();

        void ExecuteHotkeyCommand(const HotkeyCommand &hotkeyCommand, int arrayIndex);

        void Run();
    };
}


#endif //APP_H
