//
// Created by coldmint on 2025/11/28.
//

#ifndef GLIMMERWORKS_VFSDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_VFSDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"
#include "../../vfs/VirtualFileSystem.h"

namespace glimmer {
    class VFSDynamicSuggestions final : public DynamicSuggestions {
        VirtualFileSystem *virtualFileSystem_;

    public:
        explicit VFSDynamicSuggestions(VirtualFileSystem *virtualFileSystem) : virtualFileSystem_(virtualFileSystem) {
        }

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}

#endif //GLIMMERWORKS_VFSDYNAMICSUGGESTIONS_H
