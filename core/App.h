//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef APP_H
#define APP_H
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>

#include "Config.h"
#include "log/LogCat.h"


namespace Glimmer {
    class App {
        SDL_Window *window;
        bool initSDLSuccess;
        SDL_Renderer *renderer;

    public:
        ~App() {
            LogCat::i("Destroy the app");
            if (window != nullptr) {
                SDL_DestroyWindow(window);
            }
            if (initSDLSuccess) {
                SDL_Quit();
            }
        }

        App() {
            window = nullptr;
            renderer = nullptr;
            initSDLSuccess = false;
        }

        bool init(const Config &config);

        void run() const;
    };
}


#endif //APP_H
