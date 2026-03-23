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
        AbilityConfig abilityConfig_;

    public:
        ~TileItem() override = default;


        explicit TileItem(std::unique_ptr<Tile> tile);

        [[nodiscard]] const std::string &GetId() const override;

        [[nodiscard]] const std::string &GetName() const override;

        [[nodiscard]] const std::optional<std::string> &GetDescription() const override;

        void OnUse(WorldContext *worldContext, GameEntity::ID user, const AbilityConfig &abilityConfig,
                   std::unordered_set<std::string> &popupAbility) override;

        [[nodiscard]] SDL_Texture *GetIcon() const override;

        [[nodiscard]] const AbilityConfig & GetAbilityConfig() const override;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}

#endif //GLIMMERWORKS_TILEITEM_H
