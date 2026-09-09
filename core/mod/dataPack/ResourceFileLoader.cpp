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
#include "ResourceFileLoader.h"

#include <utility>

#include "AbilityItemRegistry.h"
#include "BiomeRegistry.h"
#include "ComposableItemRegistry.h"
#include "DimensionRegistry.h"
#include "FixedColorManager.h"
#include "LightMaskManager.h"
#include "LightSourceManager.h"
#include "MaterialItemRegistry.h"
#include "MobRegistry.h"
#include "RecipeManager.h"
#include "StringManager.h"
#include "StructureRegistry.h"
#include "TileResourceManager.h"
#include "core/config/Constants.h"
#include "core/context/GraphicsContext.h"
#include "core/context/ModContext.h"
#include "core/contributor/ContributorManager.h"
#include "core/inventory/InitialInventoryManager.h"
#include "core/lootTable/LootTableRegistry.h"
#include "core/mod/dataPack/BiomeDecoratorRegistry.h"
#include "core/mod/dataPack/StructurePlacementConditionsRegistry.h"
#include "core/shape/ShapeManager.h"
#include "core/utils/StringUtils.h"
#include "core/utils/TomlUtils.h"
#include "toml11/parser.hpp"

using enum glimmer::ShapeType;
using enum glimmer::BiomeDecoratorType;

glimmer::ResourceFileLoader::ResourceFileLoader(std::filesystem::path rootPath, const DataPackManifest *manifest,
                                                const VirtualFileSystem *virtualFileSystem,
                                                const TomlTemplateExpander *tomlTemplateExpander,
                                                const toml::spec &tomlVersion)
    : rootPath_(std::move(rootPath)),
      manifest_(manifest),
      virtualFileSystem_(virtualFileSystem),
      tomlTemplateExpander_(tomlTemplateExpander),
      tomlVersion_(tomlVersion) {
    RegisterHandlers();
}

