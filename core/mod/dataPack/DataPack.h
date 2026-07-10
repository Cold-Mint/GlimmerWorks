/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once
#include <string>

#include "BiomeDecoratorResourcesManager.h"
#include "BiomeDecoratorType.h"
#include "BiomesManager.h"
#include "FixedColorManager.h"
#include "ItemManager.h"
#include "LightMaskManager.h"
#include "LightSourceManager.h"
#include "MobManager.h"
#include "RecipeManager.h"
#include "StructureManager.h"
#include "core/Config.h"
#include "core/context/GraphicsContext.h"
#include "core/context/ModContext.h"
#include "core/mod/PackManifest.h"
#include "core/vfs/VirtualFileSystem.h"
#include "core/contributor/ContributorManager.h"
#include "core/inventory/InitialInventoryManager.h"
#include "core/lootTable/LootTableManager.h"
#include "core/mod/PackVerifyState.h"
#include "core/mod/TomlTemplateExpander.h"
#include "core/shape/ShapeManager.h"
#include "core/shape/ShapeType.h"
#include "core/world/structure/StructureGeneratorType.h"
#include "toml11/spec.hpp"
#include "toml11/types.hpp"

namespace glimmer
{
    struct SpecialFileProcessingParams;
    class TileResourceManager;
    class StringManager;

    class DataPack
    {
        std::filesystem::path rootPath_;
        DataPackManifest manifest_;
        toml::spec tomlVersion_;
        const VirtualFileSystem* virtualFileSystem_;
        const TomlTemplateExpander* tomlTemplateExpander_;
        PackVerifyState packVerifyState_ = PackVerifyState::Unsigned;


        /**
         * GetActuallyTemplateSearchPath
         * 获取真实的模板搜索路径。
         * @param path toml路径
         * @return The expanded path 展开后的路径
         */
        [[nodiscard]] std::vector<std::filesystem::path> GetActuallyTemplateSearchPath(
            const std::filesystem::path& path) const;

        /**
         * GetDataType
         * 获取数据类型
         * @param fileName fileName 文件名
         * @return
         */
        static std::optional<std::string> GetDataType(const std::string& fileName);


        [[nodiscard]] int LoadStringResourceFromFile(const std::filesystem::path& path,
                                                     StringManager* stringManager) const;

        void LoadLootTableResourceFromFile(const toml::value& value,
                                           LootTableManager* lootTableManager) const;


        void LoadInitialInventoryResourceFromFile(const toml::value& value,
                                                  InitialInventoryManager* lootTableManager) const;


        void LoadStructureResourceFromFile(const toml::value& value,
                                           StructureManager* structureManager,
                                           StructureGeneratorType structureGeneratorType) const;

        void LoadTileResourceFromFile(const toml::value& value, TileResourceManager* tileManager) const;

        void LoadBiomeResourceFromFile(const toml::value& value, BiomesManager* biomesManager) const;

        void LoadComposableItemResourceFromFile(const toml::value& value, ItemManager* itemManager) const;

        void LoadAbilityItemResourceFromFile(const toml::value& value, ItemManager* itemManager) const;

        void LoadMaterialItemResourceResourceFromFile(const toml::value& value, ItemManager* itemManager) const;

        void LoadContributorResourceFromFile(const toml::value& value,
                                             ContributorManager* contributorManager) const;

        void LoadMobResourceFromFile(const toml::value& value, MobManager* mobManager) const;

        void LoadShapeResourceFromFile(const toml::value& value, ShapeManager* shapeManager,
                                       ShapeType type) const;

        void LoadFixedColorResourceFromFile(const toml::value& value,
                                            FixedColorManager* fixedColorManager) const;

        void LoadLightMaskResourceFromFile(const toml::value& value,
                                           LightMaskManager* lightMaskManager) const;

        void LoadLightSourceResourceFromFile(const toml::value& value,
                                             LightSourceManager* lightSourceManager) const;

        void LoadBiomeDecoratorResourceFromFile(const toml::value& value,
                                                BiomeDecoratorResourcesManager* biomeDecoratorManager,
                                                BiomeDecoratorType type) const;

        void LoadRecipeResourceFromFile(const toml::value& value, RecipeManager* recipeManager) const;

        [[nodiscard]]
        static std::optional<std::string> ExtractLanguageFromFileName(std::string_view fileName);

        bool ProcessPublicKeyFile(const std::filesystem::path& path, SpecialFileProcessingParams& params) const;

        bool ProcessSignatureFile(const std::filesystem::path& path, SpecialFileProcessingParams& params) const;

        static std::optional<std::vector<char>> ReadFileContent(std::istream* stream);

        static void ComputeFileHash(const std::vector<char>& fileBuffer, std::vector<uint8_t>& allHashData);

        int LoadResourceByType(const std::string& dataType, const std::string& file,
                               const std::string& content, const ModContext* modContext,
                               const GraphicsContext* graphicsContext) const;

        int LoadLanguageFiles(const std::vector<std::filesystem::path>& defaultLanguageFiles,
                              const std::vector<std::filesystem::path>& targetLanguageFiles,
                              const ModContext* modContext) const;

        static PackVerifyState VerifySignature(bool findPublicKey, bool findSignature,
                                               const std::vector<uint8_t>& publicKey,
                                               const std::vector<uint8_t>& signature,
                                               const std::vector<uint8_t>& allHashData);

        bool ProcessSpecialFiles(const std::filesystem::path& path,
                                 SpecialFileProcessingParams& params) const;

        static bool ProcessLanguageFile(const std::filesystem::path& file,
                                        std::string_view dataType,
                                        std::string_view fileName,
                                        std::vector<std::filesystem::path>& defaultLanguageFiles,
                                        std::vector<std::filesystem::path>& targetLanguageFiles,
                                        const AppContext* appContext);

    public:
        explicit DataPack(std::string path, const VirtualFileSystem* virtualFileSystem,
                          const TomlTemplateExpander* tomlTemplateExpander,
                          const toml::spec& tomlVersion);

        bool LoadManifest();

        [[nodiscard]] PackVerifyState GetPackVerifyState() const;

        [[nodiscard]] bool LoadPack(AppContext* appContext);

        [[nodiscard]] const DataPackManifest& GetManifest() const;
    };
}
