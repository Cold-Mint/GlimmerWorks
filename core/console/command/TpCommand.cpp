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
#include "TpCommand.h"

#include "core/Constants.h"
#include "core/ecs/component/RigidBody2DComponent.h"
#include "core/utils/Box2DUtils.h"
#include "core/world/WorldContext.h"
#include "box2d/box2d.h"
#include "fmt/color.h"

void glimmer::TpCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild(X_DYNAMIC_SUGGESTIONS_NAME)->AddChild(Y_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::TpCommand::TpCommand(AppContext* appContext) : Command(appContext)
{
}

const std::string& glimmer::TpCommand::GetName() const
{
    return TP_COMMAND_NAME;
}

void glimmer::TpCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[x:int]");
    strings->emplace_back("[y:int]");
}

bool glimmer::TpCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
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
    if (worldContext_ == nullptr)
    {
        onMessageRef(langsResources->worldContextIsNull);
        return false;
    }

    const size_t size = commandArgs->GetSize();
    if (size < 2)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    auto playerEntity = worldContext_->GetEntityShortCut()->GetPlayer();
    if (WorldContext::IsEmptyEntityId(playerEntity))
    {
        onMessageRef(langsResources->cantFindObject);
        return false;
    }
    const WorldVector2D commandSenderPosition = commandSender->GetPosition();
    auto rigidBody2DComponent = worldContext_->GetEntityManager()->GetComponent<RigidBody2DComponent>(playerEntity);
    if (rigidBody2DComponent && rigidBody2DComponent->IsReady())
    {
        b2Vec2 newPos = Box2DUtils::ToMeters({
            commandArgs->AsCoordinate(1, commandSenderPosition.x),
            commandArgs->AsCoordinate(2, commandSenderPosition.y)
        });
        b2Rot currentRot = b2Body_GetRotation(rigidBody2DComponent->GetBodyId());
        b2Body_SetTransform(rigidBody2DComponent->GetBodyId(), newPos, currentRot);
    }
    onMessageRef(fmt::format(
        fmt::runtime(langsResources->teleportEntity),
        playerEntity, commandArgs->AsString(1), commandArgs->AsString(2)));

    return true;
}

bool glimmer::TpCommand::RequiresWorldContext() const
{
    return true;
}
