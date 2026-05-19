//
// Created by Cold-Mint on 2025/10/15.
//

#pragma once
#include <string>

#include "core/mod/PackManifest.h"
#include "core/vfs/VirtualFileSystem.h"
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
