//
// Created by coldmint on 2026/5/1.
//

#include "CommandHookEntry.h"

uint32_t glimmer::CommandHookEntry::GetKey(const SDL_EventType eventType, const uint16_t code) {
    const uint32_t type_part = static_cast<uint32_t>(eventType) << 16U;
    const uint32_t code_part = code;
    return type_part | code_part;
}
