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
#include "TomlTemplateExpander.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <sstream>
#include <algorithm>
#include <assert.h>
#include <ranges>

#include "core/log/LogCat.h"


void glimmer::TomlTemplateExpander::Register(std::unique_ptr<ITemplateCommand> command)
{
    if (command == nullptr)
        return;
    {
    }
    commandMap_[command->GetCommandName()] = std::move(command);
}

void glimmer::TomlTemplateExpander::Reset() const
{
    if (commandMap_.empty())
    {
        return;
    }
    for (const auto& command : commandMap_ | std::views::values)
    {
        command->Reset();
    }
}

std::string glimmer::TomlTemplateExpander::Expand(const std::vector<std::string>& templateSearchPath,
                                                  const std::string& inputText,
                                                  const VirtualFileSystem* virtualFileSystem) const
{
    if (templateSearchPath.empty())
    {
        return inputText;
    }
    std::stringstream inputStringStream(inputText);
    std::stringstream output;
    std::string line;
    std::unordered_map<std::string, std::string, TransparentStringHash, std::equal_to<>> variables = {};
    while (std::getline(inputStringStream, line))
    {
        if (line.starts_with("#@"))
        {
            std::string commandLine = line.substr(2);
            size_t openParen = commandLine.find('(');
            if (openParen == std::string::npos)
            {
                output << line << '\n';
                continue;
            }

            std::string cmdName = commandLine.substr(0, openParen);
            auto it = commandMap_.find(cmdName);
            if (it == commandMap_.end())
            {
                continue;
            }
            ITemplateCommand* cmd = it->second.get();
            size_t closeParen = commandLine.find(')', openParen);
            std::string argsStr;
            if (closeParen != std::string::npos)
            {
                argsStr = commandLine.substr(openParen + 1, closeParen - openParen - 1);
            }
            std::vector<std::string> args;
            std::istringstream argsIss(argsStr);
            std::string arg;
            while (std::getline(argsIss, arg, ','))
            {
                args.push_back(arg);
            }
            auto result = cmd->Execute(templateSearchPath, variables, args, virtualFileSystem);
            if (result.has_value())
            {
                output << *result << '\n';
            }
        }
        else
        {
            output << line << '\n';
        }
    }
#if  defined(NDEBUG)
    return output.str();
#else
    std::string str = output.str();
    if (str.contains('{') && str.contains('}'))
    {
        LogCat::d(str);
        LogCat::e("Try to return the unprocessed characters.");
        assert(false);
    }
    return str;
#endif
}
