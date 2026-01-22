//
// Created by Cold-Mint on 2025/12/17.
//

#ifndef GLIMMERWORKS_TILEITEM_H
#define GLIMMERWORKS_TILEITEM_H
#include "Item.h"

#include "../world/Tile.h"

namespace glimmer {
    class TileItem : public Item {
        std::unique_ptr<Tile> tile_;

    public:
        ~TileItem() override = default;


        explicit TileItem(std::unique_ptr<Tile> tile) : tile_(std::move(tile)) {
            maxStack_ = ITEM_MAX_STACK;
        }

        [[nodiscard]] std::string GetId() const override;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] std::string GetDescription() const override;

        void OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) override;

        [[nodiscard]] std::shared_ptr<SDL_Texture> GetIcon() const override;

        [[nodiscard]] std::optional<ResourceRef> ActualToResourceRef() override;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}

#endif //GLIMMERWORKS_TILEITEM_H
