//
// Created by Cold-Mint on 2025/12/28.
//

#pragma once
#include "Item.h"
#include "ability/ItemAbility.h"

namespace glimmer
{
    class AbilityItem : public Item
    {
        std::string id_;
        std::string name_;
        std::optional<std::string> description_;
        std::shared_ptr<SDL_Texture> icon_;
        std::shared_ptr<ItemAbility> itemAbility_;
        uint32_t maxDurability_;
        bool isUnbreakable_;
        bool canUseAlone_;

    public:
        explicit AbilityItem(std::string id, std::string name, std::optional<std::string> description,
                             std::shared_ptr<SDL_Texture> icon,
                             std::shared_ptr<ItemAbility> itemAbility, uint32_t maxDurability, bool isUnbreakable,
                             bool canUseAlone,
                             const ResourceRef& resourceRef);

        [[nodiscard]] const std::string& GetId() const override;

        [[nodiscard]] const std::string& GetName() const override;

        [[nodiscard]] const std::optional<std::string>& GetDescription() const override;

        [[nodiscard]] SDL_Texture* GetIcon() const override;

        [[nodiscard]] ItemAbility* GetItemAbility() const;

        static std::unique_ptr<AbilityItem> FromItemResource(const AppContext* appContext,
                                                             const AbilityItemResource* itemResource,
                                                             const ResourceRef& resourceRef);
        [[nodiscard]] uint32_t GetMaxDurability() const override;

        void Reduce(unsigned value) override;

        [[nodiscard]] bool IsUnbreakable() const override;

        [[nodiscard]] const AbilityConfig* GetAbilityConfig() const override;

        void OnUse(WorldContext* worldContext, GameEntity::ID user, const AbilityConfig* abilityConfig,
                   std::unordered_set<std::string>& popupAbility) override;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}
