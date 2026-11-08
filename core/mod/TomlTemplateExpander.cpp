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

#include <ranges>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <queue>
#include <stack>

#include "core/log/LogCat.h"
#include "core/utils/StringUtils.h"


void glimmer::TomlTemplateExpander::Register(std::unique_ptr<ITemplateCommand> command) {
    if (command == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "toml_template_command_null",
                  "Attempted to register a null template command");
        return;
    }
    commandMap_[command->GetCommandName()] = std::move(command);
}

std::string glimmer::TomlTemplateExpander::Expand(const std::vector<std::filesystem::path> &templateSearchPath,
                                                  std::stringstream &inputStringStream,
                                                  const VirtualFileSystem *virtualFileSystem) const {
    if (templateSearchPath.empty()) {
        return inputStringStream.str();
    }

    std::stack<std::unordered_map<std::string, std::string, TransparentStringHash, std::equal_to<> > > variablesStack =
            {};
    //条件栈，用于ifdef和ifndef
    std::stack<bool> conditionStack;
    //Construct the bottom element of the stack.
    //构造栈底元素。
    variablesStack.emplace();
    std::string line;
    std::deque<std::string> lines;
    while (std::getline(inputStringStream, line)) {
        lines.push_back(line);
    }
    std::stringstream output;
    bool firstWrite = true;
    std::stack<std::string> newLines;
    while (!lines.empty()) {
        line = lines.front();
        lines.pop_front();
        if (line.starts_with("#@")) {
            std::string commandLine = line.substr(2);
            size_t openParen = commandLine.find('(');
            if (openParen == std::string::npos) {
                if (firstWrite) {
                    firstWrite = false;
                } else {
                    output << '\n';
                }
                output << line;
                continue;
            }

            std::string cmdName = commandLine.substr(0, openParen);
            auto it = commandMap_.find(cmdName);
            if (it == commandMap_.end()) {
                LogCat::d(LogLabel::DEFAULT, "toml_template_command_not_found", "Template command not found: {}",
                          cmdName);
                continue;
            }
            ITemplateCommand *cmd = it->second.get();
            size_t closeParen = commandLine.find(')', openParen);
            std::string argsStr;
            if (closeParen != std::string::npos) {
                argsStr = commandLine.substr(openParen + 1, closeParen - openParen - 1);
            }
            std::vector<std::string> args;
            std::istringstream argsIss(argsStr);
            std::string arg;
            while (std::getline(argsIss, arg, ',')) {
                args.push_back(arg);
            }
            if (auto stringStream = cmd->Execute(templateSearchPath, variablesStack, args, conditionStack,
                                                 virtualFileSystem);
                stringStream != nullptr) {
                //Pointer to the reset stream.
                //复位流的指针。
                stringStream->seekg(0);
                while (std::getline(*stringStream, line)) {
                    newLines.push(line); // 正序收集：a, b, c
                }
                while (!newLines.empty()) {
                    lines.push_front(newLines.top());
                    newLines.pop();
                }
            }
        } else {
            //Here, template substitution is carried out.
            //这里实现模板替换。
            if (conditionStack.empty()) {
                if (firstWrite) {
                    firstWrite = false;
                } else {
                    output << '\n';
                }
                output << StringUtils::RenderScan(line, variablesStack.top());
            } else {
                if (conditionStack.top()) {
                    if (firstWrite) {
                        firstWrite = false;
                    } else {
                        output << '\n';
                    }
                    output << StringUtils::RenderScan(line, variablesStack.top());
                }
            }
        }
    }

#if defined(NDEBUG)
    return output.str();
#else
    std::string str = output.str();
    if (str.contains('{') && str.contains('}')) {
        assert(false);
    }
    return str;
#endif
}
