//
// Created by Cold-Mint on 2025/12/23.
//

#pragma once
#include "DynamicSuggestions.h"
#include "core/mod/dataPack/TileResourceManager.h"

namespace glimmer {
    class TileDynamicSuggestions final : public DynamicSuggestions {
        TileResourceManager *tileManager_;

    public:
        explicit TileDynamicSuggestions(TileResourceManager *tileManager);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}
