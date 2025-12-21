//
// Created by Cold-Mint on 2025/12/17.
//

#ifndef GLIMMERWORKS_TILEITEM_H
#define GLIMMERWORKS_TILEITEM_H
#include "Item.h"

#include "../world/Tile.h"

namespace glimmer {
    class TileItem : public Item {
        Tile *tile_;
    public:
        ~TileItem() override = default;


        explicit TileItem(Tile *tile) : tile_(tile) {
        }

        [[nodiscard]] std::string GetId() const override;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] std::string GetDescription() const override;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;

        void OnUse() override;

        [[nodiscard]] std::shared_ptr<SDL_Texture> GetIcon() const override;

        void OnDrop() override;
    };
}

#endif //GLIMMERWORKS_TILEITEM_H
