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
#include "TimeCommand.h"

#include "fmt/xchar.h"
#include "core/config/Constants.h"
#include "core/utils/LangsResources.h"
#include "core/world/WorldContext.h"

glimmer::TimeCommand::TimeCommand(AppContext *appContext) : Command(appContext) {
}

bool glimmer::TimeCommand::RequiresWorldContext() const {
    return true;
}

const std::string &glimmer::TimeCommand::GetName() const {
    return TIME_COMMAND_NAME;
}

void glimmer::TimeCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild("set");
}

void glimmer::TimeCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (commandArgs == nullptr || strings == nullptr) {
        return;
    }
    if (commandArgs->GetSize() >= 2 && commandArgs->AsString(1) == "set") {
        strings->emplace_back("[hour:float]");
    } else {
        strings->emplace_back("[set]");
    }
}

namespace {
    void FormatTime(float hour, int &outHour, int &outMinute) {
        const int h = static_cast<int>(hour) % 24;
        const int m = static_cast<int>((hour - static_cast<float>(static_cast<int>(hour))) * 60.0F);
        outHour = h;
        outMinute = m;
    }
}

bool glimmer::TimeCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
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
        int hour = 0;
        int minute = 0;
        FormatTime(worldContext->GetTimeOfDay(), hour, minute);
        onMessageRef(fmt::format(fmt::runtime(langsResources->timeInfo), hour, minute));
        return true;
    }

    const std::string operation = commandArgs->AsString(1);
    if (operation == "set") {
        if (size < 3) {
            onMessageRef(fmt::format(fmt::runtime(langsResources->insufficientParameterLength), 3, size));
            return false;
        }
        float hour = 0.0F;
        try {
            hour = commandArgs->AsFloat(2);
        } catch (const std::exception &) {
            onMessageRef(langsResources->timeInvalid);
            return false;
        }
        if (hour < 0.0F || hour >= 24.0F) {
            onMessageRef(langsResources->timeInvalid);
            return false;
        }
        worldContext->SetTimeOfDay(hour);
        int outHour = 0;
        int outMinute = 0;
        FormatTime(hour, outHour, outMinute);
        onMessageRef(fmt::format(fmt::runtime(langsResources->timeSetSuccess), outHour, outMinute));
        return true;
    }

    onMessageRef(langsResources->unknownCommandParameters);
    return false;
}
