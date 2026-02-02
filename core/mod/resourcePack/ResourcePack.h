//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_RESOURCEPACK_H
#define GLIMMERWORKS_RESOURCEPACK_H
#include <string>

#include "../PackManifest.h"
#include "../../vfs/VirtualFileSystem.h"
#include "toml11/spec.hpp"


namespace glimmer {
    class ResourcePack {
        std::string path_;
        ResourcePackManifest manifest_;
        const VirtualFileSystem *virtualFileSystem_;
        toml::spec tomlVersion_;

    public:
        explicit ResourcePack(std::string path, const VirtualFileSystem *virtualFileSystem,
                              const toml::spec &tomlVersion);

        bool loadManifest();

        [[nodiscard]] const ResourcePackManifest &getManifest() const;

        [[nodiscard]] std::string getPath() const;
    };
}


#endif //GLIMMERWORKS_RESOURCEPACK_H
