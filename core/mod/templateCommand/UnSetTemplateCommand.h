//
// Created by Cold-Mint on 2026/4/17.
//

#pragma once
#include "core/mod/ITemplateCommand.h"


namespace glimmer {
    class UnSetTemplateCommand : public ITemplateCommand {
        const std::string_view unset = "unset";

    public:
        [[nodiscard]] std::optional<std::string> Execute(const std::vector<std::string> &templateSearchPath,
                                                         std::unordered_map<std::string, std::string> &variable,
                                                         std::vector<std::string> &args,
                                                         const VirtualFileSystem *virtualFileSystem) override;

        [[nodiscard]] const std::string_view &GetCommandName() const override;
    };
}
