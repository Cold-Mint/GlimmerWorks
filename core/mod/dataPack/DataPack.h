//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACK_H
#define DATAPACK_H
#include <string>
#include <utility>

#include "BiomesManager.h"
#include "ItemManager.h"
#include "../PackManifest.h"
#include "../../vfs/VirtualFileSystem.h"
#include "core/lootTable/LootTableManager.h"
#include "toml11/spec.hpp"

namespace glimmer {
    class TileManager;
    class StringManager;

    class DataPack {
        std::string path_;
        DataPackManifest manifest_;
        toml::spec tomlVersion_;
        const VirtualFileSystem *virtualFileSystem_;


        /**
         * GetDataType
         * 获取数据类型
         * @param path path 路径
         * @return
         */
        [[nodiscard]] std::optional<std::string> GetDataType(const std::string &path) const;


        [[nodiscard]] int LoadStringResourceFromFile(const std::string &path, StringManager *stringManager) const;

        [[nodiscard]] bool LoadLootTableResourceFromFile(const std::string &path,
                                                         LootTableManager *lootTableManager) const;

        [[nodiscard]] bool LoadTileResourceFromFile(const std::string &path, TileManager *tileManager) const;

        [[nodiscard]] bool LoadBiomeResourceFromFile(const std::string &path, BiomesManager *biomesManager) const;

        [[nodiscard]] bool LoadComposableItemResourceFromFile(const std::string &path, ItemManager *itemManager) const;

        [[nodiscard]] bool LoadAbilityItemResourceFromFile(const std::string &path, ItemManager *itemManager) const;

        [[nodiscard]] static std::optional<std::string> ExtractLanguageFromFileName(const std::string &fileName);

    public:
        explicit DataPack(std::string path, const VirtualFileSystem *virtualFileSystem,
                          const toml::spec &tomlVersion);

        bool LoadManifest();

        [[nodiscard]] bool LoadPack(const std::string &language,
                                    StringManager *stringManager, TileManager *tileManager,
                                    BiomesManager *biomesManager, ItemManager *itemManager,
                                    LootTableManager *lootTableManager) const;

        [[nodiscard]] const DataPackManifest &GetManifest() const;
    };
}


#endif //DATAPACK_H
