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
#include "ResourceLocator.h"

#include "core/context/AppContext.h"
#include "core/context/CacheContext.h"
#include "core/context/ModContext.h"
#include "core/inventory/Item.h"
#include "core/log/LogCat.h"
#include "core/utils/StringUtils.h"
#include "dataPack/StringResourceRegistry.h"
#include "resourcePack/cache/AudioCache.h"
#include "resourcePack/cache/ColorCache.h"
#include "resourcePack/cache/GpuPipelineCache.h"
#include "resourcePack/cache/ShaderCache.h"
#include "resourcePack/cache/TextureCache.h"

bool glimmer::ResourceLocator::ValidateAccessPermission(const ResourceRef *resourceRef) const {
    if (resourceRef->GetSelfPackageId() == resourceRef->GetPackageId()) {
        //Allow access to one's own package.
        //允许访问自身包。
        return true;
    }
    if (dataPackManager_ == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "data_pack_manager_is_null",
                  "dataPackManager_ == nullptr");
        return false;
    }
    return dataPackManager_->IsDependencySatisfied(
        StringUtils::StringToUint64(resourceRef->GetSelfPackageId()),
        StringUtils::StringToUint64(resourceRef->GetPackageId()));
}

glimmer::ResourceLocator::ResourceLocator(AppContext *appContext) : appContext_(appContext), itemFactory_(appContext,
                                                                        this) {
    if (appContext_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "app_context_is_null", "appContext_ == nullptr");
        return;
    }
    cacheContext_ = appContext_->GetCacheContext();
    if (cacheContext_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "cache_context_is_null",
                  "cacheContext_ == nullptr");
        return;
    }
    const GraphicsContext *graphicsContext = appContext_->GetGraphicsContext();
    if (graphicsContext == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "graphics_context_is_null",
                  "graphicsContext == nullptr");
        return;
    }
    fixedColorManager_ =
            graphicsContext->GetFixedColorManager();
    if (fixedColorManager_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "fixed_color_manager_is_null",
                  "fixedColorManager_ == nullptr");
        return;
    }
    lightMaskResourceRegistry_ = graphicsContext->GetLightMaskResourceRegistry();
    if (lightMaskResourceRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "light_mask_manager_is_null",
                  "lightMaskManager_ == nullptr");
        return;
    }
    lightSourceResourceRegistry_ = graphicsContext->GetLightSourceResourceRegistry();
    if (lightSourceResourceRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "light_source_manager_is_null",
                  "lightSourceManager_ == nullptr");
        return;
    }
    const ModContext *modContext = appContext_->GetModContext();
    if (modContext == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "mod_context_is_null", "modContext == nullptr");
        return;
    }
    lootTableRegistry_ = modContext->GetLootTableRegistry();
    if (lootTableRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "loot_table_registry_is_null",
                  "lootTableRegistry_ == nullptr");
        return;
    }
    abilityItemRegistry_ = modContext->GetAbilityItemRegistry();
    if (abilityItemRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "ability_item_manager_is_null",
                  "abilityItemManager_ == nullptr");
        return;
    }
    composableItemRegistry_ = modContext->GetComposableItemRegistry();
    if (composableItemRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "composable_item_manager_is_null",
                  "composableItemManager_ == nullptr");
        return;
    }
    materialItemRegistry_ = modContext->GetMaterialItemRegistry();
    if (materialItemRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "material_item_manager_is_null",
                  "materialItemManager_ == nullptr");
        return;
    }
    mobRegistry_ = modContext->GetMobRegistry();
    if (mobRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "mob_manager_is_null", "mobManager_ == nullptr");
        return;
    }
    structureRegistry_ = modContext->GetStructureRegistry();
    if (structureRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "structure_registry_is_null",
                  "structureRegistry_ == nullptr");
        return;
    }
    dimensionRegistry_ = modContext->GetDimensionRegistry();
    if (dimensionRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "dimension_registry_is_null",
                  "dimensionRegistry_ == nullptr");
        return;
    }
    structurePlacementConditionsRegistry_ = modContext->GetStructurePlacementConditionsRegistry();
    if (structurePlacementConditionsRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(),
                  "structure_placement_conditions_registry_is_null",
                  "structurePlacementConditionsRegistry_ == nullptr");
        return;
    }
    tileResourceRegistry_ = modContext->GetTileResourceRegistry();
    if (tileResourceRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "tile_resource_manager_is_null",
                  "tileResourceManager_ == nullptr");
        return;
    }
    stringManager_ = modContext->GetStringResourceRegistry();
    if (stringManager_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "string_manager_is_null",
                  "stringManager_ == nullptr");
        return;
    }
    biomeDecoratorRegistry_ = modContext->GetBiomeDecoratorRegistry();
    if (biomeDecoratorRegistry_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "biome_decorator_resources_manager_is_null",
                  "biomeDecoratorResourcesManager_ == nullptr");
        return;
    }
    dataPackManager_ = modContext->GetDataPackManager();
    if (dataPackManager_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "data_pack_manager_is_null",
                  "dataPackManager_ == nullptr");
        return;
    }
    resourcePackManager_ = appContext_->GetResourcePackManager();
}

