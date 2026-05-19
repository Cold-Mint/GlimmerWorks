//@genCode
//
// Created by Cold-Mint on 2026/3/1.
//

#pragma once
#include <string>

#include "core/mod/ResourceRef.h"

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
