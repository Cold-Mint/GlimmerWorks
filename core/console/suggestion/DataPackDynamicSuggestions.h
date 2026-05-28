//
// Created by Cold-Mint on 2026/5/21.
//

#pragma once
#include "DynamicSuggestions.h"
#include "core/mod/dataPack/DataPackManager.h"

namespace glimmer {
    class DataPackDynamicSuggestions final : public DynamicSuggestions {
        DataPackManager *dataPackManager_;

    public:
        explicit DataPackDynamicSuggestions(DataPackManager *dataPackManager);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}
