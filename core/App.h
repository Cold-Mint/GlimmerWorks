//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef APP_H
#define APP_H
#include <SDL3/SDL_render.h>
#include "scene/AppContext.h"


namespace glimmer {
    class App {
        SDL_Window *window;
        bool initSDLSuccess;
        bool initSDLTtfSuccess;
        SDL_Renderer *renderer_;
        AppContext *appContext_;
        uint64_t lastTime_ = 0;

        void RendererUiMessage();

    public:
        ~App();

        explicit App(AppContext *ac);

        bool Init();

        void Run();
    };
}


#endif //APP_H