void glimmer::ResourceFileLoader::RegisterHandlers() {
    handlerMap_[DATA_FILE_TYPE_TILE] = [this](const toml::value &v, const ModContext *m, const GraphicsContext *) {
        LoadTileResourceFromFile(v, m->GetTileResourceManager());
    };
    handlerMap_[DATA_FILE_TYPE_BIOME] = [this](const toml::value &v, const ModContext *m, const GraphicsContext *) {
        LoadBiomeResourceFromFile(v, m->GetBiomeRegistry());
    };
    handlerMap_[DATA_FILE_TYPE_DIMENSION] = [this](const toml::value &v, const ModContext *m, const GraphicsContext *) {
        LoadDimensionResourceFromFile(v, m->GetDimensionRegistry());
    };
    handlerMap_[DATA_FILE_TYPE_COMPOSABLE_ITEM] = [this](const toml::value &v, const ModContext *m,
                                                         const GraphicsContext *) {
        LoadComposableItemResourceFromFile(v, m->GetComposableItemRegistry());
    };
    handlerMap_[DATA_FILE_TYPE_ABILITY_ITEM] = [this](const toml::value &v, const ModContext *m,
                                                      const GraphicsContext *) {
        LoadAbilityItemResourceFromFile(v, m->GetAbilityItemRegistry());
    };
    handlerMap_[DATA_FILE_TYPE_MATERIAL_ITEM] = [this](const toml::value &v, const ModContext *m,
                                                       const GraphicsContext *) {
        LoadMaterialItemResourceResourceFromFile(v, m->GetMaterialItemRegistry());
    };
    handlerMap_[DATA_FILE_TYPE_LOOT_TABLE] = [this
            ](const toml::value &v, const ModContext *m, const GraphicsContext *) {
                LoadLootTableResourceFromFile(v, m->GetLootTableRegistry());
            };
    handlerMap_[DATA_FILE_TYPE_TREE_STRUCTURE] = [this](const toml::value &v, const ModContext *m,
                                                        const GraphicsContext *) {
        LoadStructureResourceFromFile(v, m->GetStructureRegistry(), StructureGeneratorType::Tree);
    };
    handlerMap_[DATA_FILE_TYPE_STATIC_STRUCTURE] = [this](const toml::value &v, const ModContext *m,
                                                          const GraphicsContext *) {
        LoadStructureResourceFromFile(v, m->GetStructureRegistry(), StructureGeneratorType::Static);
    };
    handlerMap_[DATA_FILE_TYPE_INITIAL_INVENTORY] = [this](const toml::value &v, const ModContext *m,
                                                           const GraphicsContext *) {
        LoadInitialInventoryResourceFromFile(v, m->GetInitialInventoryManager());
    };
    handlerMap_[DATA_FILE_TYPE_CONTRIBUTOR] = [this](const toml::value &v, const ModContext *m,
                                                     const GraphicsContext *) {
        LoadContributorResourceFromFile(v, m->GetContributorManager());
    };
    handlerMap_[DATA_FILE_TYPE_MOB] = [this](const toml::value &v, const ModContext *m, const GraphicsContext *) {
        LoadMobResourceFromFile(v, m->GetMobRegistry());
    };
    handlerMap_[DATA_FILE_TYPE_SHAPE_CIRCLE] = [this](const toml::value &v, const ModContext *m,
                                                      const GraphicsContext *) {
        LoadShapeResourceFromFile(v, m->GetShapeManager(), CIRCLE);
    };
    handlerMap_[DATA_FILE_TYPE_SHAPE_RECTANGLE] = [this](const toml::value &v, const ModContext *m,
                                                         const GraphicsContext *) {
        LoadShapeResourceFromFile(v, m->GetShapeManager(), RECTANGLE);
    };
    handlerMap_[DATA_FILE_TYPE_SHAPE_ROUNDED_RECTANGLE] = [this](const toml::value &v, const ModContext *m,
                                                                 const GraphicsContext *) {
        LoadShapeResourceFromFile(v, m->GetShapeManager(), ROUNDED_RECTANGLE);
    };
    handlerMap_[DATA_FILE_TYPE_DECORATOR_FILL] = [this](const toml::value &v, const ModContext *m,
                                                        const GraphicsContext *) {
        LoadBiomeDecoratorResourceFromFile(v, m->GetBiomeDecoratorRegistry(), FILL);
    };
    handlerMap_[DATA_FILE_TYPE_DECORATOR_MINERAL] = [this](const toml::value &v, const ModContext *m,
                                                           const GraphicsContext *) {
        LoadBiomeDecoratorResourceFromFile(v, m->GetBiomeDecoratorRegistry(), MINERAL);
    };
    handlerMap_[DATA_FILE_TYPE_DECORATOR_SURFACE] = [this](const toml::value &v, const ModContext *m,
                                                           const GraphicsContext *) {
        LoadBiomeDecoratorResourceFromFile(v, m->GetBiomeDecoratorRegistry(), SURFACE);
    };
    handlerMap_[DATA_FILE_TYPE_FIXED_COLOR] = [this
            ](const toml::value &v, const ModContext *, const GraphicsContext *g) {
                LoadFixedColorResourceFromFile(v, g->GetFixedColorManager());
            };
    handlerMap_[DATA_FILE_TYPE_LIGHT_MASK] = [this
            ](const toml::value &v, const ModContext *, const GraphicsContext *g) {
                LoadLightMaskResourceFromFile(v, g->GetLightMaskManager());
            };
    handlerMap_[DATA_FILE_TYPE_LIGHT_SOURCE] = [this](const toml::value &v, const ModContext *,
                                                      const GraphicsContext *g) {
        LoadLightSourceResourceFromFile(v, g->GetLightSourceManager());
    };
    handlerMap_[DATA_FILE_TYPE_RECIPE] = [this](const toml::value &v, const ModContext *m, const GraphicsContext *) {
        LoadRecipeResourceFromFile(v, m->GetRecipeManager());
    };
    handlerMap_[DATA_FILE_TYPE_BIOME_STRUCTURE_CONDITION] = [this](const toml::value &v, const ModContext *m,
                                                                   const GraphicsContext *) {
        LoadStructurePlacementConditionsResourceFromFile(v, m->GetStructurePlacementConditionsRegistry(),
                                                         StructureConditionProcessorType::Biome);
    };
    handlerMap_[DATA_FILE_TYPE_HEIGHT_STRUCTURE_CONDITION] = [this](const toml::value &v, const ModContext *m,
                                                                    const GraphicsContext *) {
        LoadStructurePlacementConditionsResourceFromFile(v, m->GetStructurePlacementConditionsRegistry(),
                                                         StructureConditionProcessorType::Height);
    };
    handlerMap_[DATA_FILE_TYPE_HORIZONTAL_STRUCTURE_CONDITION] = [this](const toml::value &v, const ModContext *m,
                                                                        const GraphicsContext *) {
        LoadStructurePlacementConditionsResourceFromFile(v, m->GetStructurePlacementConditionsRegistry(),
                                                         StructureConditionProcessorType::HorizontalSpacing);
    };
    handlerMap_[DATA_FILE_TYPE_SURFACE_STRUCTURE_CONDITION] = [this](const toml::value &v, const ModContext *m,
                                                                     const GraphicsContext *) {
        LoadStructurePlacementConditionsResourceFromFile(v, m->GetStructurePlacementConditionsRegistry(),
                                                         StructureConditionProcessorType::Surface);
    };
}

