/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once
#include <string>

#include "CommandHookScope.h"
#include "SDL3/SDL_events.h"

namespace glimmer {
    struct CommandHookEntry {
        std::string hookId;
        CommandHookScope scope;
        std::string command;
        uint16_t code;
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
        [[nodiscard]] static uint32_t GetKey(SDL_EventType eventType, uint16_t code);
    };
}