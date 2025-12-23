//
// Created by coldmint on 2025/12/23.
//

#ifndef GLIMMERWORKS_TILEDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_TILEDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"
#include "../../mod/dataPack/TileManager.h"

namespace glimmer {
    class TileDynamicSuggestions final : public DynamicSuggestions {
        TileManager *tileManager_;

    public:
        explicit TileDynamicSuggestions(TileManager *tileManager) {
            tileManager_ = tileManager;
        }

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}

#endif //GLIMMERWORKS_TILEDYNAMICSUGGESTIONS_H
