//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef RESOURCEREF_H
#define RESOURCEREF_H
#include <string>

namespace glimmer {
    struct ResourceRef {
        std::string packId;
        int resourceType;
        std::string resourceKey;
    };
}


#endif //RESOURCEREF_H
