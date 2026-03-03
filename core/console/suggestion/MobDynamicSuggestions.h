//
// Created by coldmint on 2026/3/3.
//

#ifndef GLIMMERWORKS_MOBDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_MOBDYNAMICSUGGESTIONS_H
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

#endif //GLIMMERWORKS_MOBDYNAMICSUGGESTIONS_H
