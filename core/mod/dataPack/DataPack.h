//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACK_H
#define DATAPACK_H
#include <string>

#include "BiomeDecoratorResourcesManager.h"
#include "BiomeDecoratorType.h"
#include "BiomesManager.h"
#include "FixedColorManager.h"
#include "ItemManager.h"
#include "LightMaskManager.h"
#include "LightSourceManager.h"
#include "MobManager.h"
#include "StructureManager.h"
#include "../PackManifest.h"
#include "../../vfs/VirtualFileSystem.h"
#include "core/contributor/ContributorManager.h"
#include "core/inventory/InitialInventoryManager.h"
#include "core/lootTable/LootTableManager.h"
#include "core/mod/TomlTemplateExpander.h"
#include "core/shape/ShapeManager.h"
#include "core/shape/ShapeType.h"
#include "toml11/spec.hpp"

namespace glimmer {
    class TileManager;
    class StringManager;

    class DataPack {
        std::string rootPath_;
        DataPackManifest manifest_;
        toml::spec tomlVersion_;
        const VirtualFileSystem *virtualFileSystem_;
        const TomlTemplateExpander *tomlTemplateExpander_;


        /**
         * GetActuallyTemplateSearchPath
         * 获取真实的模板搜索路径。
         * @param path toml路径
         * @return The expanded path 展开后的路径
         */
        [[nodiscard]] std::vector<std::string> GetActuallyTemplateSearchPath(const std::string &path) const;

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


        [[nodiscard]] bool LoadInitialInventoryResourceFromFile(const std::string &path,
                                                                InitialInventoryManager *lootTableManager) const;


        [[nodiscard]] bool LoadStructureResourceFromFile(const std::string &path,
                                                         StructureManager *structureManager) const;

        [[nodiscard]] bool LoadTileResourceFromFile(const std::string &path, TileManager *tileManager) const;

        [[nodiscard]] bool LoadBiomeResourceFromFile(const std::string &path, BiomesManager *biomesManager) const;

        [[nodiscard]] bool LoadComposableItemResourceFromFile(const std::string &path, ItemManager *itemManager) const;

        [[nodiscard]] bool LoadAbilityItemResourceFromFile(const std::string &path, ItemManager *itemManager) const;

        [[nodiscard]] bool LoadContributorResourceFromFile(const std::string &path,
                                                           ContributorManager *contributorManager) const;

        [[nodiscard]] bool LoadMobResourceFromFile(const std::string &path, MobManager *mobManager) const;

        [[nodiscard]] bool LoadShapeResourceFromFile(const std::string &path, ShapeManager *shapeManager,
                                                     ShapeType type) const;

        [[nodiscard]] bool LoadFixedColorResourceFromFile(const std::string &path,
                                                          FixedColorManager *fixedColorManager) const;

        [[nodiscard]] bool LoadLightMaskResourceFromFile(const std::string &path,
                                                         LightMaskManager *lightMaskManager) const;

        [[nodiscard]] bool LoadLightSourceResourceFromFile(const std::string &path,
                                                           LightSourceManager *lightSourceManager) const;

        [[nodiscard]] bool LoadBiomeDecoratorResourceFromFile(const std::string &path,
                                                              BiomeDecoratorResourcesManager *biomeDecoratorManager,
                                                              BiomeDecoratorType type) const;

        [[nodiscard]] static std::optional<std::string> ExtractLanguageFromFileName(const std::string &fileName);

    public:
        explicit DataPack(std::string path, const VirtualFileSystem *virtualFileSystem,
                          const TomlTemplateExpander *tomlTemplateExpander,
                          const toml::spec &tomlVersion);

        bool LoadManifest();

        [[nodiscard]] bool LoadPack(AppContext *appContext) const;

        [[nodiscard]] const DataPackManifest &GetManifest() const;
    };
}


#endif //DATAPACK_H
