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
#include "RmlDebugCommand.h"

#include "core/LangsResources.h"
#include "core/context/AppContext.h"
#include "fmt/xchar.h"
#include "RmlUi/Debugger/Debugger.h"
#if  !defined(NDEBUG)

void glimmer::RmlDebugCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    suggestionsTree->AddChild(BOOL_TOGGLE_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::RmlDebugCommand::RmlDebugCommand(AppContext* appContext) : Command(appContext)
{
}

const std::string& glimmer::RmlDebugCommand::GetName() const
{
    return RML_DEBUG_COMMAND_NAME;
}

bool glimmer::RmlDebugCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                       const std::function<void(const std::string& text)>* onMessage)
{
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    const LangsResources* langsResources = appContext->GetLangsResources();
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

    const BoolOrToggle value = commandArgs->AsBoolOrToggle(1);
    if (value == BoolOrToggle::TOGGLE)
    {
        Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
    }
    else
    {
        Rml::Debugger::SetVisible(value == BoolOrToggle::TRUE);
    }
    return true;
}
#endif
