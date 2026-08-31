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
#include "core/inventory/AbilityItem.h"
#include "core/inventory/ComposableItem.h"
#include "core/inventory/MaterialItem.h"
#include "core/inventory/TileItem.h"
#include "core/log/LogCat.h"
#include "core/utils/RandomUtils.h"
#include "core/world/WorldContext.h"
#include "core/world/TileInstancePool.h"
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
        LogCat::w(std::source_location::current(), "dataPackManager_ == nullptr");
        return false;
    }
    return dataPackManager_->IsDependencySatisfied(
        resourceRef->GetSelfPackageId(), resourceRef->GetPackageId());
}

glimmer::ResourceLocator::ResourceLocator(AppContext *appContext) : appContext_(appContext) {
    if (appContext_ == nullptr) {
        LogCat::e(std::source_location::current(), "appContext_ == nullptr");
        return;
    }
    cacheContext_ = appContext_->GetCacheContext();
    if (cacheContext_ == nullptr) {
        LogCat::e(std::source_location::current(), "cacheContext_ == nullptr");
        return;
    }
    const GraphicsContext *graphicsContext = appContext_->GetGraphicsContext();
    if (graphicsContext == nullptr) {
        LogCat::e(std::source_location::current(), "graphicsContext == nullptr");
        return;
    }
    fixedColorManager_ =
            graphicsContext->GetFixedColorManager();
    if (fixedColorManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "fixedColorManager_ == nullptr");
        return;
    }
    lightMaskManager_ = graphicsContext->GetLightMaskManager();
    if (lightMaskManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "lightMaskManager_ == nullptr");
        return;
    }
    lightSourceManager_ = graphicsContext->GetLightSourceManager();
    if (lightSourceManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "lightSourceManager_ == nullptr");
        return;
    }
    const ModContext *modContext = appContext_->GetModContext();
    if (modContext == nullptr) {
        LogCat::e(std::source_location::current(), "modContext == nullptr");
        return;
    }
    lootTableRegistry_ = modContext->GetLootTableRegistry();
    if (lootTableRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "lootTableRegistry_ == nullptr");
        return;
    }
    abilityItemRegistry_ = modContext->GetAbilityItemRegistry();
    if (abilityItemRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "abilityItemManager_ == nullptr");
        return;
    }
    composableItemRegistry_ = modContext->GetComposableItemRegistry();
    if (composableItemRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "composableItemManager_ == nullptr");
        return;
    }
    materialItemRegistry_ = modContext->GetMaterialItemRegistry();
    if (materialItemRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "materialItemManager_ == nullptr");
        return;
    }
    mobRegistry_ = modContext->GetMobRegistry();
    if (mobRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "mobManager_ == nullptr");
        return;
    }
    tileResourceManager_ = modContext->GetTileResourceManager();
    if (tileResourceManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "tileResourceManager_ == nullptr");
        return;
    }
    shapeManager_ = modContext->GetShapeManager();
    if (shapeManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "shapeManager_ == nullptr");
        return;
    }
    stringManager_ = modContext->GetStringManager();
    if (stringManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "stringManager_ == nullptr");
        return;
    }
    biomeDecoratorRegistry_ = modContext->GetBiomeDecoratorRegistry();
    if (biomeDecoratorRegistry_ == nullptr) {
        LogCat::e(std::source_location::current(), "biomeDecoratorResourcesManager_ == nullptr");
        return;
    }
    dataPackManager_ = modContext->GetDataPackManager();
    if (dataPackManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "dataPackManager_ == nullptr");
        return;
    }
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
        LogCat::w(std::source_location::current(), "resourceRef == nullptr in FindTextureRaw");
        return textureCache->TryGetPlaceholder(appContext_, resourceRef, enablePlaceHolder);
    }
    if (resourceRef->GetResourceType() != RESOURCE_TEXTURE) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_TEXTURE (",
                  std::to_underlying(RESOURCE_TEXTURE), "), got ", std::to_underlying(resourceRef->GetResourceType()),
                  " or access permission denied");
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
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return audioCache->TryGetPlaceholder(appContext_, resourceRef, enablePlaceholder);
    }
    if (resourceRef->GetResourceType() != RESOURCE_AUDIO || !
        ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_AUDIO (",
                  std::to_underlying(RESOURCE_AUDIO), "), got ", std::to_underlying(resourceRef->GetResourceType()),
                  " or access permission denied");
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
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    const bool supportType = resourceRef->GetResourceType() == RESOURCE_SHADER_VERTEX || resourceRef->GetResourceType()
                             ==
                             RESOURCE_SHADER_FRAGMENT;
    if (!supportType || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_SHADER (",
                  std::to_underlying(RESOURCE_SHADER_VERTEX), "or", std::to_underlying(RESOURCE_SHADER_FRAGMENT),
                  "), got ", std::to_underlying(resourceRef->GetResourceType()),
                  " or access permission denied");
        return nullptr;
    }
    return shaderCache->LoadResource(appContext_, resourceRef, enablePlaceholder);
}

