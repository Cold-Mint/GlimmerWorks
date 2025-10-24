//
// Created by Cold-Mint on 2025/10/19.
//

#ifndef GLIMMERWORKS_MAPMANIFEST_H
#define GLIMMERWORKS_MAPMANIFEST_H
#include <string>

namespace Glimmer {
    struct MapManifest {
        std::string name;
        std::string gameVersionName;
        int gameVersionNumber;
        int seed;
    };
}

#endif //GLIMMERWORKS_MAPMANIFEST_H
