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

        //Load string resources
        //加载字符串资源
        [[nodiscard]] int loadStringResource() const;

        [[nodiscard]] int loadStringResourceFromFile(const std::string &path) const;

    public:
        explicit DataPack(std::string path) : path(std::move(path)), manifest() {
        }

        bool loadManifest();

        [[nodiscard]] bool loadPack() const;

        [[nodiscard]] const PackManifest &getManifest() const;
    };
}


#endif //DATAPACK_H
