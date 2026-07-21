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
#include "core/mod/ResourceLocator.h"
#include "core/context/AppContext.h"
#include "core/world/WorldContext.h"
#include "core/inventory/MaterialItem.h"
#include "core/world/TileInstancePool.h"
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

void glimmer::GiveCommand::TrySetItemAmount(const CommandArgs* commandArgs, Item* item)
{
    if (const size_t size = commandArgs->GetSize(); size < 4)
    {
        return;
    }
    ItemStackModule* itemStackModule = item->GetMutableStackModule();
    if (itemStackModule == nullptr)
    {
        return;
    }
    if (const int number = commandArgs->AsInt(3); number > 1)
    {
        itemStackModule->SetAmount(number);
    }
}

template <typename Callback>
std::optional<glimmer::ItemContainerComponent*> glimmer::GiveCommand::TryGetPlayerItemContainer(
    const WorldContext* worldContext,
    const Callback& onMessage) const
{
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr || worldContext == nullptr)
    {
        return std::nullopt;
    }
    const LangsResources* langsResources = appContext->GetLangsResources();
    if (langsResources == nullptr)
    {
        return std::nullopt;
    }
    EntityManager* entityManager = worldContext->GetEntityManager();
    if (entityManager == nullptr)
    {
        return std::nullopt;
    }
    const EntityShortCut* entityShortCut = worldContext->GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        return std::nullopt;
    }
    const GameEntityID playerId = entityShortCut->GetPlayer();
    if (WorldContext::IsEmptyEntityId(playerId))
    {
        return std::nullopt;
    }
    auto itemContainer = entityManager->GetComponent<ItemContainerComponent>(playerId);
    if (itemContainer == nullptr)
    {
        onMessage(langsResources->itemContainerIsNull);
        return std::nullopt;
    }
    return itemContainer;
}

template <typename Callback>
bool glimmer::GiveCommand::GiveTileItem(const AppContext* appContext,
                                        const WorldContext* worldContext,
                                        const CommandArgs* commandArgs,
                                        const Callback& onMessage) const
{
    const LangsResources* langsResources = appContext->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    auto itemId = commandArgs->AsResourceRef(2, RESOURCE_TILE);
    if (!itemId.has_value())
    {
        onMessage(langsResources->itemIdNotFound);
        return false;
    }
    ResourceRef& resourceRef = itemId.value();
    const auto tileResource = appContext->GetResourceLocator()->FindTileRaw(&resourceRef);
    if (tileResource == nullptr)
    {
        onMessage(langsResources->tileResourceIsNull);
        return false;
    }
    TileInstancePool* tileInstancePool = worldContext->GetTileInstancePool();
    if (tileInstancePool == nullptr)
    {
        return false;
    }
    const auto containerOpt = TryGetPlayerItemContainer(worldContext, onMessage);
    if (!containerOpt.has_value())
    {
        return false;
    }
    auto itemContainerComponent = containerOpt.value();
    auto itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        onMessage(langsResources->itemContainerIsNull);
        return false;
    }
    auto tileItem = std::make_unique<TileItem>(
        tileInstancePool->CreateTile(appContext, tileResource, resourceRef.GetFingerprint()),
        resourceRef);
    TrySetItemAmount(commandArgs, tileItem.get());
    std::unique_ptr<Item> item = itemContainer->AddItem(std::move(tileItem));
    return item == nullptr;
}

template <typename Callback>
bool glimmer::GiveCommand::GiveComposableItem(const AppContext* appContext,
                                              WorldContext* worldContext,
                                              const CommandArgs* commandArgs,
                                              const Callback& onMessage) const
{
    const LangsResources* langsResources = appContext->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    auto itemId = commandArgs->AsResourceRef(2, RESOURCE_COMPOSABLE_ITEM);
    if (!itemId.has_value())
    {
        onMessage(langsResources->itemIdNotFound);
        return false;
    }
    const ResourceRef& resourceRef = itemId.value();
    const auto itemResource = appContext->GetResourceLocator()->FindComposableItem(&resourceRef);
    if (itemResource == nullptr)
    {
        onMessage(langsResources->itemResourceIsNull);
        return false;
    }
    const auto containerOpt = TryGetPlayerItemContainer(worldContext, onMessage);
    if (!containerOpt.has_value())
    {
        return false;
    }
    const auto itemContainerComponent = containerOpt.value();
    auto itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        onMessage(langsResources->itemContainerIsNull);
        return false;
    }
    auto composableItem = ComposableItem::FromItemResource(worldContext, itemResource, resourceRef);
    if (composableItem == nullptr)
    {
        onMessage(langsResources->composableItemIsNull);
        return false;
    }
    TrySetItemAmount(commandArgs, composableItem.get());
    const std::unique_ptr<Item> item = itemContainer->AddItem(std::move(composableItem));
    return item == nullptr;
}

