//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACKMANIFEST_H
#define DATAPACKMANIFEST_H
#include <string>

#include "ResourceRef.h"


namespace glimmer {
    struct PackManifest {
        std::string id;
        ResourceRef name;
        ResourceRef description;
        bool resPack;
        std::string author;
        std::string versionName;
        int versionNumber;
        int minGameVersion;
    };

    struct DataPackManifest : PackManifest {
    };

    struct ResourcePackManifest : PackManifest {
    };
}


#endif //DATAPACKMANIFEST_H