std::shared_ptr<glimmer::GPUPipelineResourceResult> glimmer::ResourceLocator::FindPipeline(
    const ResourceRef *resourceRef, bool enablePlaceHolder) const {
    if (cacheContext_ == nullptr) {
        return nullptr;
    }
    GpuPipelineCache *gpuPipelineCache = cacheContext_->GetPipelineCache();
    if (gpuPipelineCache == nullptr) {
        return nullptr;
    }
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_PIPELINE || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_PIPELINE (",
                  std::to_underlying(RESOURCE_PIPELINE), "), got ", std::to_underlying(resourceRef->GetResourceType()),
                  " or access permission denied");
        return nullptr;
    }
    return gpuPipelineCache->LoadResource(appContext_, resourceRef, enablePlaceHolder);
}

std::unique_ptr<glimmer::Color> glimmer::ResourceLocator::FindColor(const ResourceRef *resourceRef,
                                                                    bool enablePlaceHolder) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    const ResourceTypeMessage resourceType = resourceRef->GetResourceType();
    if (!ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Access permission denied for color resource: packageId=",
                  resourceRef->GetPackageId(), ", resourceKey=", resourceRef->GetResourceKey());
        return nullptr;
    }

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
            LogCat::w(std::source_location::current(), "Failed to load color resource: packageId=",
                      resourceRef->GetPackageId(), ", resourceKey=", resourceRef->GetResourceKey());
            return nullptr;
        }
        return std::make_unique<Color>(colorResource->ToColor());
    }
    if (resourceType == RESOURCE_FIXED_COLOR) {
        const FixedColorResource *fixedColorResource = fixedColorManager_->FindFixedColorResource(
            resourceRef->GetPackageId(),
            resourceRef->GetResourceKey());
        if (fixedColorResource == nullptr) {
            LogCat::w(std::source_location::current(), "Failed to find fixed color resource: packageId=",
                      resourceRef->GetPackageId(), ", resourceKey=", resourceRef->GetResourceKey());
            return nullptr;
        }
        return std::make_unique<Color>(fixedColorResource->ToColor());
    }
    LogCat::w(std::source_location::current(), "Unsupported color resource type: packageId=",
              resourceRef->GetPackageId(), ", resourceKey=", resourceRef->GetResourceKey(), ", type=",
              std::to_underlying(resourceType));
    return nullptr;
}


glimmer::IShapeResource *glimmer::ResourceLocator::FindShape(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::e(std::source_location::current(), "resourceRef == nullptr");

        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_SHAPE || !ValidateAccessPermission(resourceRef)) {
        LogCat::e(std::source_location::current(), "Type mismatch: expected RESOURCE_SHAPE (",
                  std::to_underlying(RESOURCE_SHAPE), "), got ", std::to_underlying(resourceRef->GetResourceType()),
                  " or access permission denied");
        return nullptr;
    }
    if (shapeManager_ == nullptr) {
        LogCat::e(std::source_location::current(), "shapeManager_ == nullptr");
        return nullptr;
    }
    return shapeManager_->FindShape(resourceRef->GetPackageId(),
                                    resourceRef->GetResourceKey());
}

