//
// Created by Cold-Mint on 2025/12/11.
//

#include "ResourceLocator.h"

#include "../Constants.h"
#include "../../core/scene/AppContext.h"
#include "core/inventory/AbilityItem.h"
#include "core/inventory/ComposableItem.h"
#include "core/inventory/TileItem.h"
#include "dataPack/StringManager.h"

std::optional<glimmer::StringResource *> glimmer::ResourceLocator::FindString(const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_STRING) {
        return std::nullopt;
    }
    return appContext_->GetStringManager()->Find(resourceRef.GetPackageId(), resourceRef.GetResourceKey());
}

std::optional<glimmer::TileResource *> glimmer::ResourceLocator::FindTile(const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_TILE) {
        return std::nullopt;
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
        return std::nullopt;
    }
    return appContext_->GetTileManager()->Find(resourceRef.GetPackageId(), resourceRef.GetResourceKey());
}

std::optional<glimmer::ComposableItemResource *> glimmer::ResourceLocator::FindComposableItem(
    const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_COMPOSABLE_ITEM) {
        return std::nullopt;
    }
    return appContext_->GetItemManager()->FindComposableItemResource(resourceRef.GetPackageId(),
                                                                     resourceRef.GetResourceKey());
}

std::optional<glimmer::AbilityItemResource *> glimmer::ResourceLocator::FindAbilityItem(
    const ResourceRef &resourceRef) const {
    if (resourceRef.GetResourceType() != RESOURCE_TYPE_ABILITY_ITEM) {
        return std::nullopt;
    }
    return appContext_->GetItemManager()->FindAbilityItemResource(resourceRef.GetPackageId(),
                                                                  resourceRef.GetResourceKey());
}

std::optional<std::unique_ptr<glimmer::Item> > glimmer::ResourceLocator::FindItem(AppContext *appContext,
    const ResourceRef &resourceRef) const {
    uint32_t resourceType = resourceRef.GetResourceType();
    std::unique_ptr<Item> result = nullptr;
    if (resourceType == RESOURCE_TYPE_TILE) {
        auto tileResource = FindTile(resourceRef);
        if (tileResource.has_value()) {
            result = std::make_unique<TileItem>(Tile::FromResourceRef(appContext, tileResource.value()));
        }
    }
    if (resourceType == RESOURCE_TYPE_COMPOSABLE_ITEM) {
        auto composableItemResource = FindComposableItem(resourceRef);
        if (composableItemResource.has_value()) {
            result = std::move(
                ComposableItem::FromItemResource(appContext, composableItemResource.value(), resourceRef));
        }
    }

    if (resourceType == RESOURCE_TYPE_ABILITY_ITEM) {
        auto abilityItemResource = FindAbilityItem(resourceRef);
        if (abilityItemResource.has_value()) {
            result = std::move(AbilityItem::FromItemResource(appContext, abilityItemResource.value(), resourceRef));
        }
    }
    if (result != nullptr) {
        result->ApplyResourceRefArgs(resourceRef);
        return result;
    }
    return std::nullopt;
}