std::vector<std::filesystem::path> glimmer::ResourceFileLoader::GetActuallyTemplateSearchPath(
    const std::filesystem::path &path) const {
    const std::optional<std::filesystem::path> currentOptional = virtualFileSystem_->GetParentPath(path);
    if (!currentOptional.has_value()) {
        return {};
    }
    const std::string currentDir = currentOptional.value().string();
    std::vector<std::filesystem::path> result;
    for (std::string searchPath: manifest_->templateSearchPath) {
        StringUtils::ReplaceAll(searchPath, TEMPLATE_CURRENT, currentDir);
        StringUtils::ReplaceAll(searchPath, TEMPLATE_ROOT, rootPath_.string());
        result.emplace_back(std::move(searchPath));
    }
    return result;
}

int glimmer::ResourceFileLoader::LoadStringResourceFromFile(const std::filesystem::path &path,
                                                            StringManager *stringManager) const {
    const auto contentOptional = virtualFileSystem_->ReadFileAsString(path);
    if (!contentOptional.has_value()) {
        return 0;
    }
    const std::vector<std::filesystem::path> searchPath = GetActuallyTemplateSearchPath(path);
    if (searchPath.empty()) {
        return 0;
    }
    const toml::value value = toml::parse_str(
        tomlTemplateExpander_->Expand(searchPath, contentOptional.value(), virtualFileSystem_), tomlVersion_);
    int count = 0;
    auto array = toml::find<std::vector<StringResource> >(value, "string");
    for (auto &stringRes: array) {
        stringRes.packId = manifest_->id;
        stringManager->AddResource(
            std::make_unique<StringResource>(std::move(stringRes))
        );
        count++;
    }

    auto tagArray = toml::find<std::vector<StringResource> >(value, "tag_string");
    for (auto &stringRes: tagArray) {
        stringRes.packId = manifest_->id;
        stringManager->SetTagTranslate(
            StringUtils::StringToUint64(stringRes.resourceId), stringRes.value
        );
        count++;
    }
    return count;
}

void glimmer::ResourceFileLoader::LoadLootTableResourceFromFile(const toml::value &value,
                                                                LootTableRegistry *lootTableRegistry) const {
    auto lootResource = std::make_unique<LootResource>(toml::get<LootResource>(value));
    lootResource->packId = manifest_->id;
    for (auto &mandatory: lootResource->mandatory) {
        mandatory.item.SetSelfPackageId(manifest_->id);
        mandatory.mandatory = true;
    }
    for (auto &pool: lootResource->pool) {
        pool.item.SetSelfPackageId(manifest_->id);
        pool.mandatory = false;
    }
    lootTableRegistry->Register(std::move(lootResource));
}

void glimmer::ResourceFileLoader::LoadInitialInventoryResourceFromFile(const toml::value &value,
                                                                       InitialInventoryManager *initialInventoryManager)
const {
    auto initialInventoryResource = std::make_unique<InitialInventoryResource>(
        toml::get<InitialInventoryResource>(value));
    initialInventoryResource->packId = manifest_->id;
    for (auto &itemMessage: initialInventoryResource->addItems) {
        itemMessage.item.SetSelfPackageId(manifest_->id);
        for (auto &abilityItemRef: itemMessage.abilityItemRef) {
            abilityItemRef.item.SetSelfPackageId(manifest_->id);
        }
    }
    initialInventoryManager->AddResource(std::move(initialInventoryResource));
}

