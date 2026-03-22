//
// Created by Cold-Mint on 2025/12/11.
//

#include "ResourceLocator.h"

#include "../Constants.h"
#include "../../core/scene/AppContext.h"
#include "core/inventory/AbilityItem.h"
#include "core/inventory/ComposableItem.h"
#include "core/inventory/TileItem.h"
#include "core/log/LogCat.h"
#include "dataPack/StringManager.h"

bool glimmer::ResourceLocator::ValidateAccessPermission(const ResourceRef &resourceRef) const {
    if (resourceRef.GetSelfPackageId() == resourceRef.GetPackageId()) {
        //Allow access to one's own package.
        //允许访问自身包。
        return true;
    }

    bool result = appContext_->GetDataPackManager()->IsDependencySatisfied(
        resourceRef.GetSelfPackageId(), resourceRef.GetPackageId());;
    if (!result) {
        LogCat::w("Prevented access to resources. Source Package ID: ", resourceRef.GetSelfPackageId(),
                  ", Target Package ID: ", resourceRef.GetPackageId(), ".");
    }
    return result;
}

glimmer::ResourceLocator::ResourceLocator(AppContext *appContext_) : appContext_(appContext_) {
}

std::shared_ptr<SDL_Texture> glimmer::ResourceLocator::FindTexture(const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_TEXTURES || !ValidateAccessPermission(resourceRef)) {
        return appContext_->GetResourcePackManager()->errorTexture_;
    }
    return appContext_->GetResourcePackManager()->LoadTextureFromFile(appContext_, resourceRef);
}

glimmer::ColorResource *glimmer::ResourceLocator::FindColorResource(const ResourceRef &resourceRef,
                                                                    ColorResource *defaultColor) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_COLOR || !ValidateAccessPermission(resourceRef)) {
        if (defaultColor == nullptr) {
            return appContext_->GetResourcePackManager()->globalDefaultColor_.get();
        }
        return defaultColor;
    }
    return appContext_->GetResourcePackManager()->LoadColorResFromFile(appContext_, resourceRef, defaultColor);
}

glimmer::ShapeResource *glimmer::ResourceLocator::FindShape(const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_SHAPE || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetShapeManager()->FindShape(resourceRef.GetPackageId(), resourceRef.GetResourceKey());
}

glimmer::StringResource *glimmer::ResourceLocator::FindString(const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_STRING || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetStringManager()->Find(resourceRef.GetPackageId(), resourceRef.GetResourceKey());
}

glimmer::TileResource *glimmer::ResourceLocator::FindTile(const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_TILE || !ValidateAccessPermission(resourceRef)) {
        return appContext_->GetTileManager()->GetError();
    }
    if (resourceRef.GetPackageId() == RESOURCE_REF_CORE) {
        if (resourceRef.GetResourceKey() == TILE_ID_AIR) {
            return appContext_->GetTileManager()->GetAir();
        }
        if (resourceRef.GetResourceKey() == TILE_ID_WATER) {
            return appContext_->GetTileManager()->GetWater();
        }
        if (resourceRef.GetResourceKey() == TILE_ID_BEDROCK) {
            return appContext_->GetTileManager()->GetBedrock();
        }
        if (resourceRef.GetResourceKey() == TILE_ID_ERROR) {
            return appContext_->GetTileManager()->GetError();
        }
    }
    return appContext_->GetTileManager()->Find(resourceRef.GetPackageId(), resourceRef.GetResourceKey());
}

glimmer::MobResource *glimmer::ResourceLocator::FindMob(const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_MOB || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetMobManager()->FindMobResource(resourceRef.GetPackageId(),
                                                         resourceRef.GetResourceKey());
}

glimmer::ComposableItemResource *glimmer::ResourceLocator::FindComposableItem(
    const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_COMPOSABLE_ITEM || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetItemManager()->FindComposableItemResource(resourceRef.GetPackageId(),
                                                                     resourceRef.GetResourceKey());
}

glimmer::AbilityItemResource *glimmer::ResourceLocator::FindAbilityItem(
    const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_ABILITY_ITEM || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetItemManager()->FindAbilityItemResource(resourceRef.GetPackageId(),
                                                                  resourceRef.GetResourceKey());
}

glimmer::LootResource *glimmer::ResourceLocator::FindLoot(const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_LOOT_TABLE || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    return appContext_->GetLootTableManager()->Find(resourceRef.GetPackageId(),
                                                    resourceRef.GetResourceKey());
}

std::unique_ptr<glimmer::Item>
glimmer::ResourceLocator::FindItem(const ItemMessage &itemMessage) const {
    ResourceRef resourceRef;
    resourceRef.ReadResourceRefMessage(itemMessage.itemresourceref());
    const uint32_t resourceType = resourceRef.GetResourceType();
    if (resourceType == RESOURCE_TYPE_NONE || !ValidateAccessPermission(resourceRef)) {
        return nullptr;
    }
    std::unique_ptr<Item> result = nullptr;
    if (resourceType == RESOURCE_TYPE_TILE) {
        auto tileResource = FindTile(resourceRef);
        if (tileResource != nullptr) {
            result = std::make_unique<TileItem>(Tile::FromTileResource(appContext_, tileResource));
        }
    }
    if (resourceType == RESOURCE_TYPE_COMPOSABLE_ITEM) {
        auto composableItemResource = FindComposableItem(resourceRef);
        if (composableItemResource != nullptr) {
            result = std::move(
                ComposableItem::FromItemResource(appContext_, composableItemResource));
        }
    }

    if (resourceType == RESOURCE_TYPE_ABILITY_ITEM) {
        auto abilityItemResource = FindAbilityItem(resourceRef);
        if (abilityItemResource != nullptr) {
            result = std::move(AbilityItem::FromItemResource(appContext_, abilityItemResource, resourceRef));
        }
    }
    if (result == nullptr) {
        return nullptr;
    }
    result->ReadItemMessage(appContext_, itemMessage);
    return result;
}

std::unique_ptr<glimmer::Item>
glimmer::ResourceLocator::FindItem(const ItemMessageResource &itemMessageResource) const {
    auto itemMessage = ItemMessage();
    itemMessage.set_amount(itemMessageResource.amount);
    itemMessageResource.item.WriteResourceRefMessage(*itemMessage.mutable_itemresourceref());
    for (auto &abilityItemResource: itemMessageResource.abilityItemRef) {
        ItemMessage *abilityItem = itemMessage.add_abilityitemref();
        abilityItemResource.item.WriteResourceRefMessage(*abilityItem->mutable_itemresourceref());
        abilityItem->set_amount(abilityItemResource.amount);
    }
    return FindItem(itemMessage);
}
