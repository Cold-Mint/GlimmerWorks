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
#include "ItemFactory.h"

#include "AbilityItem.h"
#include "ComposableItem.h"
#include "MaterialItem.h"
#include "TileItem.h"
#include "core/log/LogCat.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"
#include "core/utils/RandomUtils.h"
#include "core/world/TileInstancePool.h"
#include "core/world/WorldContext.h"
#include "src/saves/item.pb.h"

glimmer::ItemFactory::ItemFactory(AppContext *appContext, ResourceLocator *resourceLocator)
    : appContext_(appContext), resourceLocator_(resourceLocator) {
}

std::unique_ptr<glimmer::Item> glimmer::ItemFactory::CreateItem(WorldContext *worldContext,
                                                                const ItemMessage &itemMessage) const {
    if (worldContext == nullptr) {
        LogCat::w(std::source_location::current(), "world_context_is_null", "worldContext == nullptr");
        return nullptr;
    }
    ResourceRef resourceRef;
    resourceRef.ReadResourceRefMessage(itemMessage.itemresourceref());
    const ResourceTypeMessage resourceType = resourceRef.GetResourceType();
    if (resourceType == RESOURCE_NONE || !resourceLocator_->ValidateAccessPermission(&resourceRef)) {
        LogCat::w(std::source_location::current(), "item_resource_invalid_type",
                  "Invalid resource type (RESOURCE_NONE) or access permission denied for item resource: type={}",
                  std::to_underlying(resourceType));
        return nullptr;
    }
    std::unique_ptr<Item> result = nullptr;
    if (resourceType == RESOURCE_TILE) {
        auto tileInstancePool = worldContext->GetTileInstancePool();
        if (tileInstancePool == nullptr) {
            LogCat::w(std::source_location::current(), "tile_instance_pool_is_null", "tileInstancePool == nullptr");
            return nullptr;
        }
        auto tileResource = resourceLocator_->FindTileRaw(&resourceRef);
        if (tileResource != nullptr) {
            result = std::make_unique<TileItem>(
                tileInstancePool->CreateTile(appContext_, tileResource, resourceRef.GetFingerprint()), resourceRef);
        }
    }
    if (resourceType == RESOURCE_COMPOSABLE_ITEM) {
        auto composableItemResource = resourceLocator_->FindComposableItem(&resourceRef);
        if (composableItemResource != nullptr) {
            result = std::move(
                ComposableItem::FromItemResource(worldContext, composableItemResource, resourceRef));
        }
    }
    if (resourceType == RESOURCE_ABILITY_ITEM) {
        auto abilityItemResource = resourceLocator_->FindAbilityItem(&resourceRef);
        if (abilityItemResource != nullptr) {
            result = std::move(AbilityItem::FromItemResource(appContext_, abilityItemResource, resourceRef));
        }
    }
    if (resourceType == RESOURCE_MATERIAL_ITEM) {
        auto materialItemResource = resourceLocator_->FindMaterialItem(&resourceRef);
        if (materialItemResource != nullptr) {
            result = std::move(MaterialItem::FromItemResource(appContext_, materialItemResource, resourceRef));
        }
    }
    if (result == nullptr) {
        LogCat::w(std::source_location::current(), "item_creation_failed",
                  "Failed to create item from resource: packageId={}, resourceKey={}, type={}",
                  resourceRef.GetPackageId(), resourceRef.GetResourceKey(), std::to_underlying(resourceType));
        return nullptr;
    }
    result->ReadItemMessage(worldContext, itemMessage);
    LogCat::d("item_factory_create_success", "Create item success: itemId={} type={}", result->GetId(),
              std::to_underlying(resourceType));
    return result;
}

std::unique_ptr<glimmer::Item> glimmer::ItemFactory::CreateItem(WorldContext *worldContext,
                                                                const ItemMessageResource &itemMessageResource) const {
    if (worldContext == nullptr) {
        LogCat::w(std::source_location::current(), "world_context_is_null", "worldContext == nullptr");
        return nullptr;
    }
    auto itemMessage = ItemMessage();
    LogCat::d("item_factory_create_from_message",
              "Create item from item message resource: amount={} abilityItemCount={}",
              itemMessageResource.amount, itemMessageResource.abilityItemRef.size());
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
        const auto randomStrategy = static_cast<AllocStrategyTypeMessage>(RandomUtils::Random(0, 3));
        LogCat::d("item_factory_create_random_strategy",
                  "Durability strategy not specified, use random strategy: {}",
                  std::to_underlying(randomStrategy));
        itemMessage.set_durabilitystrategy(randomStrategy);
    } else {
        itemMessage.set_durabilitystrategy(
            static_cast<AllocStrategyTypeMessage>(itemMessageResource.durabilityStrategyType)
        );
    }
    return CreateItem(worldContext, itemMessage);
}