void glimmer::ResourceFileLoader::LoadStructureResourceFromFile(const toml::value &value,
                                                                StructureRegistry *structureRegistry,
                                                                StructureGeneratorType structureGeneratorType) const {
    std::unique_ptr<IStructureResource> structureResource;
    switch (structureGeneratorType) {
        case StructureGeneratorType::Tree:
            structureResource = std::make_unique<TreeStructureResource>(
                toml::get<TreeStructureResource>(value));
            break;
        case StructureGeneratorType::Static: {
            std::unique_ptr<StaticStructureResource> staticStructureResource = std::make_unique<
                StaticStructureResource>(
                toml::get<StaticStructureResource>(value));
            for (auto &tile_info: staticStructureResource->tileInfo) {
                tile_info.tile.SetSelfPackageId(manifest_->id);
            }
            structureResource = std::move(staticStructureResource);
        }
        break;
        case StructureGeneratorType::None:
            break;
    }
    structureResource->packId = manifest_->id;
    structureResource->generatorId = std::to_underlying(structureGeneratorType);
    for (auto &ref: structureResource->data) {
        ref.SetSelfPackageId(manifest_->id);
    }
    for (auto &condition: structureResource->condition) {
        condition.SetSelfPackageId(manifest_->id);
    }
    structureRegistry->Register(std::move(structureResource));
}

void glimmer::ResourceFileLoader::LoadTileResourceFromFile(const toml::value &value,
                                                           TileResourceManager *tileManager) const {
    auto tileResource = std::make_unique<TileResource>(toml::get<TileResource>(value));
    tileResource->packId = manifest_->id;
    tileResource->name.SetSelfPackageId(manifest_->id);
    tileResource->description.SetSelfPackageId(manifest_->id);
    tileResource->texture.SetSelfPackageId(manifest_->id);
    tileResource->pipeline.SetSelfPackageId(manifest_->id);
    tileResource->sampler.SetSelfPackageId(manifest_->id);
    tileResource->blueprintTexture.SetSelfPackageId(manifest_->id);
    tileResource->breakSfx.SetSelfPackageId(manifest_->id);
    tileResource->placeSfx.SetSelfPackageId(manifest_->id);
    tileResource->lightSource.SetSelfPackageId(manifest_->id);
    tileResource->sideLightMask.SetSelfPackageId(manifest_->id);
    tileResource->backLightMask.SetSelfPackageId(manifest_->id);
    for (auto &tag: tileResource->tags) {
        tag.MakeCachedTag();
    }
    if (tileResource->customLootTable) {
        tileResource->lootTable.SetSelfPackageId(manifest_->id);
    }
    tileManager->AddResource(std::move(tileResource));
}

void glimmer::ResourceFileLoader::LoadBiomeResourceFromFile(const toml::value &value,
                                                            BiomeRegistry *biomeRegistry) const {
    auto biomeResource = std::make_unique<BiomeResource>(toml::get<BiomeResource>(value));
    biomeResource->packId = manifest_->id;
    biomeResource->bgm.SetSelfPackageId(manifest_->id);
    for (auto &decorator: biomeResource->decors) {
        decorator.SetSelfPackageId(manifest_->id);
    }
    for (auto &dimension: biomeResource->dimensions) {
        dimension.SetSelfPackageId(manifest_->id);
    }
    biomeRegistry->Register(std::move(biomeResource));
}

void glimmer::ResourceFileLoader::LoadDimensionResourceFromFile(const toml::value &value,
                                                                DimensionRegistry *dimensionRegistry) const {
    auto dimensionResource = std::make_unique<DimensionResource>(toml::get<DimensionResource>(value));
    dimensionResource->packId = manifest_->id;
    dimensionResource->name.SetSelfPackageId(manifest_->id);
    for (auto &ambientLightKeyframe: dimensionResource->ambientLightKeyframes) {
        ambientLightKeyframe.color.SetSelfPackageId(manifest_->id);
    }
    dimensionRegistry->Register(std::move(dimensionResource));
}

void glimmer::ResourceFileLoader::LoadComposableItemResourceFromFile(const toml::value &value,
                                                                     ComposableItemRegistry *composableItemRegistry)
const {
    auto itemResource = std::make_unique<ComposableItemResource>(toml::get<ComposableItemResource>(value));
    itemResource->packId = manifest_->id;
    itemResource->name.SetSelfPackageId(manifest_->id);
    itemResource->description.SetSelfPackageId(manifest_->id);
    itemResource->texture.SetSelfPackageId(manifest_->id);
    itemResource->pipeline.SetSelfPackageId(manifest_->id);
    itemResource->sampler.SetSelfPackageId(manifest_->id);
    itemResource->lightSource.SetSelfPackageId(manifest_->id);
    for (auto &tag: itemResource->tags) {
        tag.MakeCachedTag();
    }
    for (auto &defaultAbility: itemResource->defaultAbilityList) {
        defaultAbility.item.SetSelfPackageId(manifest_->id);
        for (auto &abilityItemRef: defaultAbility.abilityItemRef) {
            abilityItemRef.item.SetSelfPackageId(manifest_->id);
        }
    }
    composableItemRegistry->Register(std::move(itemResource));
}