glimmer::IBiomeDecoratorResource *glimmer::ResourceLocator::FindBiomeDecorator(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_BIOME_DECORATOR || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_BIOME_DECORATOR (",
                  std::to_underlying(RESOURCE_BIOME_DECORATOR), "), got ",
                  std::to_underlying(resourceRef->GetResourceType()), " or access permission denied");
        return nullptr;
    }
    if (biomeDecoratorRegistry_ == nullptr) {
        LogCat::w(std::source_location::current(), "biomeDecoratorResourcesManager_ == nullptr");
        return nullptr;
    }
    return biomeDecoratorRegistry_->Find(
        resourceRef->GetPackageId(), resourceRef->GetResourceKey());
}

glimmer::StringResource *glimmer::ResourceLocator::FindString(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_STRING) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_STRING (",
                  std::to_underlying(RESOURCE_STRING), "), got ", std::to_underlying(resourceRef->GetResourceType()),
                  " Perhaps this is an optional string resource.");
        return nullptr;
    }
    if (!ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Access permission denied.");
        return nullptr;
    }
    if (stringManager_ == nullptr) {
        LogCat::w(std::source_location::current(), "stringManager_ == nullptr");
        return nullptr;
    }
    return stringManager_->Find(resourceRef->GetPackageId(),
                                resourceRef->GetResourceKey());
}

glimmer::LightSourceResource *glimmer::ResourceLocator::FindLightSource(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_LIGHT_SOURCE || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_LIGHT_SOURCE (",
                  std::to_underlying(RESOURCE_LIGHT_SOURCE), "), got ",
                  std::to_underlying(resourceRef->GetResourceType()), " or access permission denied");
        return nullptr;
    }
    if (lightSourceManager_ == nullptr) {
        LogCat::w(std::source_location::current(), "lightSourceManager_ == nullptr");
        return nullptr;
    }
    return lightSourceManager_->FindLightSourceResource(
        resourceRef->GetPackageId(),
        resourceRef->GetResourceKey());
}

glimmer::LightMaskResource *glimmer::ResourceLocator::FindLightMask(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_LIGHT_MASK || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_LIGHT_MASK (",
                  std::to_underlying(RESOURCE_LIGHT_MASK), "), got ",
                  std::to_underlying(resourceRef->GetResourceType()), " or access permission denied");
        return nullptr;
    }
    if (lightMaskManager_ == nullptr) {
        LogCat::w(std::source_location::current(), "lightMaskManager_ == nullptr");
        return nullptr;
    }
    return lightMaskManager_->FindLightMaskResource(resourceRef->GetPackageId(),
                                                    resourceRef->GetResourceKey());
}

glimmer::TileResource *glimmer::ResourceLocator::FindTileFallback(const ResourceRef *resourceRef,
                                                                  TileLayerType tileLayer) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (tileResourceManager_ == nullptr) {
        LogCat::w(std::source_location::current(), "tileResource == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_TILE || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_TILE (",
                  std::to_underlying(RESOURCE_TILE), "), got ", std::to_underlying(resourceRef->GetResourceType()),
                  " or access permission denied");
        return tileResourceManager_->GenerateAccessDeniedPlaceHolder(
            resourceRef->GetPackageId(), resourceRef->GetResourceKey(), tileLayer);
    }
    return tileResourceManager_->FindTileFallback(resourceRef->GetPackageId(),
                                                  resourceRef->GetResourceKey(),
                                                  tileLayer);
}

