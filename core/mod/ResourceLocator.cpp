//
// Created by Cold-Mint on 2025/12/11.
//

#include "ResourceLocator.h"

#include "core/scene/AppContext.h"
#include "core/inventory/AbilityItem.h"
#include "core/inventory/ComposableItem.h"
#include "core/inventory/TileItem.h"
#include "core/log/LogCat.h"
#include "core/world/WorldContext.h"
#include "core/world/TileInstancePool.h"
#include "dataPack/StringManager.h"

bool glimmer::ResourceLocator::ValidateAccessPermission(const ResourceRef *resourceRef) const {
    if (resourceRef->GetSelfPackageId() == resourceRef->GetPackageId()) {
        //Allow access to one's own package.
        //允许访问自身包。
        return true;
    }

    bool result = appContext_->GetDataPackManager()->IsDependencySatisfied(
        resourceRef->GetSelfPackageId(), resourceRef->GetPackageId());;
    if (!result) {
        LogCat::w("Prevented access to resources. Source Package ID: ", resourceRef->GetSelfPackageId(),
                  ", Target Package ID: ", resourceRef->GetPackageId(), ".");
    }
    return result;
}

glimmer::ResourceLocator::ResourceLocator(AppContext *appContext_) : appContext_(appContext_) {
}

std::shared_ptr<SDL_Texture> glimmer::ResourceLocator::FindTexture(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != Texture) {
        return appContext_->GetResourcePackManager()->errorTexture_;
    }
    if (!ValidateAccessPermission(resourceRef)) {
        return appContext_->GetResourcePackManager()->accessDeniedTexture_;
    }
    return appContext_->GetResourcePackManager()->LoadTextureFromFile(appContext_, resourceRef);
}

std::shared_ptr<MIX_Audio> glimmer::ResourceLocator::FindAudio(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != ResourceTypeMessage::Audio || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetResourcePackManager()->LoadAudioFromFile(appContext_, resourceRef);
}

std::unique_ptr<glimmer::Color> glimmer::ResourceLocator::FindColor(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    const uint32_t resourceType = resourceRef->GetResourceType();
    if (!ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    if (resourceType == ResourceTypeMessage::Color) {
        const ColorResource *colorResource = appContext_->GetResourcePackManager()->LoadColorResFromFile(
            appContext_, resourceRef);
        if (colorResource == nullptr) {
            return nullptr;
        }
        return std::make_unique<Color>(colorResource->ToColor());
    }
    if (resourceType == FixedColor) {
        const FixedColorResource *fixedColorResource = appContext_->GetFixedColorManager()->FindFixedColorResource(
            resourceRef->GetPackageId(),
            resourceRef->GetResourceKey());
        if (fixedColorResource == nullptr) {
            return nullptr;
        }
        return std::make_unique<Color>(fixedColorResource->ToColor());
    }
    return nullptr;
}


glimmer::IShapeResource *glimmer::ResourceLocator::FindShape(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != Shape || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetShapeManager()->FindShape(resourceRef->GetPackageId(), resourceRef->GetResourceKey());
}

glimmer::IBiomeDecoratorResource *glimmer::ResourceLocator::FindBiomeDecorator(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != BiomeDecorator || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetBiomeDecoratorResourcesManager()->FindBiomeDecorator(
        resourceRef->GetPackageId(), resourceRef->GetResourceKey());
}

glimmer::StringResource *glimmer::ResourceLocator::FindString(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != String || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetStringManager()->Find(resourceRef->GetPackageId(), resourceRef->GetResourceKey());
}

glimmer::LightSourceResource *glimmer::ResourceLocator::FindLightSource(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != ResourceTypeMessage::LightSource || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetLightSourceManager()->FindLightSourceResource(resourceRef->GetPackageId(),
                                                                         resourceRef->GetResourceKey());
}

glimmer::LightMaskResource *glimmer::ResourceLocator::FindLightMask(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != ResourceTypeMessage::LightMask || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetLightMaskManager()->FindLightMaskResource(resourceRef->GetPackageId(),
                                                                     resourceRef->GetResourceKey());
}

glimmer::TileResource *glimmer::ResourceLocator::FindTileFallback(const ResourceRef *resourceRef,
                                                                  TileLayerType tileLayer) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != ResourceTypeMessage::Tile || !ValidateAccessPermission(resourceRef)) {
        return appContext_->GetTileResourceManager()->GenerateAccessDeniedPlaceHolder(
            resourceRef->GetPackageId(), resourceRef->GetResourceKey(), tileLayer);
    }
    return appContext_->GetTileResourceManager()->FindTileFallback(resourceRef->GetPackageId(),
                                                                   resourceRef->GetResourceKey(),
                                                                   tileLayer);
}

