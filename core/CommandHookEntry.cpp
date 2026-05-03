//
// Created by coldmint on 2026/5/1.
//

#include "CommandHookEntry.h"

uint32_t glimmer::CommandHookEntry::GetKey(const SDL_Scancode scancode, const SDL_EventType eventType) {
    constexpr uint32_t SCANCODE_BIT_SHIFT = 9;
    const uint32_t type = static_cast<uint32_t>(eventType);
    const uint32_t code = static_cast<uint32_t>(scancode);
    return type << SCANCODE_BIT_SHIFT | code;
}