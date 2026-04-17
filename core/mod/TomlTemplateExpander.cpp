#include "TomlTemplateExpander.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <sstream>
#include <algorithm>

namespace glimmer {
    void TomlTemplateExpander::Register(std::unique_ptr<ITemplateCommand> command) {
        if (command == nullptr)
            return;
        {
        }
        commandMap_[command->GetCommandName()] = std::move(command);
    }

    void TomlTemplateExpander::Reset() const {
        if (commandMap_.empty()) {
            return;
        }
        for (auto &commandMap: commandMap_) {
            commandMap.second->Reset();
        }
    }

    std::string TomlTemplateExpander::Expand(const std::vector<std::string> &templateSearchPath,
                                             const std::string &inputText,
                                             const VirtualFileSystem *virtualFileSystem) const {
        std::stringstream inputStringStream(inputText);
        std::stringstream output;
        std::string line;
        std::unordered_map<std::string, std::string> variables = {};
        while (std::getline(inputStringStream, line)) {
            if (line.starts_with("#@")) {
                std::string commandLine = line.substr(2);
                size_t openParen = commandLine.find('(');
                if (openParen == std::string::npos) {
                    output << line << '\n';
                    continue;
                }

                std::string cmdName = commandLine.substr(0, openParen);
                auto it = commandMap_.find(cmdName);
                if (it == commandMap_.end()) {
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
                auto result = cmd->Execute(templateSearchPath, variables, args, virtualFileSystem);
                if (result.has_value()) {
                    output << *result << '\n';
                }
            } else {
                output << line << '\n';
            }
        }

        return output.str();
    }
} // namespace glimmer
