//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef APP_H
#define APP_H
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>

#include "log/LogCat.h"
#include "scene/AppContext.h"
#include "SDL3_ttf/SDL_ttf.h"


namespace glimmer {
    class App {
        SDL_Window *window;
        bool initSDLSuccess;
        bool initSDLTtfSuccess;
        SDL_Renderer *renderer_;
        AppContext *appContext;

    public:
        ~App() {
            LogCat::i("Destroy the app");
            if (window != nullptr) {
                SDL_DestroyWindow(window);
            }
            if (initSDLTtfSuccess) {
                TTF_Quit();
            }
            if (initSDLSuccess) {
                SDL_Quit();
            }
        }

        explicit App(AppContext *ac) : appContext(ac) {
            window = nullptr;
            renderer_ = nullptr;
            initSDLSuccess = false;
            initSDLTtfSuccess = false;
        }

        bool init();

        void run() const;
    };
}


#endif //APP_H
