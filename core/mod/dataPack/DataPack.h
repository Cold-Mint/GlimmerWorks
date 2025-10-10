//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACK_H
#define DATAPACK_H
#include <string>
#include <utility>

#include "../PackManifest.h"


namespace Glimmer {
    class DataPack {
        std::string path;
        PackManifest manifest;

    public:
        explicit DataPack(std::string path) : path(std::move(path)), manifest() {
        }

        bool loadManifest();

        [[nodiscard]] const PackManifest &getManifest() const;
    };
}


#endif //DATAPACK_H
