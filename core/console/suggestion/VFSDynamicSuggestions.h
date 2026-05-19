//
// Created by Cold-Mint on 2025/11/28.
//

#pragma once
#include "DynamicSuggestions.h"
#include "core/vfs/VirtualFileSystem.h"

namespace glimmer {
    class VFSDynamicSuggestions final : public DynamicSuggestions {
        VirtualFileSystem *virtualFileSystem_;

    public:
        explicit VFSDynamicSuggestions(VirtualFileSystem *virtualFileSystem);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}
