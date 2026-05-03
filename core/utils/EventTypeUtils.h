//
// Created by coldmint on 2026/5/3.
//

#ifndef GLIMMERWORKS_EVENTTYPEUTILS_H
#define GLIMMERWORKS_EVENTTYPEUTILS_H
#include <unordered_map>
#include <vector>

#include "SDL3/SDL_events.h"

namespace glimmer {
    class EventTypeUtils {
        static std::unordered_map<std::string, SDL_EventType> stringToEventMap_;
        static std::vector<std::string> cachedKeys_;

    public:
        /**
         * @brief 将字符串转换为对应的SDL_EventType枚举值
         * @param eventStr 事件类型字符串（小写+下划线格式，如"quit"、"key_down"）
         * @return 对应的SDL_EventType，未找到则返回SDL_EVENT_FIRST
         */
        [[nodiscard]] static SDL_EventType StringToEventType(const std::string &eventStr);

        /**
         * @brief 检查指定的字符串是否是有效的事件类型键
         * @param eventStr 事件类型字符串
         * @return 存在返回true，否则返回false
         */
        [[nodiscard]] static bool ContainsKey(const std::string &eventStr);

        /**
         * @brief 获取所有有效的事件类型字符串键
         * @return 包含所有键的const vector引用
         */
        [[nodiscard]] static const std::vector<std::string> &GetAllEventTypeKeys();
    };
}

#endif //GLIMMERWORKS_EVENTTYPEUTILS_H
