//
// Created by coldmint on 2026/4/17.
//

#include "InsertTemplateCommand.h"

#include "absl/strings/internal/cord_internal.h"
#include "core/Constants.h"
#include "core/utils/StringUtils.h"


std::optional<std::string> glimmer::InsertTemplateCommand::Execute(const std::vector<std::string> &templateSearchPath,
                                                                   std::unordered_map<std::string, std::string> &
                                                                   variable, std::vector<std::string> &args,
                                                                   const VirtualFileSystem *virtualFileSystem) {
    if (args.size() == 1) {
        if (templateSearchPath.empty()) {
            return std::nullopt;
        }
        for (auto &searchPath: templateSearchPath) {
            const std::string path = searchPath + "/" + args[0] + "." + DATA_FILE_TYPE_TEMPLATE + ".toml";
            if (!virtualFileSystem->Exists(path)) {
                continue;
            }
            std::optional<std::string> fileDataOptional = virtualFileSystem->ReadFile(path);
            if (!fileDataOptional.has_value()) {
                continue;
            }
            std::string &processedContent = fileDataOptional.value();
            for (const auto &[key, value]: variable) {
                StringUtils::ReplaceAll(processedContent, "{" + key + "}", value);
            }
            return processedContent;
        }
    }

    return std::nullopt;
}

const std::string_view &glimmer::InsertTemplateCommand::GetCommandName() const {
    return insert;
}
