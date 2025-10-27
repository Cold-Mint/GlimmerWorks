//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACK_H
#define DATAPACK_H
#include <string>
#include <utility>

#include "../PackManifest.h"

namespace glimmer {
    class StringManager;

    class DataPack {
        std::string path;
        DataPackManifest manifest;

        //Load string resources
        //加载字符串资源
        [[nodiscard]] int LoadStringResource(const std::string &language, StringManager &stringManager) const;

        [[nodiscard]] int LoadStringResourceFromFile(const std::string &path, StringManager &stringManager) const;

    public:
        explicit DataPack(std::string path) : path(std::move(path)), manifest() {
        }

        bool LoadManifest();

        [[nodiscard]] bool LoadPack(const std::string &language,
                                    StringManager &stringManager) const;

        [[nodiscard]] const DataPackManifest &GetManifest() const;
    };
}


#endif //DATAPACK_H
