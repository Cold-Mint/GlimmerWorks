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
        AppContext *appContext;

    public:
        ~App();

        explicit App(AppContext *ac);

        bool init();

        void run() const;
    };
}


#endif //APP_H
