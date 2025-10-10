//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACKMANIFEST_H
#define DATAPACKMANIFEST_H
#include <string>

#include "Resource.h"


namespace Glimmer {
    struct PackManifest {
        std::string id;
        StringResource name;
        StringResource description;
        bool resPack;
        std::string author;
        std::string versionName;
        int versionNumber;
        int minGameVersion;
    };
}


#endif //DATAPACKMANIFEST_H
