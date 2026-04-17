//
// Created by coldmint on 2026/4/17.
//

#ifndef GLIMMERWORKS_ITEMPLATECOMMAND_H
#define GLIMMERWORKS_ITEMPLATECOMMAND_H
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/vfs/VirtualFileSystem.h"


namespace glimmer {
    class ITemplateCommand {
    public:
        virtual ~ITemplateCommand();

        [[nodiscard]] virtual std::optional<std::string> Execute(const std::vector<std::string> &templateSearchPath,
                                                                 std::unordered_map<std::string, std::string> &variable,
                                                                 std::vector<std::string> &args,
                                                                 const VirtualFileSystem *virtualFileSystem) = 0;

        virtual void Reset();

        [[nodiscard]] virtual const std::string_view &GetCommandName() const = 0;
    };
}


#endif //GLIMMERWORKS_ITEMPLATECOMMAND_H