void glimmer::ResourceFileLoader::LoadAbilityItemResourceFromFile(const toml::value &value,
                                                                  AbilityItemRegistry *abilityItemRegistry) const {
    auto itemResource = std::make_unique<AbilityItemResource>(toml::get<AbilityItemResource>(value));
    itemResource->packId = manifest_->id;
    itemResource->name.SetSelfPackageId(manifest_->id);
    itemResource->description.SetSelfPackageId(manifest_->id);
    itemResource->texture.SetSelfPackageId(manifest_->id);
    itemResource->pipeline.SetSelfPackageId(manifest_->id);
    itemResource->sampler.SetSelfPackageId(manifest_->id);
    itemResource->lightSource.SetSelfPackageId(manifest_->id);
    for (auto &tag: itemResource->tags) {
        tag.MakeCachedTag();
    }
    abilityItemRegistry->Register(std::move(itemResource));
}

void glimmer::ResourceFileLoader::LoadMaterialItemResourceResourceFromFile(const toml::value &value,
                                                                           MaterialItemRegistry *materialItemRegistry)
const {
    auto itemResource = std::make_unique<MaterialItemResource>(toml::get<MaterialItemResource>(value));
    itemResource->packId = manifest_->id;
    itemResource->name.SetSelfPackageId(manifest_->id);
    itemResource->description.SetSelfPackageId(manifest_->id);
    itemResource->texture.SetSelfPackageId(manifest_->id);
    itemResource->pipeline.SetSelfPackageId(manifest_->id);
    itemResource->sampler.SetSelfPackageId(manifest_->id);
    itemResource->lightSource.SetSelfPackageId(manifest_->id);
    for (auto &tag: itemResource->tags) {
        tag.MakeCachedTag();
    }
    materialItemRegistry->Register(std::move(itemResource));
}

void glimmer::ResourceFileLoader::LoadContributorResourceFromFile(const toml::value &value,
                                                                  ContributorManager *contributorManager) const {
    auto contributorResource = std::make_unique<Contributor>(toml::get<Contributor>(value));
    contributorResource->displayName.SetSelfPackageId(manifest_->id);
    contributorManager->Register(std::move(contributorResource));
}

void glimmer::ResourceFileLoader::LoadMobResourceFromFile(const toml::value &value, MobRegistry *mobRegistry) const {
    auto mobResource = std::make_unique<MobResource>(toml::get<MobResource>(value));
    mobResource->packId = manifest_->id;
    mobResource->shape.SetSelfPackageId(manifest_->id);
    mobResource->texture.SetSelfPackageId(manifest_->id);
    mobResource->pipeline.SetSelfPackageId(manifest_->id);
    mobResource->sampler.SetSelfPackageId(manifest_->id);
    ItemMessageResource &emptyHandAutoUseItem = mobResource->emptyHandAutoUseItem;
    emptyHandAutoUseItem.item.SetSelfPackageId(manifest_->id);
    for (auto &abilityItemRef: emptyHandAutoUseItem.abilityItemRef) {
        abilityItemRef.item.SetSelfPackageId(manifest_->id);
    }
    mobRegistry->Register(std::move(mobResource));
}

void glimmer::ResourceFileLoader::LoadShapeResourceFromFile(const toml::value &value, ShapeManager *shapeManager,
                                                            ShapeType type) const {
    std::unique_ptr<IShapeResource> shapeResource;
    switch (type) {
        case CIRCLE: {
            shapeResource = std::make_unique<CircularShapeResource>(
                toml::get<CircularShapeResource>(value));
            shapeResource->shapeType = std::to_underlying(CIRCLE);
            break;
        }

        case RECTANGLE: {
            shapeResource = std::make_unique<RectangleShapeResource>(
                toml::get<RectangleShapeResource>(value));
            shapeResource->shapeType = std::to_underlying(RECTANGLE);
            break;
        }
        case ROUNDED_RECTANGLE: {
            shapeResource = std::make_unique<RoundedRectangleShapeResource>(
                toml::get<RoundedRectangleShapeResource>(value));
            shapeResource->shapeType = std::to_underlying(ROUNDED_RECTANGLE);
            break;
        }
    }
    shapeResource->packId = manifest_->id;
    shapeManager->Register(std::move(shapeResource));
}

