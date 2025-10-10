//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef RESOURCE_H
#define RESOURCE_H
#include <string>

namespace Glimmer {
    struct Resource {
        std::string packId;
        std::string key;
    };

    struct StringResource : Resource {
        //If the local string cannot be found, use this field.
        //如果找不到本地字符串，则使用此字段。
        std::string fallback;
    };
}


#endif //RESOURCE_H
