//
// Created by Cold-Mint on 2025/12/17.
//

#pragma once
#include "Item.h"

#include "core/world/Tile.h"

namespace glimmer {
    class TileItem : public Item {
        std::shared_ptr<Tile> tile_;
        AbilityConfig abilityConfig_;

    public:
        ~TileItem() override = default;

        explicit TileItem(const std::shared_ptr<Tile> &tile, const ResourceRef &resourceRef);

        [[nodiscard]] const std::string &GetId() const override;

        [[nodiscard]] const std::string &GetName() const override;

        [[nodiscard]] const std::optional<std::string> &GetDescription() const override;

        [[nodiscard]] const Tile *GetTile() const;

        void OnUse(WorldContext *worldContext, GameEntity::ID user, const AbilityConfig &abilityConfig,
                   std::unordered_set<std::string> &popupAbility) override;

        [[nodiscard]] SDL_Texture *GetIcon() const override;

        [[nodiscard]] const AbilityConfig &GetAbilityConfig() const override;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}
