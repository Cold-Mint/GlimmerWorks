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
#include "ClearCommand.h"
#include "../../world/WorldContext.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/ecs/component/PlayerComponent.h"

void glimmer::ClearCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
}

glimmer::ClearCommand::ClearCommand(AppContext* appContext) : Command(appContext)
{
}

std::string glimmer::ClearCommand::GetName() const
{
    return CLEAR_COMMAND_NAME;
}

void glimmer::ClearCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
}


bool glimmer::ClearCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                    const std::function<void(const std::string& text)>* onMessage)
{
    if (appContext_ == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const LangsResources* langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }

    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    if (worldContext_ == nullptr)
    {
        onMessageRef(langsResources->worldContextIsNull);
        return false;
    }
    auto playerEntity = worldContext_->GetEntityShortCut()->GetPlayer();
    const auto* itemContainerComponent = worldContext_->GetEntityManager()->GetComponent<
        ItemContainerComponent>(playerEntity);
    if (itemContainerComponent == nullptr)
    {
        onMessageRef(langsResources->itemContainerIsNull);
        return false;
    }
    appContext_->PostToNextMainFrame([this, itemContainerComponent, playerEntity]
        {
            auto* playerComponent = worldContext_->GetEntityManager()->GetComponent<PlayerComponent>(playerEntity);
            if (playerComponent != nullptr)
            {
                playerComponent->item = nullptr;
            }

            auto* itemContainer = itemContainerComponent->GetItemContainer();
            if (itemContainer != nullptr)
            {
                itemContainer->ResetItems();
            }
        }
    );
    return true;
}

bool glimmer::ClearCommand::RequiresWorldContext() const
{
    return true;
}
