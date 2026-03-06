//@genCode
//
// Created by coldmint on 2026/3/1.
//

#ifndef GLIMMERWORKS_CONTRIBUTOR_H
#define GLIMMERWORKS_CONTRIBUTOR_H
#include <string>

#include "../mod/ResourceRef.h"

namespace glimmer {
    //@genNextLine(Contributor|贡献者)
    struct Contributor {
        //@genNextLine(uuid|uuid)
        std::string uuid;
        //@genNextLine(name|名称)
        std::string name;
        //@genNextLine(displayName|显示名称)
        ResourceRef displayName;
        //@genNextLine(country|国家)
        std::string country;
    };
}

#endif //GLIMMERWORKS_CONTRIBUTOR_H
