//
// Created by coldmint on 2026/3/1.
//

#ifndef GLIMMERWORKS_CONTRIBUTOR_H
#define GLIMMERWORKS_CONTRIBUTOR_H
#include <string>

#include "core/mod/ResourceRef.h"

namespace glimmer {
    struct Contributor {
        std::string uuid;
        std::string name;
        ResourceRef displayName;
        std::string country;
    };
}

#endif //GLIMMERWORKS_CONTRIBUTOR_H
