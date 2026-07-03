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
#include "GiveCommand.h"

#include "core/Constants.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/inventory/AbilityItem.h"
#include "core/inventory/ComposableItem.h"
#include "core/inventory/ItemContainer.h"
#include "core/inventory/TileItem.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "core/inventory/MaterialItem.h"
#include "fmt/color.h"

void glimmer::GiveCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("tileItem")->AddChild(TILE_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("composableItem")->AddChild(COMPOSABLE_ITEM_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("abilityItem")->AddChild(ABILITY_ITEM_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("materialItem")->AddChild(MATERIAL_ITEM_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::GiveCommand::GiveCommand(AppContext* appContext)
    : Command(appContext)
{
}

void glimmer::GiveCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[item_type:string]");
    strings->emplace_back("[item_id:string]");
    strings->emplace_back("[number:int]");
}

bool glimmer::GiveCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                   const std::function<void(const std::string& text)>* onMessage)
{
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    if (worldContext_ == nullptr)
    {
        onMessageRef(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    const size_t size = commandArgs->GetSize();
    if (size < 3)
    {
        onMessageRef(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            3, size));
        return false;
    }
    const std::string itemType = commandArgs->AsString(1);
    if (itemType == "tileItem")
    {
        auto itemId = commandArgs->AsResourceRef(2, RESOURCE_TILE);
        if (!itemId.has_value())
        {
            onMessageRef(appContext_->GetLangsResources()->itemIdNotFound);
            return false;
        }
        ResourceRef& resourceRef = itemId.value();
        const auto* tileResource = appContext_->GetResourceLocator()->FindTileRaw(&resourceRef);
        if (tileResource == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->tileResourceIsNull);
            return false;
        }
        EntityManager* entityManager = worldContext_->GetEntityManager();
        if (entityManager == nullptr)
        {
            return false;
        }
        EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut();
        if (entityShortCut == nullptr)
        {
            return false;
        }
        auto playerId = entityShortCut->GetPlayer();
        if (WorldContext::IsEmptyEntityId(playerId))
        {
            return false;
        }
        auto* itemContainer = entityManager->GetComponent<ItemContainerComponent>(playerId);
        if (itemContainer == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->itemContainerIsNull);
            return false;
        }
        TileInstancePool* tileInstancePool = worldContext_->GetTileInstancePool();
        if (tileInstancePool == nullptr)
        {
            return false;
        }
        auto tileItem = std::make_unique<
            TileItem>(tileInstancePool->CreateTile(appContext_, tileResource, resourceRef.GetFingerprint()),
                      resourceRef);
        if (size >= 4)
        {
            if (const int number = commandArgs->AsInt(3); number > 1)
            {
                tileItem->SetAmount(number);
            }
        }
        std::unique_ptr<Item> item = itemContainer->GetItemContainer()->AddItem(
            std::move(tileItem));
        return item == nullptr;
    }
    if (itemType == "composableItem")
    {
        auto itemId = commandArgs->AsResourceRef(2, RESOURCE_COMPOSABLE_ITEM);
        if (!itemId.has_value())
        {
            onMessageRef(appContext_->GetLangsResources()->itemIdNotFound);
            return false;
        }
        ResourceRef& resourceRef = itemId.value();
        auto* itemResource = appContext_->GetResourceLocator()->FindComposableItem(&resourceRef);
        if (itemResource == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->itemResourceIsNull);
            return false;
        }
        EntityManager* entityManager = worldContext_->GetEntityManager();
        if (entityManager == nullptr)
        {
            return false;
        }
        EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut();
        if (entityShortCut == nullptr)
        {
            return false;
        }
        auto playerId = entityShortCut->GetPlayer();
        if (WorldContext::IsEmptyEntityId(playerId))
        {
            return false;
        }
        auto* itemContainerComponent = entityManager->GetComponent<ItemContainerComponent>(playerId);
        if (itemContainerComponent == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->itemContainerIsNull);
            return false;
        }
        auto* itemContainer = itemContainerComponent->GetItemContainer();
        if (itemContainer == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->itemContainerIsNull);
            return false;
        }

        auto composableItem = ComposableItem::FromItemResource(worldContext_, itemResource, resourceRef);
        if (composableItem == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->composableItemIsNull);
            return false;
        }
        if (size >= 4)
        {
            if (const int number = commandArgs->AsInt(3); number > 1)
            {
                composableItem->SetAmount(number);
            }
        }
        std::unique_ptr<Item> item = itemContainer->AddItem(
            std::move(composableItem));
        return item == nullptr;
    }
    if (itemType == "abilityItem")
    {
        auto itemId = commandArgs->AsResourceRef(2, RESOURCE_ABILITY_ITEM);
        if (!itemId.has_value())
        {
            onMessageRef(appContext_->GetLangsResources()->itemIdNotFound);
            return false;
        }
        ResourceRef& resourceRef = itemId.value();
        auto* itemResource = appContext_->GetResourceLocator()->FindAbilityItem(&resourceRef);
        if (itemResource == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->itemResourceIsNull);
            return false;
        }
        EntityManager* entityManager = worldContext_->GetEntityManager();
        if (entityManager == nullptr)
        {
            return false;
        }
        EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut();
        if (entityShortCut == nullptr)
        {
            return false;
        }
        auto playerId = entityShortCut->GetPlayer();
        if (WorldContext::IsEmptyEntityId(playerId))
        {
            return false;
        }
        auto* itemContainerComponent = entityManager->GetComponent<ItemContainerComponent>(playerId);
        if (itemContainerComponent == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->itemContainerIsNull);
            return false;
        }
        auto* itemContainer = itemContainerComponent->GetItemContainer();
        if (itemContainer == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->itemContainerIsNull);
            return false;
        }

        auto abilityItem = AbilityItem::FromItemResource(appContext_, itemResource, resourceRef);
        if (abilityItem == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->composableItemIsNull);
            return false;
        }
        if (size >= 4)
        {
            if (const int number = commandArgs->AsInt(3); number > 1)
            {
                abilityItem->SetAmount(number);
            }
        }
        std::unique_ptr<Item> item = itemContainer->AddItem(
            std::move(abilityItem));
        return item == nullptr;
    }
    if (itemType == "materialItem")
    {
        auto itemId = commandArgs->AsResourceRef(2, RESOURCE_MATERIAL_ITEM);
        if (!itemId.has_value())
        {
            onMessageRef(appContext_->GetLangsResources()->itemIdNotFound);
            return false;
        }
        ResourceRef& resourceRef = itemId.value();
        auto* itemResource = appContext_->GetResourceLocator()->FindMaterialItem(&resourceRef);
        if (itemResource == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->itemResourceIsNull);
            return false;
        }
        EntityManager* entityManager = worldContext_->GetEntityManager();
        if (entityManager == nullptr)
        {
            return false;
        }
        EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut();
        if (entityShortCut == nullptr)
        {
            return false;
        }
        auto playerId = entityShortCut->GetPlayer();
        if (WorldContext::IsEmptyEntityId(playerId))
        {
            return false;
        }
        auto* itemContainerComponent = entityManager->GetComponent<ItemContainerComponent>(playerId);
        if (itemContainerComponent == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->itemContainerIsNull);
            return false;
        }

        auto* itemContainer = itemContainerComponent->GetItemContainer();
        if (itemContainer == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->itemContainerIsNull);
            return false;
        }

        auto materialItem = MaterialItem::FromItemResource(appContext_, itemResource, resourceRef);
        if (materialItem == nullptr)
        {
            onMessageRef(appContext_->GetLangsResources()->composableItemIsNull);
            return false;
        }
        if (size >= 4)
        {
            if (const int number = commandArgs->AsInt(3); number > 1)
            {
                materialItem->SetAmount(number);
            }
        }
        std::unique_ptr<Item> item = itemContainer->AddItem(
            std::move(materialItem));
        return item == nullptr;
    }
    onMessageRef(appContext_->GetLangsResources()->unknownCommandParameters);
    return false;
}

std::string glimmer::GiveCommand::GetName() const
{
    return GIVE_COMMAND_NAME;
}

bool glimmer::GiveCommand::RequiresWorldContext() const
{
    return true;
}