std::shared_ptr<glimmer::TextureResourceResult> glimmer::ResourceLocator::FindTexture(const ResourceRef *resourceRef,
    bool enablePlaceHolder) const {
    if (cacheContext_ == nullptr) {
        return nullptr;
    }
    TextureCache *textureCache = cacheContext_->GetTextureCache();
    if (textureCache == nullptr) {
        return nullptr;
    }
    if (resourceRef == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "resource_ref_null_find_texture_raw",
                  "resourceRef == nullptr in FindTextureRaw");
        return textureCache->TryGetPlaceholder(appContext_, resourceRef, enablePlaceHolder);
    }
    if (resourceRef->GetResourceType() != RESOURCE_TEXTURE) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "type_mismatch_texture",
                  "Type mismatch: expected RESOURCE_TEXTURE ({}), got {}).", std::to_underlying(RESOURCE_TEXTURE),
                  std::to_underlying(resourceRef->GetResourceType()));
        return textureCache->TryGetPlaceholder(appContext_, resourceRef, enablePlaceHolder);
    }
    return textureCache->LoadResource(appContext_, resourceRef, enablePlaceHolder);
}

std::shared_ptr<glimmer::AudioResourceResult> glimmer::ResourceLocator::FindAudio(const ResourceRef *resourceRef,
    bool enablePlaceholder) const {
    if (cacheContext_ == nullptr) {
        return nullptr;
    }
    AudioCache *audioCache = cacheContext_->GetAudioCache();
    if (audioCache == nullptr) {
        return nullptr;
    }
    if (resourceRef == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return audioCache->TryGetPlaceholder(appContext_, resourceRef, enablePlaceholder);
    }
    if (resourceRef->GetResourceType() != RESOURCE_AUDIO) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "type_mismatch_audio",
                  "Type mismatch: expected RESOURCE_AUDIO ({}), got {}).", std::to_underlying(RESOURCE_AUDIO),
                  std::to_underlying(resourceRef->GetResourceType()));
        return audioCache->TryGetPlaceholder(appContext_, resourceRef, enablePlaceholder);
    }
    return audioCache->LoadResource(appContext_, resourceRef, enablePlaceholder);
}

std::shared_ptr<glimmer::ShaderResourceResult> glimmer::ResourceLocator::FindShader(const ResourceRef *resourceRef,
    bool enablePlaceholder) const {
    if (cacheContext_ == nullptr) {
        return nullptr;
    }
    ShaderCache *shaderCache = cacheContext_->GetShaderCache();
    if (shaderCache == nullptr) {
        return nullptr;
    }
    if (resourceRef == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return nullptr;
    }
    const bool supportType = resourceRef->GetResourceType() == RESOURCE_SHADER_VERTEX || resourceRef->GetResourceType()
                             ==
                             RESOURCE_SHADER_FRAGMENT;
    if (!supportType) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "type_mismatch_shader",
                  "Type mismatch: expected RESOURCE_SHADER ({}or{}).", std::to_underlying(RESOURCE_SHADER_VERTEX),
                  std::to_underlying(RESOURCE_SHADER_FRAGMENT));
        return nullptr;
    }
    return shaderCache->LoadResource(appContext_, resourceRef, enablePlaceholder);
}

