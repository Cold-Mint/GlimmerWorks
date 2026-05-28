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
#include "ItemEditorCommand.h"

#include "core/LangsResources.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "fmt/xchar.h"

glimmer::ItemEditorCommand::ItemEditorCommand(AppContext* appContext)
    : Command(appContext)
{
}

bool glimmer::ItemEditorCommand::RequiresWorldContext() const
{
    return true;
}


void glimmer::ItemEditorCommand::AddSuggestionsValue(NodeTree<std::string>* suggestionsTree, bool setMode)
{
    suggestionsTree->AddChild("used_durability");
    auto durabilityStrategy = suggestionsTree->AddChild("durability_strategy");
    if (setMode)
    {
        durabilityStrategy->AddChild(ALLOC_STRATEGY_TYPE_DYNAMIC_SUGGESTIONS_NAME);
    }
    suggestionsTree->AddChild("amount");
}

void glimmer::ItemEditorCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    NodeTree<std::string>* getSuggestionsTree = suggestionsTree->AddChild("get");
    AddSuggestionsValue(getSuggestionsTree, false);
    getSuggestionsTree->AddChild("max_durability");
    getSuggestionsTree->AddChild("max_stack");
    AddSuggestionsValue(suggestionsTree->AddChild("set"), true);
}

std::string glimmer::ItemEditorCommand::GetName() const
{
    return ITEM_EDITOR_COMMAND_NAME;
}

void glimmer::ItemEditorCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (commandArgs == nullptr || strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[operation_type:string]");
    strings->emplace_back("[attribute:string]");
    int size = commandArgs->GetSize();
    if (size > 1)
    {
        const std::string operationType = commandArgs->AsString(1);
        if (operationType == "set")
        {
            if (size > 2)
            {
                const std::string attribute = commandArgs->AsString(2);
                if (attribute == "used_durability")
                {
                    strings->emplace_back("[number:uint]");
                }
                else if (attribute == "durability_strategy")
                {
                    strings->emplace_back("[type:string]");
                }
                else if (attribute == "amount")
                {
                    strings->emplace_back("[number:uint]");
                }
            }
        }
    }
}

bool glimmer::ItemEditorCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
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
    const LangsResources* langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    const int size = commandArgs->GetSize();
    if (size < 2)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    auto player = worldContext_->GetPlayerEntity();
    auto playerComponent = worldContext_->GetComponent<PlayerComponent>(player);
    if (playerComponent == nullptr)
    {
        onMessageRef(langsResources->playerDoesNotExist);
        return false;
    }
    auto item = playerComponent->item;
    if (item == nullptr)
    {
        onMessageRef(langsResources->itemEditorHoldItem);
        return false;
    }
    std::string operation = commandArgs->AsString(1);
    if (operation == "get")
    {
        std::stringstream value;
        const std::string attribute = commandArgs->AsString(2);
        if (attribute == "used_durability")
        {
            value << item->GetUsedDurability();
        }
        else if (attribute == "durability_strategy")
        {
            auto composableItem = dynamic_cast<ComposableItem*>(item);
            if (composableItem != nullptr)
            {
                switch (composableItem->GetAllocStrategyType())
                {
                case ALLOC_STRATEGY_FORWARD:
                    value << ALLOC_STR_STRATEGY_FORWARD;
                    break;
                case ALLOC_STRATEGY_BACKWARD:
                    value << ALLOC_STR_STRATEGY_BACKWARD;

                    break;
                case ALLOC_STRATEGY_BALANCE:
                    value << ALLOC_STR_STRATEGY_BALANCE;
                    break;
                case ALLOC_STRATEGY_RANDOM:
                    value << ALLOC_STR_STRATEGY_RANDOM;
                    break;
                case AllocStrategyTypeMessage_INT_MIN_SENTINEL_DO_NOT_USE_:
                    break;
                case AllocStrategyTypeMessage_INT_MAX_SENTINEL_DO_NOT_USE_:
                    break;
                }
            }
        }
        else if (attribute == "amount")
        {
            value << item->GetAmount();
        }
        else if (attribute == "max_durability")
        {
            value << item->GetMaxDurability();
        }
        else if (attribute == "max_stack")
        {
            value << item->GetMaxStack();
        }
        onMessageRef(fmt::format(fmt::runtime(langsResources->itemEditorReadAttr), item->GetName(), attribute,
                                 value.str()));
        return true;
    }
    if (operation == "set")
    {
        if (size < 3)
        {
            onMessageRef(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                3, size));
            return false;
        }
        std::string value = commandArgs->AsString(3);
        const std::string attribute = commandArgs->AsString(2);
        appContext_->PostToNextMainFrame([attribute, item, value]
        {
            if (attribute == "used_durability")
            {
                item->SetUsedDurability(std::stoi(value));
            }
            else if (attribute == "durability_strategy")
            {
                auto composableItem = dynamic_cast<ComposableItem*>(item);
                if (composableItem != nullptr)
                {
                    if (value == ALLOC_STR_STRATEGY_FORWARD)
                    {
                        composableItem->SetAllocStrategyType(ALLOC_STRATEGY_FORWARD);
                    }
                    else if (value == ALLOC_STR_STRATEGY_BACKWARD)
                    {
                        composableItem->SetAllocStrategyType(ALLOC_STRATEGY_BACKWARD);
                    }
                    else if (value == ALLOC_STR_STRATEGY_BALANCE)
                    {
                        composableItem->SetAllocStrategyType(ALLOC_STRATEGY_BALANCE);
                    }
                    else if (value == ALLOC_STR_STRATEGY_RANDOM)
                    {
                        composableItem->SetAllocStrategyType(ALLOC_STRATEGY_RANDOM);
                    }
                }
            }
            else if (attribute == "amount")
            {
                item->SetAmount(std::stoi(value));
            }
        });

        onMessageRef(fmt::format(fmt::runtime(langsResources->itemEditorSetAttr), item->GetName(), attribute,
                                 value));
        return true;
    }
    return false;
}
