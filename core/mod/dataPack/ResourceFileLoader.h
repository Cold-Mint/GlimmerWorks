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
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>

#include "BiomeDecoratorType.h"
#include "core/mod/PackManifest.h"
#include "core/shape/ShapeType.h"
#include "core/world/structure/StructureConditionProcessorType.h"
#include "core/world/structure/StructureGeneratorType.h"
#include "toml11/spec.hpp"
#include "toml11/types.hpp"

namespace glimmer {
    class VirtualFileSystem;
    class TomlTemplateExpander;
    class ModContext;
    class GraphicsContext;
    class StringManager;
    class LootTableRegistry;
    class InitialInventoryManager;
    class StructureRegistry;
    class TileResourceManager;
    class BiomeRegistry;
    class DimensionRegistry;
    class ComposableItemRegistry;
    class AbilityItemRegistry;
    class MaterialItemRegistry;
    class ContributorManager;
    class MobRegistry;
    class ShapeManager;
    class FixedColorManager;
    class LightMaskManager;
    class LightSourceManager;
    class BiomeDecoratorRegistry;
    class StructurePlacementConditionsRegistry;
    class RecipeManager;

    /**
     * ResourceFileLoader
     * 资源文件加载器
     * Loads every data-pack resource type from parsed TOML values, dispatching by data type
     * through a handler registry (Factory Method + Strategy).
     * 从解析后的 TOML 值加载各类数据包资源，通过处理器注册表（工厂方法 + 策略）按数据类型分发。
     */
    class ResourceFileLoader {
        std::filesystem::path rootPath_;
        const DataPackManifest *manifest_;
        const VirtualFileSystem *virtualFileSystem_;
        const TomlTemplateExpander *tomlTemplateExpander_;
        toml::spec tomlVersion_;

        /**
         * LoadHandler
         * 资源加载处理器
         * A handler receives the parsed TOML value and the contexts used to locate target managers.
         * 处理器接收解析后的 TOML 值以及用于定位目标管理器的上下文。
         */
        using LoadHandler = std::function<void(const toml::value &, const ModContext *, const GraphicsContext *)>;

        std::unordered_map<std::string, LoadHandler> handlerMap_;

        /**
         * GetActuallyTemplateSearchPath
         * 获取真实的模板搜索路径
         * @param path path TOML 路径
         * @return The expanded search paths 展开后的搜索路径
         */
        [[nodiscard]] std::vector<std::filesystem::path> GetActuallyTemplateSearchPath(
            const std::filesystem::path &path) const;

        int LoadStringResourceFromFile(const std::filesystem::path &path, StringManager *stringManager) const;

        void LoadLootTableResourceFromFile(const toml::value &value, LootTableRegistry *lootTableRegistry) const;

        void LoadInitialInventoryResourceFromFile(const toml::value &value,
                                                  InitialInventoryManager *initialInventoryManager) const;

        void LoadStructureResourceFromFile(const toml::value &value, StructureRegistry *structureRegistry,
                                           StructureGeneratorType structureGeneratorType) const;

        void LoadTileResourceFromFile(const toml::value &value, TileResourceManager *tileManager) const;

        void LoadBiomeResourceFromFile(const toml::value &value, BiomeRegistry *biomeRegistry) const;

        void LoadDimensionResourceFromFile(const toml::value &value, DimensionRegistry *dimensionRegistry) const;

        void LoadComposableItemResourceFromFile(const toml::value &value,
                                                ComposableItemRegistry *composableItemRegistry) const;

        void LoadAbilityItemResourceFromFile(const toml::value &value, AbilityItemRegistry *abilityItemRegistry) const;

        void LoadMaterialItemResourceResourceFromFile(const toml::value &value,
                                                      MaterialItemRegistry *materialItemRegistry) const;

        void LoadContributorResourceFromFile(const toml::value &value, ContributorManager *contributorManager) const;

        void LoadMobResourceFromFile(const toml::value &value, MobRegistry *mobRegistry) const;

        void LoadShapeResourceFromFile(const toml::value &value, ShapeManager *shapeManager, ShapeType type) const;

        void LoadFixedColorResourceFromFile(const toml::value &value, FixedColorManager *fixedColorManager) const;

        void LoadLightMaskResourceFromFile(const toml::value &value, LightMaskManager *lightMaskManager) const;

        void LoadLightSourceResourceFromFile(const toml::value &value, LightSourceManager *lightSourceManager) const;

        void LoadBiomeDecoratorResourceFromFile(const toml::value &value,
                                                BiomeDecoratorRegistry *biomeDecoratorRegistry,
                                                BiomeDecoratorType type) const;

        void LoadStructurePlacementConditionsResourceFromFile(const toml::value &value,
                                                              StructurePlacementConditionsRegistry *
                                                              structurePlacementConditionsRegistry,
                                                              StructureConditionProcessorType processorType) const;

        void LoadRecipeResourceFromFile(const toml::value &value, RecipeManager *recipeManager) const;

        /**
         * RegisterHandlers
         * 注册数据类型到加载处理器的映射
         */
        void RegisterHandlers();

    public:
        /**
         * ResourceFileLoader
         * 资源文件加载器构造
         * @param rootPath rootPath 数据包根路径
         * @param manifest manifest 数据包清单
         * @param virtualFileSystem virtualFileSystem 虚拟文件系统
         * @param tomlTemplateExpander tomlTemplateExpander TOML 模板展开器
         * @param tomlVersion tomlVersion TOML 版本
         */
        ResourceFileLoader(std::filesystem::path rootPath, const DataPackManifest *manifest,
                           const VirtualFileSystem *virtualFileSystem,
                           const TomlTemplateExpander *tomlTemplateExpander, const toml::spec &tomlVersion);

        /**
         * LoadLanguageFiles
         * 加载语言文件
         * @param defaultLanguageFiles defaultLanguageFiles 默认语言文件
         * @param targetLanguageFiles targetLanguageFiles 目标语言文件
         * @param modContext modContext 模组上下文
         * @return The number of loaded strings 加载的字符串数量
         */
        int LoadLanguageFiles(const std::vector<std::filesystem::path> &defaultLanguageFiles,
                              const std::vector<std::filesystem::path> &targetLanguageFiles,
                              const ModContext *modContext) const;

        /**
         * LoadResourceByType
         * 按数据类型加载资源
         * @param dataType dataType 数据类型
         * @param file file 文件路径
         * @param content content 文件内容
         * @param modContext modContext 模组上下文
         * @param graphicsContext graphicsContext 图形上下文
         * @return 1 if loaded, 0 otherwise 加载成功返回1，否则返回0
         */
        int LoadResourceByType(const std::string &dataType, const std::string &file,
                               const std::string &content, const ModContext *modContext,
                               const GraphicsContext *graphicsContext) const;
    };
}
