//
// Created by Cold-Mint on 2026/3/3.
//

#pragma once
#include "DynamicSuggestions.h"
#include "core/mod/dataPack/MobManager.h"

namespace glimmer {
    class MobDynamicSuggestions final : public DynamicSuggestions {
        MobManager *mobManager_;

    public:
        explicit MobDynamicSuggestions(MobManager *mobManager);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}