glimmer::TileResource *glimmer::ResourceLocator::FindTileRaw(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_TILE || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_TILE (",
                  std::to_underlying(RESOURCE_TILE), "), got ", std::to_underlying(resourceRef->GetResourceType()),
                  " or access permission denied");
        return nullptr;
    }
    if (tileResourceManager_ == nullptr) {
        LogCat::w(std::source_location::current(), "tileResource == nullptr");
        return nullptr;
    }
    return tileResourceManager_->FindTileRaw(resourceRef->GetPackageId(),
                                             resourceRef->GetResourceKey());
}

glimmer::MobResource *glimmer::ResourceLocator::FindMob(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_MOB || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_MOB (",
                  std::to_underlying(RESOURCE_MOB), "), got ", std::to_underlying(resourceRef->GetResourceType()),
                  " or access permission denied");
        return nullptr;
    }
    if (mobRegistry_ == nullptr) {
        LogCat::w(std::source_location::current(), "mobManager == nullptr");
        return nullptr;
    }
    return mobRegistry_->Find(resourceRef->GetPackageId(),
                              resourceRef->GetResourceKey());
}

glimmer::ComposableItemResource *glimmer::ResourceLocator::FindComposableItem(
    const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_COMPOSABLE_ITEM || !
        ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_COMPOSABLE_ITEM (",
                  std::to_underlying(RESOURCE_COMPOSABLE_ITEM), "), got ",
                  std::to_underlying(resourceRef->GetResourceType()), " or access permission denied");
        return nullptr;
    }
    if (composableItemRegistry_ == nullptr) {
        LogCat::w(std::source_location::current(), "itemManager == nullptr");
        return nullptr;
    }
    return composableItemRegistry_->Find(resourceRef->GetPackageId(),
                                         resourceRef->GetResourceKey());
}

glimmer::AbilityItemResource *glimmer::ResourceLocator::FindAbilityItem(
    const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_ABILITY_ITEM || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_ABILITY_ITEM (",
                  std::to_underlying(RESOURCE_ABILITY_ITEM), "), got ",
                  std::to_underlying(resourceRef->GetResourceType()), " or access permission denied");
        return nullptr;
    }
    if (abilityItemRegistry_ == nullptr) {
        LogCat::w(std::source_location::current(), "itemManager == nullptr");
        return nullptr;
    }
    return abilityItemRegistry_->Find(resourceRef->GetPackageId(),
                                      resourceRef->GetResourceKey());
}

glimmer::MaterialItemResource *glimmer::ResourceLocator::FindMaterialItem(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_MATERIAL_ITEM || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_MATERIAL_ITEM (",
                  std::to_underlying(RESOURCE_MATERIAL_ITEM), "), got ",
                  std::to_underlying(resourceRef->GetResourceType()), " or access permission denied");
        return nullptr;
    }
    if (materialItemRegistry_ == nullptr) {
        LogCat::w(std::source_location::current(), "itemManager == nullptr");
        return nullptr;
    }
    return materialItemRegistry_->Find(resourceRef->GetPackageId(),
                                       resourceRef->GetResourceKey());
}

glimmer::LootResource *glimmer::ResourceLocator::FindLoot(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        LogCat::w(std::source_location::current(), "Resource == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_LOOT_TABLE || !ValidateAccessPermission(resourceRef)) {
        LogCat::w(std::source_location::current(), "Type mismatch: expected RESOURCE_LOOT_TABLE (",
                  std::to_underlying(RESOURCE_LOOT_TABLE), "), got ",
                  std::to_underlying(resourceRef->GetResourceType()), " or access permission denied");
        return nullptr;
    }
    if (lootTableRegistry_ == nullptr) {
        LogCat::w(std::source_location::current(), "lootTableManager_ == nullptr");
        return nullptr;
    }
    return lootTableRegistry_->Find(resourceRef->GetPackageId(),
                                    resourceRef->GetResourceKey());
}

