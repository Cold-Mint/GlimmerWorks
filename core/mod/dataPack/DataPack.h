//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACK_H
#define DATAPACK_H
#include <string>
#include <utility>

#include "BiomesManager.h"
#include "../PackManifest.h"
#include "../../vfs/VirtualFileSystem.h"

namespace glimmer {
    class TileManager;
    class StringManager;

    class DataPack {
        std::string path_;
        DataPackManifest manifest_;
        const VirtualFileSystem *virtualFileSystem_;

        //Load string resources
        //加载字符串资源
        [[nodiscard]] int LoadStringResource(const std::string &language, StringManager &stringManager) const;

        [[nodiscard]] int LoadTileResource(TileManager &tileManager) const;

        [[nodiscard]] int LoadBiomeResource(BiomesManager &biomesManager) const;

        [[nodiscard]] int LoadStringResourceFromFile(const std::string &path, StringManager &stringManager) const;

        [[nodiscard]] bool LoadTileResourceFromFile(const std::string &path, TileManager &tileManager) const;

        [[nodiscard]] bool LoadBiomeResourceFromFile(const std::string &path, BiomesManager &biomesManager) const;

    public:
        explicit DataPack(std::string path, const VirtualFileSystem *virtualFileSystem) : path_(std::move(path)),
            virtualFileSystem_(virtualFileSystem), manifest_() {
        }

        bool LoadManifest();

        [[nodiscard]] bool LoadPack(const std::string &language,
                                    StringManager &stringManager, TileManager &tileManager,BiomesManager &biomesManager) const;

        [[nodiscard]] const DataPackManifest &GetManifest() const;
    };
}


#endif //DATAPACK_H
