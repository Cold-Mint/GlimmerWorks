//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACK_H
#define DATAPACK_H
#include <string>
#include <utility>

#include "StringManager.h"
#include "../PackManifest.h"


namespace Glimmer {
    class DataPack {
        std::string path;
        PackManifest manifest;

        //Load string resources
        //加载字符串资源
        [[nodiscard]] int loadStringResource(const std::string &language, StringManager &stringManager) const;

        [[nodiscard]] int loadStringResourceFromFile(const std::string &path, StringManager &stringManager) const;

    public:
        explicit DataPack(std::string path) : path(std::move(path)), manifest() {
        }

        bool loadManifest();

        [[nodiscard]] bool loadPack(const std::string &language,
                                    StringManager &stringManager) const;

        [[nodiscard]] const PackManifest &getManifest() const;
    };
}


#endif //DATAPACK_H
