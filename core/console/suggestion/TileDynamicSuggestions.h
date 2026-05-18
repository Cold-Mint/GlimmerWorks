//
// Created by Cold-Mint on 2025/12/23.
//

#ifndef GLIMMERWORKS_TILEDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_TILEDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"
#include "../../mod/dataPack/TileResourceManager.h"

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

#endif //GLIMMERWORKS_TILEDYNAMICSUGGESTIONS_H
