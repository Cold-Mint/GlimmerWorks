//
// Created by coldmint on 2026/5/1.
//

#ifndef GLIMMERWORKS_COMMANDHOOK_H
#define GLIMMERWORKS_COMMANDHOOK_H
#include <string>

#include "CommandHookScope.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_events.h"

namespace glimmer {
    struct CommandHookEntry {
        std::string hookId;
        CommandHookScope scope;
        std::string command;
        SDL_Scancode scancode;
        SDL_EventType eventType;
        //Whether to execute during repeated input.
        //是否在重复输入时执行。
        //For example, press A continuously.
        //例如连续按下A。
        bool keyRepeat = false;

        /**
        * By using the bit concatenation algorithm to combine event types and scan codes, an absolutely unique 32-bit unsigned key value is generated, ensuring that different event + scan code combinations correspond to a unique key.
        * 通过位拼接算法组合事件类型和扫描码，生成绝对唯一的32位无符号键值，保证不同事件+扫描码组合对应唯一键
        * @return
        */
        [[nodiscard]] static uint32_t GetKey(SDL_Scancode scancode, SDL_EventType eventType);
    };
}

#endif //GLIMMERWORKS_COMMANDHOOK_H