std::unique_ptr<glimmer::Item> glimmer::ResourceLocator::FindItem(WorldContext *worldContext,
                                                                  const ItemMessage &itemMessage) const {
    if (worldContext == nullptr) {
        LogCat::w(std::source_location::current(), "worldContext == nullptr");
        return nullptr;
    }
    ResourceRef resourceRef;
    resourceRef.ReadResourceRefMessage(itemMessage.itemresourceref());
    const ResourceTypeMessage resourceType = resourceRef.GetResourceType();
    if (resourceType == RESOURCE_NONE || !ValidateAccessPermission(&resourceRef)) {
        LogCat::w(std::source_location::current(),
                  "Invalid resource type (RESOURCE_NONE) or access permission denied for item resource: type=",
                  std::to_underlying(resourceType));
        return nullptr;
    }
    std::unique_ptr<Item> result = nullptr;
    if (resourceType == RESOURCE_TILE) {
        auto tileInstancePool = worldContext->GetTileInstancePool();
        if (tileInstancePool == nullptr) {
            LogCat::w(std::source_location::current(), "tileInstancePool == nullptr");
            return nullptr;
        }
        auto tileResource = FindTileRaw(&resourceRef);
        if (tileResource != nullptr) {
            result = std::make_unique<TileItem>(
                tileInstancePool->CreateTile(appContext_, tileResource, resourceRef.GetFingerprint()), resourceRef);
        }
    }
    if (resourceType == RESOURCE_COMPOSABLE_ITEM) {
        auto composableItemResource = FindComposableItem(&resourceRef);
        if (composableItemResource != nullptr) {
            result = std::move(
                ComposableItem::FromItemResource(worldContext, composableItemResource, resourceRef));
        }
    }

    if (resourceType == RESOURCE_ABILITY_ITEM) {
        auto abilityItemResource = FindAbilityItem(&resourceRef);
        if (abilityItemResource != nullptr) {
            result = std::move(AbilityItem::FromItemResource(appContext_, abilityItemResource, resourceRef));
        }
    }
    if (resourceType == RESOURCE_MATERIAL_ITEM) {
        auto materialItemResource = FindMaterialItem(&resourceRef);
        if (materialItemResource != nullptr) {
            result = std::move(MaterialItem::FromItemResource(appContext_, materialItemResource, resourceRef));
        }
    }
    if (result == nullptr) {
        LogCat::w(std::source_location::current(), "Failed to create item from resource: packageId=",
                  resourceRef.GetPackageId(), ", resourceKey=", resourceRef.GetResourceKey(), ", type=",
                  std::to_underlying(resourceType));
        return nullptr;
    }
    result->ReadItemMessage(worldContext, itemMessage);
    return result;
}

std::unique_ptr<glimmer::Item> glimmer::ResourceLocator::FindItem(WorldContext *worldContext,
                                                                  const ItemMessageResource &itemMessageResource)
const {
    if (worldContext == nullptr) {
        LogCat::w(std::source_location::current(), "worldContext == nullptr");
        return nullptr;
    }
    auto itemMessage = ItemMessage();
    itemMessage.set_locked(itemMessageResource.locked);
    itemMessage.set_amount(itemMessageResource.amount);
    itemMessageResource.item.WriteResourceRefMessage(*itemMessage.mutable_itemresourceref());
    for (auto &abilityItemResource: itemMessageResource.abilityItemRef) {
        ItemMessage *abilityItem = itemMessage.add_abilityitemref();
        abilityItemResource.item.WriteResourceRefMessage(*abilityItem->mutable_itemresourceref());
        abilityItem->set_locked(abilityItemResource.locked);
        abilityItem->set_amount(abilityItemResource.amount);
    }
    if (itemMessageResource.durabilityStrategyType < 0) {
        itemMessage.set_durabilitystrategy(
            static_cast<AllocStrategyTypeMessage>(RandomUtils::Random(0, 3))
        );
    } else {
        itemMessage.set_durabilitystrategy(
            static_cast<AllocStrategyTypeMessage>(itemMessageResource.durabilityStrategyType)
        );
    }
    return FindItem(worldContext, itemMessage);
}
