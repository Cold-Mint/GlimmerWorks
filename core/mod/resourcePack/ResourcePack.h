//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_RESOURCEPACK_H
#define GLIMMERWORKS_RESOURCEPACK_H
#include <string>

#include "../PackManifest.h"
#include "../../vfs/VirtualFileSystem.h"


namespace glimmer {
    class ResourcePack {
        std::string path_;
        ResourcePackManifest manifest;
        const VirtualFileSystem *virtualFileSystem_;

    public:
        explicit ResourcePack(std::string path, const VirtualFileSystem *virtualFileSystem) : path_(std::move(path)),
            virtualFileSystem_(virtualFileSystem), manifest() {
        }

        bool loadManifest();

        [[nodiscard]] const ResourcePackManifest &getManifest() const;

        [[nodiscard]] std::string getPath() const;
    };
}


#endif //GLIMMERWORKS_RESOURCEPACK_H
