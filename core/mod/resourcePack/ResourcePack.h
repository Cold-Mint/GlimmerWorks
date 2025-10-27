//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_RESOURCEPACK_H
#define GLIMMERWORKS_RESOURCEPACK_H
#include <string>

#include "../PackManifest.h"


namespace glimmer {
    class ResourcePack {
        std::string path;
        ResourcePackManifest manifest;

    public:
        explicit ResourcePack(std::string path) : path(std::move(path)), manifest() {
        }

        bool loadManifest();

        [[nodiscard]] const ResourcePackManifest &getManifest() const;

        [[nodiscard]] std::string_view getPath() const;
    };
}


#endif //GLIMMERWORKS_RESOURCEPACK_H