std::shared_ptr<glimmer::GPUPipelineResourceResult> glimmer::ResourceLocator::FindGPUGraphicsPipeline(
    const ResourceRef *resourceRef, const bool enablePlaceHolder) const {
    if (cacheContext_ == nullptr) {
        return nullptr;
    }
    GpuPipelineCache *gpuPipelineCache = cacheContext_->GetPipelineCache();
    if (gpuPipelineCache == nullptr) {
        return nullptr;
    }
    if (resourceRef == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_PIPELINE) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "type_mismatch_pipeline",
                  "Type mismatch: expected RESOURCE_PIPELINE ({}), got {}).", std::to_underlying(RESOURCE_PIPELINE),
                  std::to_underlying(resourceRef->GetResourceType()));
        return nullptr;
    }
    return gpuPipelineCache->LoadResource(appContext_, resourceRef, enablePlaceHolder);
}

std::shared_ptr<glimmer::GPUSamplerResourceResult> glimmer::ResourceLocator::FindGPUGraphicsSampler(
    const ResourceRef *resourceRef, bool enablePlaceHolder) const {
    if (cacheContext_ == nullptr) {
        return nullptr;
    }
    GpuSamplerCache *gpuSamplerCache = cacheContext_->GetGpuSamplerCache();
    if (gpuSamplerCache == nullptr) {
        return nullptr;
    }
    if (resourceRef == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_SAMPLER) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "type_mismatch_sampler",
                  "Type mismatch: expected RESOURCE_SAMPLER ({}), got {}).", std::to_underlying(RESOURCE_SAMPLER),
                  std::to_underlying(resourceRef->GetResourceType()));
        return nullptr;
    }
    return gpuSamplerCache->LoadResource(appContext_, resourceRef, enablePlaceHolder);
}

std::shared_ptr<glimmer::UniformBlockResourceResult> glimmer::ResourceLocator::FindUniformBlock(
    const ResourceRef *resourceRef, bool enablePlaceHolder) const {
    if (cacheContext_ == nullptr) {
        return nullptr;
    }
    UniformBlockCache *uniformBlockCache = cacheContext_->GetUniformBlockCache();
    if (uniformBlockCache == nullptr) {
        return nullptr;
    }
    if (resourceRef == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_UNIFORM_BLOCK) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "type_mismatch_uniform_block",
                  "Type mismatch: expected RESOURCE_UNIFORM_BLOCK ({}), got {}).",
                  std::to_underlying(RESOURCE_UNIFORM_BLOCK), std::to_underlying(resourceRef->GetResourceType()));
        return nullptr;
    }
    return uniformBlockCache->LoadResource(appContext_, resourceRef, enablePlaceHolder);
}

std::optional<glimmer::Color> glimmer::ResourceLocator::FindColor(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return std::nullopt;
    }
    const ResourceTypeMessage resourceType = resourceRef->GetResourceType();
    if (resourceType == RESOURCE_COLOR) {
        if (cacheContext_ == nullptr) {
            return std::nullopt;
        }
        ColorCache *colorCache = cacheContext_->GetColorCache();
        if (colorCache == nullptr) {
            return std::nullopt;
        }
        const std::shared_ptr<ColorResource> colorResource = colorCache->LoadResource(
            appContext_, resourceRef);
        if (colorResource == nullptr) {
            LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "color_resource_load_failed",
                      "Failed to load color resource: packageId={}, resourceKey={}", resourceRef->GetPackageId(),
                      resourceRef->GetResourceKey());
            return std::nullopt;
        }
        return colorResource->ToColor();
    }
    if (resourceType == RESOURCE_FIXED_COLOR) {
        if (!ValidateAccessPermission(resourceRef)) {
            LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "color_resource_access_denied",
                      "Access permission denied for color resource: packageId={}, resourceKey={}",
                      resourceRef->GetPackageId(), resourceRef->GetResourceKey());
            return std::nullopt;
        }
        const FixedColorResource *fixedColorResource = fixedColorManager_->FindFixedColorResource(
            resourceRef->GetPackageId(),
            resourceRef->GetResourceKey());
        if (fixedColorResource == nullptr) {
            LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "fixed_color_resource_not_found",
                      "Failed to find fixed color resource: packageId={}, resourceKey={}", resourceRef->GetPackageId(),
                      resourceRef->GetResourceKey());
            return std::nullopt;
        }
        return fixedColorResource->ToColor();
    }
    LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "unsupported_color_resource_type",
              "Unsupported color resource type: packageId={}, resourceKey={}, type={}", resourceRef->GetPackageId(),
              resourceRef->GetResourceKey(), std::to_underlying(resourceType));
    return std::nullopt;
}


