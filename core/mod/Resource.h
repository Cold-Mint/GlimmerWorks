//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef RESOURCE_H
#define RESOURCE_H
#include <string>

namespace glimmer {
    struct Resource {
        std::string packId;
        std::string key;
    };

    struct StringResource : Resource {
        std::string value;
    };

    struct TileResource : Resource {
        std::string texture;
    };
}


#endif //RESOURCE_H
