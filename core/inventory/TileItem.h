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


        explicit TileItem(std::unique_ptr<Tile> tile);

        [[nodiscard]] std::string GetId() const override;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] std::optional<std::string> GetDescription() const override;

        void OnUse(WorldContext *worldContext, GameEntity::ID user, const VariableConfig &abilityData,
                   std::unordered_set<std::string> &popupAbility) override;

        [[nodiscard]] std::shared_ptr<SDL_Texture> GetIcon() const override;

        [[nodiscard]] std::optional<ResourceRef> ActualToResourceRef() override;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}

#endif //GLIMMERWORKS_TILEITEM_H