glimmer::TileResource *glimmer::ResourceLocator::FindTileRaw(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != ResourceTypeMessage::Tile || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetTileResourceManager()->FindTileRaw(resourceRef->GetPackageId(),
                                                              resourceRef->GetResourceKey());
}

glimmer::MobResource *glimmer::ResourceLocator::FindMob(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != Mob || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetMobManager()->FindMobResource(resourceRef->GetPackageId(),
                                                         resourceRef->GetResourceKey());
}

glimmer::ComposableItemResource *glimmer::ResourceLocator::FindComposableItem(
    const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != ResourceTypeMessage::ComposableItem || !
        ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetItemManager()->FindComposableItemResource(resourceRef->GetPackageId(),
                                                                     resourceRef->GetResourceKey());
}

glimmer::AbilityItemResource *glimmer::ResourceLocator::FindAbilityItem(
    const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != ResourceTypeMessage::AbilityItem || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetItemManager()->FindAbilityItemResource(resourceRef->GetPackageId(),
                                                                  resourceRef->GetResourceKey());
}

glimmer::LootResource *glimmer::ResourceLocator::FindLoot(const ResourceRef *resourceRef) const {
    if (resourceRef == nullptr) {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != LootTable || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetLootTableManager()->Find(resourceRef->GetPackageId(),
                                                    resourceRef->GetResourceKey());
}

std::unique_ptr<glimmer::Item> glimmer::ResourceLocator::FindItem(WorldContext *worldContext,
                                                                  const ItemMessage &itemMessage) const {
    if (worldContext == nullptr) {
        return nullptr;
    }
    ResourceRef resourceRef;
    resourceRef.ReadResourceRefMessage(itemMessage.itemresourceref());
    const uint32_t resourceType = resourceRef.GetResourceType();
    if (resourceType == ResourceTypeMessage::None || !ValidateAccessPermission(&resourceRef)) {
        return nullptr;
    }
    std::unique_ptr<Item> result = nullptr;
    if (resourceType == ResourceTypeMessage::Tile) {
        auto tileInstancePool = worldContext->GetTileInstancePool();
        if (tileInstancePool == nullptr) {
            return nullptr;
        }
        auto tileResource = FindTileRaw(&resourceRef);
        if (tileResource != nullptr) {
            result = std::make_unique<TileItem>(
                tileInstancePool->CreateTile(appContext_, tileResource, resourceRef.GetFingerprint()), resourceRef);
        }
    }
    if (resourceType == ResourceTypeMessage::ComposableItem) {
        auto composableItemResource = FindComposableItem(&resourceRef);
        if (composableItemResource != nullptr) {
            result = std::move(
                ComposableItem::FromItemResource(worldContext, composableItemResource, resourceRef));
        }
    }

    if (resourceType == ResourceTypeMessage::AbilityItem) {
        auto abilityItemResource = FindAbilityItem(&resourceRef);
        if (abilityItemResource != nullptr) {
            result = std::move(AbilityItem::FromItemResource(appContext_, abilityItemResource, resourceRef));
        }
    }
    if (result == nullptr) {
        return nullptr;
    }
    result->ReadItemMessage(worldContext, itemMessage);
    return result;
}

std::unique_ptr<glimmer::Item> glimmer::ResourceLocator::FindItem(WorldContext *worldContext,
                                                                  const ItemMessageResource &itemMessageResource)
const {
    if (worldContext == nullptr) {
        return nullptr;
    }
    auto itemMessage = ItemMessage();
    itemMessage.set_amount(itemMessageResource.amount);
    itemMessageResource.item.WriteResourceRefMessage(*itemMessage.mutable_itemresourceref());
    for (auto &abilityItemResource: itemMessageResource.abilityItemRef) {
        ItemMessage *abilityItem = itemMessage.add_abilityitemref();
        abilityItemResource.item.WriteResourceRefMessage(*abilityItem->mutable_itemresourceref());
        abilityItem->set_amount(abilityItemResource.amount);
    }
    return FindItem(worldContext, itemMessage);
}