glimmer::IBiomeDecoratorResource *glimmer::ResourceLocator::FindBiomeDecorator(const ResourceRef *resourceRef) const {
    return FindRegistered<IBiomeDecoratorResource>(biomeDecoratorRegistry_, resourceRef, RESOURCE_BIOME_DECORATOR);
}

glimmer::StringResource *glimmer::ResourceLocator::FindString(const ResourceRef *resourceRef) const {
    return FindRegistered<StringResource>(stringManager_, resourceRef, RESOURCE_STRING);
}

glimmer::LightSourceResource *glimmer::ResourceLocator::FindLightSource(const ResourceRef *resourceRef) const {
    return FindRegistered<LightSourceResource>(lightSourceResourceRegistry_, resourceRef, RESOURCE_LIGHT_SOURCE);
}

glimmer::LightMaskResource *glimmer::ResourceLocator::FindLightMask(const ResourceRef *resourceRef) const {
    return FindRegistered<LightMaskResource>(lightMaskResourceRegistry_, resourceRef, RESOURCE_LIGHT_MASK);
}

glimmer::TileResource *glimmer::ResourceLocator::FindTile(const ResourceRef *resourceRef) const {
    return FindRegistered<TileResource>(tileResourceRegistry_, resourceRef, RESOURCE_TILE);
}

glimmer::MobResource *glimmer::ResourceLocator::FindMob(const ResourceRef *resourceRef) const {
    return FindRegistered<MobResource>(mobRegistry_, resourceRef, RESOURCE_MOB);
}

glimmer::IStructureResource *glimmer::ResourceLocator::FindStructure(const ResourceRef *resourceRef) const {
    return FindRegistered<IStructureResource>(structureRegistry_, resourceRef, RESOURCE_STRUCTURE);
}

glimmer::DimensionResource *glimmer::ResourceLocator::FindDimension(const ResourceRef *resourceRef) const {
    return FindRegistered<DimensionResource>(dimensionRegistry_, resourceRef, RESOURCE_DIMENSION);
}

glimmer::IStructurePlacementConditionsResource *glimmer::ResourceLocator::FindStructurePlacementConditions(
    const ResourceRef *resourceRef) const {
    return FindRegistered<IStructurePlacementConditionsResource>(structurePlacementConditionsRegistry_, resourceRef,
                                                                 RESOURCE_STRUCTURE_PLACEMENT_CONDITIONS);
}

glimmer::ComposableItemResource *glimmer::ResourceLocator::FindComposableItem(
    const ResourceRef *resourceRef) const {
    return FindRegistered<ComposableItemResource>(composableItemRegistry_, resourceRef, RESOURCE_COMPOSABLE_ITEM);
}

glimmer::AbilityItemResource *glimmer::ResourceLocator::FindAbilityItem(
    const ResourceRef *resourceRef) const {
    return FindRegistered<AbilityItemResource>(abilityItemRegistry_, resourceRef,
                                               RESOURCE_ABILITY_ITEM);
}

glimmer::MaterialItemResource *glimmer::ResourceLocator::FindMaterialItem(const ResourceRef *resourceRef) const {
    return FindRegistered<MaterialItemResource>(materialItemRegistry_, resourceRef, RESOURCE_MATERIAL_ITEM);
}

glimmer::MaterialResource *glimmer::ResourceLocator::FindMaterial(const ResourceRef *resourceRef) const {
    return FindRegistered<MaterialResource>(materialResourceRegistry_, resourceRef, RESOURCE_MATERIAL);
}

glimmer::LootResource *glimmer::ResourceLocator::FindLoot(const ResourceRef *resourceRef) const {
    return FindRegistered<LootResource>(lootTableRegistry_, resourceRef, RESOURCE_LOOT_TABLE);
}

std::unique_ptr<glimmer::Item> glimmer::ResourceLocator::FindItem(WorldContext *worldContext,
                                                                  const ItemMessage &itemMessage) const {
    return itemFactory_.CreateItem(worldContext, itemMessage);
}

std::unique_ptr<glimmer::Item> glimmer::ResourceLocator::FindItem(WorldContext *worldContext,
                                                                  const ItemMessageResource &itemMessageResource)
const {
    return itemFactory_.CreateItem(worldContext, itemMessageResource);
}
