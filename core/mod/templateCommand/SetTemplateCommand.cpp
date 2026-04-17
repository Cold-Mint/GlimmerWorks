//
// Created by coldmint on 2026/4/17.
//

#include "SetTemplateCommand.h"

std::optional<std::string> glimmer::SetTemplateCommand::Execute(const std::vector<std::string> &templateSearchPath,
                                                                std::unordered_map<std::string, std::string> &variable,
                                                                std::vector<std::string> &args,
                                                                const VirtualFileSystem *virtualFileSystem) {
    if (args.size() == 2) {
        const std::string &key = args[0];
        const std::string &value = args[1];
        variable[key] = value;
    }
    return std::nullopt;
}

const std::string_view &glimmer::SetTemplateCommand::GetCommandName() const {
    return set;
}
