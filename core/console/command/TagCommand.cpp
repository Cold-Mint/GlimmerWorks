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
#if  !defined(NDEBUG)
#include "TagCommand.h"

#include "core/LangsResources.h"
#include "core/ecs/EntityShortCut.h"
#include "core/mod/dataPack/StringManager.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "fmt/xchar.h"

void glimmer::TagCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("hand");
    suggestionsTree->AddChild("inventory");
}

void glimmer::TagCommand::WriteTag(const std::string& tagItem, std::stringstream& stringStream,
                                   StringManager* stringManager, const ItemTagResource& itemTagResource)
{
    auto tagTranslate = stringManager->GetTagTranslate(itemTagResource.GetCachedTagId());
    stringStream << fmt::format(fmt::runtime(tagItem), itemTagResource.name,
                                tagTranslate.value_or(itemTagResource.name),
                                itemTagResource.GetCachedTagId(), itemTagResource.value);
}

std::string glimmer::TagCommand::BuildTagListString(const std::string& tagItem, StringManager* stringManager,
                                                    const std::vector<const ItemTagResource*>& tagList)
{
    std::stringstream stringStream;
    bool first = true;
    for (auto tag : tagList)
    {
        if (tag == nullptr)
        {
            continue;
        }
        if (first)
        {
            first = false;
        }
        else
        {
            stringStream << '\n';
        }
        WriteTag(tagItem, stringStream, stringManager, *tag);
    }
    return stringStream.str();
}


glimmer::TagCommand::TagCommand(AppContext* appContext) : Command(appContext)
{
}

bool glimmer::TagCommand::RequiresWorldContext() const
{
    return true;
}

const std::string& glimmer::TagCommand::GetName() const
{
    return TAG_COMMAND_NAME;
}

void glimmer::TagCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[type:string]");
}

bool glimmer::TagCommand::ExecuteHand([[maybe_unused]] const CommandSender* commandSender,
                                      const WorldContext* worldContext,
                                      const std::function<void(const std::string& text)>& onMessageRef,
                                      const AppContext* appContext, const LangsResources* langsResources)
{
    const EntityShortCut* entityShortCut = worldContext->GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        return false;
    }
    EntityManager* entityManager = worldContext->GetEntityManager();
    if (entityManager == nullptr)
    {
        return false;
    }
    auto plyerEntity = entityShortCut->GetPlayer();
    if (WorldContext::IsEmptyEntityId(plyerEntity))
    {
        return false;
    }
    auto playerComponent = entityManager->GetComponent<PlayerComponent>(plyerEntity);
    if (playerComponent == nullptr)
    {
        return false;
    }
    auto item = playerComponent->GetItem();
    if (item == nullptr)
    {
        return false;
    }
    StringManager* stringManager = appContext->GetStringManager();
    const std::vector<uint64_t>& tagList = item->GetTags();
    if (tagList.empty())
    {
        onMessageRef(langsResources->tagCannotFound);
        return true;
    }
    std::vector<const ItemTagResource*> itemTagResourceList;
    for (uint64_t tag : tagList)
    {
        const ItemTagResource* itemTagResource = item->GetItemTagResource(tag);
        if (itemTagResource == nullptr)
        {
            continue;
        }
        itemTagResourceList.emplace_back(itemTagResource);
    }
    onMessageRef(BuildTagListString(langsResources->tagItem, stringManager, itemTagResourceList));
    return true;
}

bool glimmer::TagCommand::ExecuteInventory([[maybe_unused]] const CommandSender* commandSender,
                                           const WorldContext* worldContext,
                                           const std::function<void(const std::string& text)>& onMessageRef,
                                           const AppContext* appContext, const LangsResources* langsResources)
{
    const EntityShortCut* entityShortCut = worldContext->GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        return false;
    }
    const ItemContainerComponent* itemContainerComponent = entityShortCut->GetItemContainerComponent();
    if (itemContainerComponent == nullptr)
    {
        return false;
    }
    ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        return false;
    }
    const std::vector<const ItemTagResource*>& totalTags = itemContainer->GetTotalTags();
    StringManager* stringManager = appContext->GetStringManager();
    if (totalTags.empty())
    {
        onMessageRef(langsResources->tagCannotFound);
        return true;
    }
    onMessageRef(BuildTagListString(langsResources->tagItem, stringManager, totalTags));
    return true;
}

bool glimmer::TagCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                  const std::function<void(const std::string& text)>* onMessage)
{
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    const LangsResources* langsResources = appContext->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    if (int size = commandArgs->GetSize(); size < 2)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    const WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        onMessageRef(appContext->GetLangsResources()->worldContextIsNull);
        return false;
    }
    const std::string type = commandArgs->AsString(1);
    if (type == "hand")
    {
        return ExecuteHand(commandSender, worldContext, onMessageRef, appContext, langsResources);
    }
    if (type == "inventory")
    {
        return ExecuteInventory(commandSender, worldContext, onMessageRef, appContext, langsResources);
    }
    return false;
}
#endif
