//
// Created by coldmint on 2026/4/17.
//

#ifndef GLIMMERWORKS_INSERTTEMPLATECOMMAND_H
#define GLIMMERWORKS_INSERTTEMPLATECOMMAND_H
#include "core/mod/ITemplateCommand.h"


namespace glimmer {
    class InsertTemplateCommand : public ITemplateCommand {
        const std::string_view insert = "insert_template";

    public:
        [[nodiscard]] std::optional<std::string> Execute(const std::vector<std::string> &templateSearchPath,
            std::unordered_map<std::string, std::string> &variable, std::vector<std::string> &args,
            const VirtualFileSystem *virtualFileSystem) override;

        [[nodiscard]] const std::string_view &GetCommandName() const override;
    };
}


#endif //GLIMMERWORKS_INSERTTEMPLATECOMMAND_H