void glimmer::ResourceFileLoader::LoadFixedColorResourceFromFile(const toml::value &value,
                                                                 FixedColorManager *fixedColorManager) const {
    auto fixedColorResource = std::make_unique<FixedColorResource>(toml::get<FixedColorResource>(value));
    fixedColorResource->packId = manifest_->id;
    fixedColorManager->Register(std::move(fixedColorResource));
}

void glimmer::ResourceFileLoader::LoadLightMaskResourceFromFile(const toml::value &value,
                                                                LightMaskManager *lightMaskManager) const {
    auto lightMaskResource = std::make_unique<LightMaskResource>(toml::get<LightMaskResource>(value));
    lightMaskResource->packId = manifest_->id;
    lightMaskResource->lightMaskColor.SetSelfPackageId(manifest_->id);
    lightMaskManager->Register(std::move(lightMaskResource));
}

void glimmer::ResourceFileLoader::LoadLightSourceResourceFromFile(const toml::value &value,
                                                                  LightSourceManager *lightSourceManager) const {
    auto lightSourceResource = std::make_unique<LightSourceResource>(toml::get<LightSourceResource>(value));
    lightSourceResource->packId = manifest_->id;
    lightSourceResource->lightColor.SetSelfPackageId(manifest_->id);
    if (lightSourceResource->lightRadius > CHUNK_SIZE) {
        lightSourceResource->lightRadius = CHUNK_SIZE;
    }
    lightSourceManager->Register(std::move(lightSourceResource));
}

void glimmer::ResourceFileLoader::LoadBiomeDecoratorResourceFromFile(const toml::value &value,
                                                                     BiomeDecoratorRegistry *biomeDecoratorRegistry,
                                                                     const BiomeDecoratorType type) const {
    switch (type) {
        case FILL: {
            auto fillResource = std::make_unique<FillBiomeDecoratorResource>(
                toml::get<FillBiomeDecoratorResource>(value));
            fillResource->packId = manifest_->id;
            fillResource->tile.SetSelfPackageId(manifest_->id);
            fillResource->biomeDecoratorType = std::to_underlying(type);
            biomeDecoratorRegistry->Register(std::move(fillResource));
            break;
        }
        case MINERAL: {
            auto mineralBiomeDecoratorResource = std::make_unique<MineralBiomeDecoratorResource>(
                toml::get<MineralBiomeDecoratorResource>(value));
            mineralBiomeDecoratorResource->packId = manifest_->id;
            mineralBiomeDecoratorResource->ore.SetSelfPackageId(manifest_->id);
            mineralBiomeDecoratorResource->biomeDecoratorType = std::to_underlying(type);
            biomeDecoratorRegistry->Register(std::move(mineralBiomeDecoratorResource));
            break;
        }
        case SURFACE: {
            auto surfaceBiomeDecoratorResource = std::make_unique<SurfaceBiomeDecoratorResource>(
                toml::get<SurfaceBiomeDecoratorResource>(value));
            surfaceBiomeDecoratorResource->packId = manifest_->id;
            surfaceBiomeDecoratorResource->openAirTile.SetSelfPackageId(manifest_->id);
            surfaceBiomeDecoratorResource->underwaterTile.SetSelfPackageId(manifest_->id);
            surfaceBiomeDecoratorResource->biomeDecoratorType = std::to_underlying(type);
            biomeDecoratorRegistry->Register(std::move(surfaceBiomeDecoratorResource));
            break;
        }
    }
}

