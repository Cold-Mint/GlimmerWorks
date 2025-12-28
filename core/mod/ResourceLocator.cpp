//
// Created by Cold-Mint on 2025/12/11.
//

#include "ResourceLocator.h"

#include "../Constants.h"
#include "../../core/scene/AppContext.h"
#include "dataPack/StringManager.h"

void glimmer::ResourceLocator::SetAppContext(AppContext *appContext) {
    appContext_ = appContext;
}

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
