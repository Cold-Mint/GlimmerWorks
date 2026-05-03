//
// Created by coldmint on 2026/5/3.
//

#ifndef GLIMMERWORKS_MUSEBUTTONUTILS_H
#define GLIMMERWORKS_MUSEBUTTONUTILS_H
#include <unordered_map>
#include <vector>

namespace glimmer {
    class MouseButtonUtils {
        static std::unordered_map<std::string, u_int8_t> stringToMouseButtonMap_;
        static std::vector<std::string> cachedKeys_;

    public:
        [[nodiscard]] static u_int8_t StringToMouseButton(const std::string &buttonStr);

        [[nodiscard]] static bool ContainsKey(const std::string &buttonStr);

        [[nodiscard]] static const std::vector<std::string> &GetAllMouseButtonKeys();
    };
}


#endif //GLIMMERWORKS_MUSEBUTTONUTILS_H
