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
#include "LootCommand.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "fmt/format.h"

void glimmer::LootCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("get")->AddChild(LOOT_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::LootCommand::LootCommand(AppContext* appContext)
    : Command(appContext)
{
}

const std::string& glimmer::LootCommand::GetName() const
{
    return LOOT_COMMAND_NAME;
}

void glimmer::LootCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[type:string]");
    strings->emplace_back("[lootTableId:string]");
}

bool glimmer::LootCommand::RequiresWorldContext() const
{
    return true;
}

bool glimmer::LootCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
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
    const size_t size = commandArgs->GetSize();
    if (size < 3)
    {
        onMessageRef(fmt::format(
            fmt::runtime(appContext->GetLangsResources()->insufficientParameterLength),
            3, size));
        return false;
    }
    const std::string type = commandArgs->AsString(1);
    if (type == "get")
    {
        EntityShortCut* entityShortCut = worldContext->GetEntityShortCut();
        if (entityShortCut == nullptr)
        {
            return false;
        }
        EntityManager* entityManager = worldContext->GetEntityManager();
        if (entityManager == nullptr)
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
            onMessageRef(appContext->GetLangsResources()->itemContainerIsNull);
            return false;
        }
        auto lootId = commandArgs->AsResourceRef(2, RESOURCE_LOOT_TABLE);
        if (!lootId.has_value())
        {
            onMessageRef(appContext->GetLangsResources()->lootTableNotFound);
            return false;
        }
        ResourceRef& resourceRef = lootId.value();
        LootResource* lootResource = appContext->GetResourceLocator()->FindLoot(&resourceRef);
        if (lootResource == nullptr)
        {
            return false;
        }
        auto itemMessageList = LootResource::GetLootItems(lootResource);
        if (itemMessageList.empty())
        {
            return false;
        }
        for (auto& itemMessage : itemMessageList)
        {
            auto itemRes = appContext->GetResourceLocator()->FindItem(worldContext, itemMessage);
            if (itemRes != nullptr)
            {
                itemRes->ReadItemMessage(worldContext, itemMessage);
                std::unique_ptr<Item> item = itemContainer->GetItemContainer()->AddItem(
                    std::move(itemRes));
            }
        }
        return true;
    }
    return false;
}
