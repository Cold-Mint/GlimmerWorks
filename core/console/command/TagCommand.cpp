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
                                tagTranslate.has_value() ? tagTranslate.value() : itemTagResource.name,
                                itemTagResource.GetCachedTagId(), itemTagResource.value);
}


glimmer::TagCommand::TagCommand(AppContext* appContext) : Command(appContext)
{
}

bool glimmer::TagCommand::RequiresWorldContext() const
{
    return true;
}

std::string glimmer::TagCommand::GetName() const
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

bool glimmer::TagCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                  const std::function<void(const std::string& text)>* onMessage)
{
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    const LangsResources* langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    int size = commandArgs->GetSize();
    if (size < 2)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    const std::string type = commandArgs->AsString(1);
    if (type == "hand")
    {
        EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut();
        if (entityShortCut == nullptr)
        {
            return false;
        }
        EntityManager* entityManager = worldContext_->GetEntityManager();
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
        auto item = playerComponent->item;
        if (item == nullptr)
        {
            return false;
        }
        StringManager* stringManager = appContext_->GetStringManager();
        const std::vector<ItemTagResource>& tagList = item->GetTags();
        if (tagList.empty())
        {
            onMessageRef(langsResources->tagCannotFound);
        }
        else
        {
            std::stringstream stringStream;
            bool first = true;
            for (auto& tag : tagList)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    stringStream << '\n';
                }
                WriteTag(langsResources->tagItem, stringStream, stringManager, tag);
            }
            onMessageRef(stringStream.str());
        }
        return true;
    }
    if (type == "inventory")
    {
        EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut();
        if (entityShortCut == nullptr)
        {
            return false;
        }
        ItemContainerComponent* itemContainerComponent = entityShortCut->GetItemContainerComponent();
        if (itemContainerComponent == nullptr)
        {
            return false;
        }
        ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
        if (itemContainer == nullptr)
        {
            return false;
        }
        const std::vector<ItemTagResource*>& totalTags = itemContainer->GetTotalTags();
        StringManager* stringManager = appContext_->GetStringManager();
        if (totalTags.empty())
        {
            onMessageRef(langsResources->tagCannotFound);
        }
        else
        {
            std::stringstream stringStream;
            bool first = true;
            for (auto tag : totalTags)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    stringStream << '\n';
                }
                WriteTag(langsResources->tagItem, stringStream, stringManager, *tag);
            }
            onMessageRef(stringStream.str());
        }
        return true;
    }
    return false;
}
#endif
