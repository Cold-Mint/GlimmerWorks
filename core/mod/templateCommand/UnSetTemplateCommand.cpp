//
// Created by coldmint on 2026/4/17.
//

#include "UnSetTemplateCommand.h"


std::optional<std::string> glimmer::UnSetTemplateCommand::Execute(const std::vector<std::string> &templateSearchPath,
                                                                  std::unordered_map<std::string, std::string> &
                                                                  variable, std::vector<std::string> &args,
                                                                  const VirtualFileSystem *virtualFileSystem) {
    if (args.size() == 1) {
        variable.erase(args[0]);
    }
    return std::nullopt;
}

const std::string_view &glimmer::UnSetTemplateCommand::GetCommandName() const {
    return unset;
}
