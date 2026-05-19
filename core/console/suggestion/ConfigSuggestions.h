//
// Created by Cold-Mint on 2025/12/2.
//

#pragma once
#include "DynamicSuggestions.h"
#include "toml11/types.hpp"

namespace glimmer {
    class ConfigSuggestions final : public DynamicSuggestions {
        toml::value *configValue_;

        static void ParseTable(const toml::value::table_type &table,
                               std::vector<std::string> &fields,
                               const std::string &prefix = "");

    public:
        explicit ConfigSuggestions(toml::value *configValue);

        bool Match(std::string keyword, std::string param) override;

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;
    };
}
