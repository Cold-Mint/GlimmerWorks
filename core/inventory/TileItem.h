//
// Created by Cold-Mint on 2025/12/17.
//

#ifndef GLIMMERWORKS_TILEITEM_H
#define GLIMMERWORKS_TILEITEM_H
#include "Item.h"
#include "../world/Tile.h"

namespace glimmer {
    class TileItem : public Item {
    public:
        ~TileItem() override = default;

    private:
        Tile tile_;

        [[nodiscard]] std::string GetId() const override;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] std::string GetDescription() const override;

        void OnUse() override;

        [[nodiscard]] std::shared_ptr<SDL_Texture> GetIcon() const override;

        void OnDrop() override;
    };
}

#endif //GLIMMERWORKS_TILEITEM_H
