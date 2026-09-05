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
#include "DimensionCommand.h"

#include "fmt/xchar.h"
#include "core/config/Constants.h"
#include "core/mod/ResourceRef.h"
#include "core/utils/LangsResources.h"
#include "core/world/WorldContext.h"

glimmer::DimensionCommand::DimensionCommand(AppContext *appContext) : Command(appContext) {
}

bool glimmer::DimensionCommand::RequiresWorldContext() const {
    return true;
}

bool glimmer::DimensionCommand::RequiresCheatEnabled() const {
    return true;
}

const std::string &glimmer::DimensionCommand::GetName() const {
    return DIMENSION_COMMAND_NAME;
}

void glimmer::DimensionCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild("switch")->AddChild(DIMENSION_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("get");
}

void glimmer::DimensionCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (commandArgs == nullptr || strings == nullptr) {
        return;
    }
    strings->emplace_back("[operation:string]");
    if (commandArgs->GetSize() >= 2 && commandArgs->AsString(1) == "switch") {
        strings->emplace_back("[dimensionId:string]");
    }
}

bool glimmer::DimensionCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                        const std::function<void(const std::string &text)> *onMessage) {
    const AppContext *appContext = GetAppContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        onMessageRef(appContext->GetLangsResources()->worldContextIsNull);
        return false;
    }
    const LangsResources *langsResources = appContext->GetLangsResources();
    const int size = commandArgs->GetSize();

    if (size < 2) {
        onMessageRef(fmt::format("Current dimension: {}", worldContext->GetCurrentDimensionId()));
        return true;
    }

    const std::string operation = commandArgs->AsString(1);
    if (operation == "get") {
        onMessageRef(fmt::format("Current dimension: {}", worldContext->GetCurrentDimensionId()));
        return true;
    }

    if (operation == "switch") {
        if (size < 3) {
            onMessageRef(fmt::format(fmt::runtime(langsResources->insufficientParameterLength), 3, size));
            return false;
        }
        const auto resourceRef = commandArgs->AsResourceRef(2, RESOURCE_DIMENSION);
        if (!resourceRef.has_value()) {
            onMessageRef(langsResources->unknownCommandParameters);
            return false;
        }
        worldContext->SwitchDimension(resourceRef.value());
        onMessageRef(fmt::format("Switched to dimension: {}", worldContext->GetCurrentDimensionId()));
        return true;
    }

    onMessageRef(langsResources->unknownCommandParameters);
    return false;
}
