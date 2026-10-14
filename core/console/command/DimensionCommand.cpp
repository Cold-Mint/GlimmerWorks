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

#include "core/context/AppContext.h"
#include "core/utils/LangsResources.h"
#include "core/world/WorldContext.h"
#include "fmt/format.h"

void glimmer::DimensionCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild(DIMENSION_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::DimensionCommand::DimensionCommand(AppContext *appContext) : Command(appContext) {
}

const std::string &glimmer::DimensionCommand::GetName() const {
    return DIMENSION_COMMAND_NAME;
}

bool glimmer::DimensionCommand::RequiresWorldContext() const {
    return true;
}

bool glimmer::DimensionCommand::RequiresCheatEnabled() const {
    return true;
}

void glimmer::DimensionCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (strings == nullptr) {
        return;
    }
    strings->emplace_back("[dimensionId:string]");
}

bool glimmer::DimensionCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                        const std::function<void(const std::string &text)> *onMessage) {
    const AppContext *appContext = GetAppContext();
    WorldContext *worldContext = GetWorldContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    const LangsResources *langsResources = appContext->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    if (worldContext == nullptr) {
        onMessageRef(langsResources->worldContextIsNull);
        return false;
    }
    const int size = commandArgs->GetSize();
    if (size < 2) {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    const auto dimensionRefOptional = commandArgs->AsResourceRef(1, RESOURCE_DIMENSION);
    if (!dimensionRefOptional.has_value()) {
        return false;
    }
    const ResourceRef &dimensionRef = dimensionRefOptional.value();
    if (!worldContext->SwitchDimension(dimensionRef)) {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->dimensionNotFound), dimensionRef.GetResourceKey()));
        return false;
    }
    onMessageRef(fmt::format(
        fmt::runtime(langsResources->dimensionSwitchSuccess), dimensionRef.GetResourceKey()));
    return true;
}
