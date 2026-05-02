//
// Created by coldmint on 2026/5/1.
//

#ifndef GLIMMERWORKS_COMMANDHOOK_H
#define GLIMMERWORKS_COMMANDHOOK_H
#include <string>

#include "CommandHookScope.h"
#include "SDL3/SDL_scancode.h"

namespace glimmer {
    struct CommandHookEntry {
        std::string hookId;
        CommandHookScope scope;
        std::string command;
        SDL_Scancode scancode;
    };
}

#endif //GLIMMERWORKS_COMMANDHOOK_H
