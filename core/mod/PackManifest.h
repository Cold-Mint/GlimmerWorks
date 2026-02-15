//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACKMANIFEST_H
#define DATAPACKMANIFEST_H
#include <string>

#include "ResourceRef.h"
#include "dataPack/PackDependence.h"


namespace glimmer {
    struct PackManifest {
        std::string id;
        ResourceRef name;
        ResourceRef description;
        bool resPack;
        std::string author;
        std::string versionName;
        uint32_t versionNumber;
        uint32_t minGameVersion;
    };

    struct DataPackManifest : PackManifest {
        std::vector<PackDependence> packDependencies;
    };

    struct ResourcePackManifest : PackManifest {
    };
}


#endif //DATAPACKMANIFEST_H
