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
#include "core/inventory/Item.h"
#include "core/log/LogCat.h"
#include "core/utils/StringUtils.h"
#include "dataPack/StringManager.h"
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
        LogCat::w(std::source_location::current(), "data_pack_manager_is_null", "dataPackManager_ == nullptr");
        return false;
    }
    return dataPackManager_->IsDependencySatisfied(
        StringUtils::StringToUint64(resourceRef->GetSelfPackageId()),
        StringUtils::StringToUint64(resourceRef->GetPackageId()));
}

glimmer::ResourceLocator::ResourceLocator(AppContext *appContext) : appContext_(appContext), itemFactory_(appContext,
                                                                        this) {
    if (appContext_ == nullptr) {
        LogCat::e(std::source_location::current(), "app_context_is_null", "appContext_ == nullptr");
        return;
    }
    cacheContext_ = appContext_->GetCacheContext();
    if (cacheContext_ == nullptr) {
        LogCat::e(std::source_location::current(), "cache_context_is_null", "cacheContext_ == nullptr");
        return;
    }
    const GraphicsContext *graphicsContext = appContext_->GetGraphicsContext();
    if (graphicsContext == nullptr) {
        LogCat::e(std::source_location::current(), "graphics_context_is_null", "graphicsContext == nullptr");
        return;
    }
    fixedColorManager_ =
            graphicsContext->GetFixedColorManager();
    if (fixedColorManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "fixed_color_manager_is_null", "fixedColorManager_ == nullptr");
        return;
    }
    lightMaskManager_ = graphicsContext->GetLightMaskManager();
    if (lightMaskManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "light_mask_manager_is_null", "lightMaskManager_ == nullptr");
        return;
    }
    lightSourceManager_ = graphicsContext->GetLightSourceManager();
    if (lightSourceManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "light_source_manager_is_null", "lightSourceManager_ == nullptr");
        return;
    }
    const ModContext *modContext = appContext_->GetModContext();
    if (modContext == nullptr) {
        LogCat::e(std::source_location::current(), "mod_context_is_null", "modContext == nullptr");
        return;
    }
    lootTableRegistry_ = modContext->GetLootTableRegistry();
    if (lootTableRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "loot_table_registry_is_null", "lootTableRegistry_ == nullptr");
        return;
    }
    abilityItemRegistry_ = modContext->GetAbilityItemRegistry();
    if (abilityItemRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "ability_item_manager_is_null", "abilityItemManager_ == nullptr");
        return;
    }
    composableItemRegistry_ = modContext->GetComposableItemRegistry();
    if (composableItemRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "composable_item_manager_is_null",
                  "composableItemManager_ == nullptr");
        return;
    }
    materialItemRegistry_ = modContext->GetMaterialItemRegistry();
    if (materialItemRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "material_item_manager_is_null", "materialItemManager_ == nullptr");
        return;
    }
    mobRegistry_ = modContext->GetMobRegistry();
    if (mobRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "mob_manager_is_null", "mobManager_ == nullptr");
        return;
    }
    tileResourceManager_ = modContext->GetTileResourceManager();
    if (tileResourceManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "tile_resource_manager_is_null", "tileResourceManager_ == nullptr");
        return;
    }
    shapeManager_ = modContext->GetShapeManager();
    if (shapeManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "shape_manager_is_null", "shapeManager_ == nullptr");
        return;
    }
    stringManager_ = modContext->GetStringManager();
    if (stringManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "string_manager_is_null", "stringManager_ == nullptr");
        return;
    }
    biomeDecoratorRegistry_ = modContext->GetBiomeDecoratorRegistry();
    if (biomeDecoratorRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "biome_decorator_resources_manager_is_null",
                  "biomeDecoratorResourcesManager_ == nullptr");
        return;
    }
    dataPackManager_ = modContext->GetDataPackManager();
    if (dataPackManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "data_pack_manager_is_null", "dataPackManager_ == nullptr");
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
        LogCat::w(std::source_location::current(), "resource_ref_null_find_texture_raw",
                  "resourceRef == nullptr in FindTextureRaw");
        return textureCache->TryGetPlaceholder(appContext_, resourceRef, enablePlaceHolder);
    }
    if (resourceRef->GetResourceType() != RESOURCE_TEXTURE) {
        LogCat::w(std::source_location::current(), "type_mismatch_texture",
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
        LogCat::w(std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return audioCache->TryGetPlaceholder(appContext_, resourceRef, enablePlaceholder);
    }
    if (resourceRef->GetResourceType() != RESOURCE_AUDIO) {
        LogCat::w(std::source_location::current(), "type_mismatch_audio",
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
        LogCat::w(std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return nullptr;
    }
    const bool supportType = resourceRef->GetResourceType() == RESOURCE_SHADER_VERTEX || resourceRef->GetResourceType()
                             ==
                             RESOURCE_SHADER_FRAGMENT;
    if (!supportType) {
        LogCat::w(std::source_location::current(), "type_mismatch_shader",
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
        LogCat::w(std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_PIPELINE) {
        LogCat::w(std::source_location::current(), "type_mismatch_pipeline",
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
        LogCat::w(std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_SAMPLER) {
        LogCat::w(std::source_location::current(), "type_mismatch_sampler",
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
        LogCat::w(std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_UNIFORM_BLOCK) {
        LogCat::w(std::source_location::current(), "type_mismatch_uniform_block",
                  "Type mismatch: expected RESOURCE_UNIFORM_BLOCK ({}), got {}).",
                  std::to_underlying(RESOURCE_UNIFORM_BLOCK), std::to_underlying(resourceRef->GetResourceType()));
        return nullptr;
    }
    return uniformBlockCache->LoadResource(appContext_, resourceRef, enablePlaceHolder);
}

std::unique_ptr<glimmer::Color> glimmer::ResourceLocator::FindColor(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return nullptr;
    }
    const ResourceTypeMessage resourceType = resourceRef->GetResourceType();
    if (resourceType == RESOURCE_COLOR) {
        if (cacheContext_ == nullptr) {
            return nullptr;
        }
        ColorCache *colorCache = cacheContext_->GetColorCache();
        if (colorCache == nullptr) {
            return nullptr;
        }
        const std::shared_ptr<ColorResource> colorResource = colorCache->LoadResource(
            appContext_, resourceRef);
        if (colorResource == nullptr) {
            LogCat::w(std::source_location::current(), "color_resource_load_failed",
                      "Failed to load color resource: packageId={}, resourceKey={}", resourceRef->GetPackageId(),
                      resourceRef->GetResourceKey());
            return nullptr;
        }
        return std::make_unique<Color>(colorResource->ToColor());
    }
    if (resourceType == RESOURCE_FIXED_COLOR) {
        if (!ValidateAccessPermission(resourceRef)) {
            LogCat::w(std::source_location::current(), "color_resource_access_denied",
                      "Access permission denied for color resource: packageId={}, resourceKey={}",
                      resourceRef->GetPackageId(), resourceRef->GetResourceKey());
            return nullptr;
        }
        const FixedColorResource *fixedColorResource = fixedColorManager_->FindFixedColorResource(
            resourceRef->GetPackageId(),
            resourceRef->GetResourceKey());
        if (fixedColorResource == nullptr) {
            LogCat::w(std::source_location::current(), "fixed_color_resource_not_found",
                      "Failed to find fixed color resource: packageId={}, resourceKey={}", resourceRef->GetPackageId(),
                      resourceRef->GetResourceKey());
            return nullptr;
        }
        return std::make_unique<Color>(fixedColorResource->ToColor());
    }
    LogCat::w(std::source_location::current(), "unsupported_color_resource_type",
              "Unsupported color resource type: packageId={}, resourceKey={}, type={}", resourceRef->GetPackageId(),
              resourceRef->GetResourceKey(), std::to_underlying(resourceType));
    return nullptr;
}

glimmer::IShapeResource *glimmer::ResourceLocator::FindShape(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::e(std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_SHAPE) {
        LogCat::e(std::source_location::current(), "type_mismatch_shape",
                  "Type mismatch: expected RESOURCE_SHAPE ({}), got {} or access permission denied",
                  std::to_underlying(RESOURCE_SHAPE), std::to_underlying(resourceRef->GetResourceType()));
        return nullptr;
    }
    if (shapeManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "shape_manager_is_null", "shapeManager_ == nullptr");
        return nullptr;
    }
    return shapeManager_->FindShape(resourceRef->GetPackageId(),
                                    resourceRef->GetResourceKey());
}

glimmer::IBiomeDecoratorResource *glimmer::ResourceLocator::FindBiomeDecorator(const ResourceRef *resourceRef) const {
    return FindRegistered<IBiomeDecoratorResource>(resourceRef, RESOURCE_BIOME_DECORATOR,
                                                   [this, resourceRef]() -> IBiomeDecoratorResource * {
                                                       if (biomeDecoratorRegistry_ == nullptr) {
                                                           LogCat::w(std::source_location::current(),
                                                                     "biome_decorator_resources_manager_is_null",
                                                                     "biomeDecoratorResourcesManager_ == nullptr");
                                                           return nullptr;
                                                       }
                                                       return biomeDecoratorRegistry_->Find(
                                                           resourceRef->GetPackageId(), resourceRef->GetResourceKey());
                                                   });
}

glimmer::StringResource *glimmer::ResourceLocator::FindString(const ResourceRef *resourceRef) const {
    return FindRegistered<StringResource>(resourceRef, RESOURCE_STRING, [this, resourceRef]() -> StringResource * {
        if (stringManager_ == nullptr) {
            LogCat::w(std::source_location::current(), "string_manager_is_null", "stringManager_ == nullptr");
            return nullptr;
        }
        return stringManager_->Find(resourceRef->GetPackageId(),
                                    resourceRef->GetResourceKey());
    });
}

glimmer::LightSourceResource *glimmer::ResourceLocator::FindLightSource(const ResourceRef *resourceRef) const {
    return FindRegistered<LightSourceResource>(resourceRef, RESOURCE_LIGHT_SOURCE,
                                               [this, resourceRef]() -> LightSourceResource * {
                                                   if (lightSourceManager_ == nullptr) {
                                                       LogCat::w(std::source_location::current(),
                                                                 "light_source_manager_is_null",
                                                                 "lightSourceManager_ == nullptr");
                                                       return nullptr;
                                                   }
                                                   return lightSourceManager_->FindLightSourceResource(
                                                       resourceRef->GetPackageId(),
                                                       resourceRef->GetResourceKey());
                                               });
}

glimmer::LightMaskResource *glimmer::ResourceLocator::FindLightMask(const ResourceRef *resourceRef) const {
    return FindRegistered<LightMaskResource>(resourceRef, RESOURCE_LIGHT_MASK,
                                             [this, resourceRef]() -> LightMaskResource * {
                                                 if (lightMaskManager_ == nullptr) {
                                                     LogCat::w(std::source_location::current(),
                                                               "light_mask_manager_is_null",
                                                               "lightMaskManager_ == nullptr");
                                                     return nullptr;
                                                 }
                                                 return lightMaskManager_->FindLightMaskResource(
                                                     resourceRef->GetPackageId(),
                                                     resourceRef->GetResourceKey());
                                             });
}

glimmer::TileResource *glimmer::ResourceLocator::FindTileFallback(const ResourceRef *resourceRef,
                                                                  TileLayerType tileLayer) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resource_ref_is_null", "resourceRef == nullptr");
        return nullptr;
    }
    if (tileResourceManager_ == nullptr) {
        LogCat::w(std::source_location::current(), "tile_resource_is_null", "tileResource == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_TILE || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "type_mismatch_tile",
                  "Type mismatch: expected RESOURCE_TILE ({}), got {} or access permission denied",
                  std::to_underlying(RESOURCE_TILE), std::to_underlying(resourceRef->GetResourceType()));
        return tileResourceManager_->GenerateAccessDeniedPlaceHolder(
            resourceRef->GetPackageId(), resourceRef->GetResourceKey(), tileLayer);
    }
    return tileResourceManager_->FindTileFallback(resourceRef->GetPackageId(),
                                                  resourceRef->GetResourceKey(),
                                                  tileLayer);
}

glimmer::TileResource *glimmer::ResourceLocator::FindTileRaw(const ResourceRef *resourceRef) const {
    return FindRegistered<TileResource>(resourceRef, RESOURCE_TILE, [this, resourceRef]() -> TileResource * {
        if (tileResourceManager_ == nullptr) {
            LogCat::w(std::source_location::current(), "tile_resource_is_null", "tileResource == nullptr");
            return nullptr;
        }
        return tileResourceManager_->FindTileRaw(resourceRef->GetPackageId(),
                                                 resourceRef->GetResourceKey());
    });
}

glimmer::MobResource *glimmer::ResourceLocator::FindMob(const ResourceRef *resourceRef) const {
    return FindRegistered<MobResource>(resourceRef, RESOURCE_MOB, [this, resourceRef]() -> MobResource * {
        if (mobRegistry_ == nullptr) {
            LogCat::w(std::source_location::current(), "mob_manager_is_null", "mobManager == nullptr");
            return nullptr;
        }
        return mobRegistry_->Find(resourceRef->GetPackageId(),
                                  resourceRef->GetResourceKey());
    });
}

glimmer::ComposableItemResource *glimmer::ResourceLocator::FindComposableItem(
    const ResourceRef *resourceRef) const {
    return FindRegistered<ComposableItemResource>(resourceRef, RESOURCE_COMPOSABLE_ITEM,
                                                  [this, resourceRef]() -> ComposableItemResource * {
                                                      if (composableItemRegistry_ == nullptr) {
                                                          LogCat::w(std::source_location::current(),
                                                                    "item_manager_is_null", "itemManager == nullptr");
                                                          return nullptr;
                                                      }
                                                      return composableItemRegistry_->Find(resourceRef->GetPackageId(),
                                                          resourceRef->GetResourceKey());
                                                  });
}

glimmer::AbilityItemResource *glimmer::ResourceLocator::FindAbilityItem(
    const ResourceRef *resourceRef) const {
    return FindRegistered<AbilityItemResource>(resourceRef, RESOURCE_ABILITY_ITEM,
                                               [this, resourceRef]() -> AbilityItemResource * {
                                                   if (abilityItemRegistry_ == nullptr) {
                                                       LogCat::w(std::source_location::current(),
                                                                 "item_manager_is_null", "itemManager == nullptr");
                                                       return nullptr;
                                                   }
                                                   return abilityItemRegistry_->Find(resourceRef->GetPackageId(),
                                                       resourceRef->GetResourceKey());
                                               });
}

glimmer::MaterialItemResource *glimmer::ResourceLocator::FindMaterialItem(const ResourceRef *resourceRef) const {
    return FindRegistered<MaterialItemResource>(resourceRef, RESOURCE_MATERIAL_ITEM,
                                                [this, resourceRef]() -> MaterialItemResource * {
                                                    if (materialItemRegistry_ == nullptr) {
                                                        LogCat::w(std::source_location::current(),
                                                                  "item_manager_is_null", "itemManager == nullptr");
                                                        return nullptr;
                                                    }
                                                    return materialItemRegistry_->Find(resourceRef->GetPackageId(),
                                                        resourceRef->GetResourceKey());
                                                });
}

glimmer::LootResource *glimmer::ResourceLocator::FindLoot(const ResourceRef *resourceRef) const {
    return FindRegistered<LootResource>(resourceRef, RESOURCE_LOOT_TABLE, [this, resourceRef]() -> LootResource * {
        if (lootTableRegistry_ == nullptr) {
            LogCat::w(std::source_location::current(), "loot_table_manager_is_null", "lootTableManager_ == nullptr");
            return nullptr;
        }
        return lootTableRegistry_->Find(resourceRef->GetPackageId(),
                                        resourceRef->GetResourceKey());
    });
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