void glimmer::ResourceFileLoader::LoadStructurePlacementConditionsResourceFromFile(
    const toml::value &value, StructurePlacementConditionsRegistry *structurePlacementConditionsRegistry,
    StructureConditionProcessorType processorType) const {
    switch (processorType) {
        case StructureConditionProcessorType::Biome: {
            auto biomeStructurePlacementConditionsResource = std::make_unique<
                BiomeStructurePlacementConditionsResource>(
                toml::get<BiomeStructurePlacementConditionsResource>(value));
            biomeStructurePlacementConditionsResource->packId = manifest_->id;
            biomeStructurePlacementConditionsResource->processorId = std::to_underlying(processorType);
            for (auto &targetBiome: biomeStructurePlacementConditionsResource->targetBiomes) {
                targetBiome.SetSelfPackageId(manifest_->id);
            }
            biomeStructurePlacementConditionsResource->RefreshCache();
            structurePlacementConditionsRegistry->Register(std::move(biomeStructurePlacementConditionsResource));
            break;
        }
        case StructureConditionProcessorType::None: {
            auto noneStructurePlacementConditionsResource = std::make_unique<
                NoneStructurePlacementConditionsResource>(
                toml::get<NoneStructurePlacementConditionsResource>(value));
            noneStructurePlacementConditionsResource->packId = manifest_->id;
            noneStructurePlacementConditionsResource->processorId = std::to_underlying(processorType);
            structurePlacementConditionsRegistry->Register(std::move(noneStructurePlacementConditionsResource));
            break;
        }
        case StructureConditionProcessorType::Height: {
            auto heightStructureConditionsResource = std::make_unique<HeightStructureConditionsResource>(
                toml::get<HeightStructureConditionsResource>(value));
            heightStructureConditionsResource->packId = manifest_->id;
            heightStructureConditionsResource->processorId = std::to_underlying(processorType);
            structurePlacementConditionsRegistry->Register(std::move(heightStructureConditionsResource));
            break;
        }
        case StructureConditionProcessorType::HorizontalSpacing: {
            auto horizontalSpacingStructureConditionsResource = std::make_unique<
                HorizontalSpacingStructureConditionsResource>(
                toml::get<HorizontalSpacingStructureConditionsResource>(value));
            horizontalSpacingStructureConditionsResource->packId = manifest_->id;
            horizontalSpacingStructureConditionsResource->processorId = std::to_underlying(processorType);
            structurePlacementConditionsRegistry->Register(
                std::move(horizontalSpacingStructureConditionsResource));
            break;
        }
        case StructureConditionProcessorType::Surface: {
            auto surfaceStructurePlacementConditionsResource = std::make_unique<
                SurfaceStructurePlacementConditionsResource>(
                toml::get<SurfaceStructurePlacementConditionsResource>(value));
            surfaceStructurePlacementConditionsResource->packId = manifest_->id;
            surfaceStructurePlacementConditionsResource->processorId = std::to_underlying(processorType);
            structurePlacementConditionsRegistry->Register(
                std::move(surfaceStructurePlacementConditionsResource));
            break;
        }
    }
}

void glimmer::ResourceFileLoader::LoadRecipeResourceFromFile(const toml::value &value,
                                                             RecipeManager *recipeManager) const {
    auto recipeResource = std::make_unique<RecipeResource>(toml::get<RecipeResource>(value));
    recipeResource->packId = manifest_->id;
    ItemMessageResource &output = recipeResource->output;
    output.item.SetSelfPackageId(manifest_->id);
    for (auto &abilityItemRef: output.abilityItemRef) {
        abilityItemRef.item.SetSelfPackageId(manifest_->id);
    }
    for (auto &input: recipeResource->input) {
        input.MakeCachedTag();
    }
    recipeManager->RegisterRecipe(std::move(recipeResource));
}

int glimmer::ResourceFileLoader::LoadLanguageFiles(const std::vector<std::filesystem::path> &defaultLanguageFiles,
                                                   const std::vector<std::filesystem::path> &targetLanguageFiles,
                                                   const ModContext *modContext) const {
    int total = 0;
    const auto &filesToLoad = targetLanguageFiles.empty() ? defaultLanguageFiles : targetLanguageFiles;
    for (const auto &file: filesToLoad) {
        total += LoadStringResourceFromFile(file, modContext->GetStringManager());
    }
    return total;
}

int glimmer::ResourceFileLoader::LoadResourceByType(const std::string &dataType, const std::string &file,
                                                    const std::string &content, const ModContext *modContext,
                                                    const GraphicsContext *graphicsContext) const {
    const std::vector<std::filesystem::path> searchPath = GetActuallyTemplateSearchPath(file);
    if (dataType == DATA_FILE_TYPE_TEMPLATE) {
        return 1;
    }
    std::string data = tomlTemplateExpander_->Expand(searchPath, content, virtualFileSystem_);
    const toml::value value = toml::parse_str(data, tomlVersion_);

    const auto it = handlerMap_.find(dataType);
    if (it == handlerMap_.end()) {
        return 0;
    }
    it->second(value, modContext, graphicsContext);
    return 1;
}