template <typename Callback>
bool glimmer::GiveCommand::GiveAbilityItem(const AppContext* appContext,
                                           const WorldContext* worldContext,
                                           const CommandArgs* commandArgs,
                                           const Callback& onMessage) const
{
    const LangsResources* langsResources = appContext->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    const auto itemId = commandArgs->AsResourceRef(2, RESOURCE_ABILITY_ITEM);
    if (!itemId.has_value())
    {
        onMessage(langsResources->itemIdNotFound);
        return false;
    }
    const ResourceRef& resourceRef = itemId.value();
    const auto itemResource = appContext->GetResourceLocator()->FindAbilityItem(&resourceRef);
    if (itemResource == nullptr)
    {
        onMessage(langsResources->itemResourceIsNull);
        return false;
    }
    const auto containerOpt = TryGetPlayerItemContainer(worldContext, onMessage);
    if (!containerOpt.has_value())
    {
        return false;
    }
    auto itemContainerComponent = containerOpt.value();
    auto itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        onMessage(langsResources->itemContainerIsNull);
        return false;
    }
    auto abilityItem = AbilityItem::FromItemResource(appContext, itemResource, resourceRef);
    if (abilityItem == nullptr)
    {
        onMessage(langsResources->composableItemIsNull);
        return false;
    }
    TrySetItemAmount(commandArgs, abilityItem.get());
    const std::unique_ptr<Item> item = itemContainer->AddItem(std::move(abilityItem));
    return item == nullptr;
}

template <typename Callback>
bool glimmer::GiveCommand::GiveMaterialItem(const AppContext* appContext,
                                            const WorldContext* worldContext,
                                            const CommandArgs* commandArgs,
                                            const Callback& onMessage) const
{
    const LangsResources* langsResources = appContext->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    auto itemId = commandArgs->AsResourceRef(2, RESOURCE_MATERIAL_ITEM);
    if (!itemId.has_value())
    {
        onMessage(langsResources->itemIdNotFound);
        return false;
    }
    const ResourceRef& resourceRef = itemId.value();
    const auto itemResource = appContext->GetResourceLocator()->FindMaterialItem(&resourceRef);
    if (itemResource == nullptr)
    {
        onMessage(langsResources->itemResourceIsNull);
        return false;
    }
    const auto containerOpt = TryGetPlayerItemContainer(worldContext, onMessage);
    if (!containerOpt.has_value())
    {
        return false;
    }
    const auto itemContainerComponent = containerOpt.value();
    auto itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        onMessage(langsResources->itemContainerIsNull);
        return false;
    }
    auto materialItem = MaterialItem::FromItemResource(appContext, itemResource, resourceRef);
    if (materialItem == nullptr)
    {
        onMessage(langsResources->composableItemIsNull);
        return false;
    }
    TrySetItemAmount(commandArgs, materialItem.get());
    const std::unique_ptr<Item> item = itemContainer->AddItem(std::move(materialItem));
    return item == nullptr;
}

bool glimmer::GiveCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                   const std::function<void(const std::string& text)>* onMessage)
{
    const AppContext* appContext = GetAppContext();
    WorldContext* worldContext = GetWorldContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    if (worldContext == nullptr)
    {
        onMessageRef(appContext->GetLangsResources()->worldContextIsNull);
        return false;
    }
    if (const size_t size = commandArgs->GetSize(); size < 3)
    {
        onMessageRef(fmt::format(
            fmt::runtime(appContext->GetLangsResources()->insufficientParameterLength),
            3, size));
        return false;
    }
    const std::string itemType = commandArgs->AsString(1);
    if (itemType == "tileItem")
    {
        return GiveTileItem(appContext, worldContext, commandArgs, onMessageRef);
    }
    if (itemType == "composableItem")
    {
        return GiveComposableItem(appContext, worldContext, commandArgs, onMessageRef);
    }
    if (itemType == "abilityItem")
    {
        return GiveAbilityItem(appContext, worldContext, commandArgs, onMessageRef);
    }
    if (itemType == "materialItem")
    {
        return GiveMaterialItem(appContext, worldContext, commandArgs, onMessageRef);
    }
    onMessageRef(appContext->GetLangsResources()->unknownCommandParameters);
    return false;
}

const std::string& glimmer::GiveCommand::GetName() const
{
    return GIVE_COMMAND_NAME;
}

bool glimmer::GiveCommand::RequiresWorldContext() const
{
    return true;
}
