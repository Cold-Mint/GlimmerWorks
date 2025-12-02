//
// Created by coldmint on 2025/12/2.
//

#ifndef GLIMMERWORKS_CONFIGSUGGESTION_H
#define GLIMMERWORKS_CONFIGSUGGESTION_H
#include <utility>

#include "DynamicSuggestions.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

namespace glimmer {
    class ConfigSuggestions final : public DynamicSuggestions {
        nlohmann::json json_;

        static void CollectKeys(const nlohmann::json &j, const std::string &prefix, std::vector<std::string> &out);
    public:
        explicit ConfigSuggestions(nlohmann::json json) : json_(std::move(json)) {
        }

        bool Match(std::string keyword, std::string param) override;

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;
    };
}

#endif //GLIMMERWORKS_CONFIGSUGGESTION_H
