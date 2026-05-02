//
// Created by coldmint on 2026/5/2.
//

#ifndef GLIMMERWORKS_UUIDUTILS_H
#define GLIMMERWORKS_UUIDUTILS_H
#include <string>

namespace glimmer {
    class UUIDUtils {
    public:
        /**
         * Generate
         * 生成UUID
         * @return
         */
        [[nodiscard]] static std::string Generate();
    };
}


#endif //GLIMMERWORKS_UUIDUTILS_H
