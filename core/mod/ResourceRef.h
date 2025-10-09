//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef RESOURCEREF_H
#define RESOURCEREF_H
#include <string>


namespace Glimmer {
    struct ResourceRef {
        std::string packId;
        std::string resourceType;
        std::string resourceKey;
    };
}


#endif //RESOURCEREF_H
