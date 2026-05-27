//
// Created by coldmint on 2026/5/27.
//

#pragma once
#include "Item.h"

namespace glimmer
{
    class MaterialItem : public Item
    {
        std::string id_;
        std::string name_;
        std::optional<std::string> description_;
        std::shared_ptr<SDL_Texture> icon_;

    public:
        explicit MaterialItem(std::string id, std::string name, std::optional<std::string> description,
                              std::shared_ptr<SDL_Texture> icon,
                              const ResourceRef& resourceRef);
        [[nodiscard]] const std::string& GetId() const override;
        [[nodiscard]] const std::string& GetName() const override;
        [[nodiscard]] const std::optional<std::string>& GetDescription() const override;
        [[nodiscard]] const AbilityConfig* GetAbilityConfig() const override;
        void Reduce(unsigned value) override;
        [[nodiscard]] SDL_Texture* GetIcon() const override;
        [[nodiscard]] uint32_t GetMaxDurability() const override;
        [[nodiscard]] bool IsUnbreakable() const override;
        void OnUse(WorldContext* worldContext, GameEntity::ID user, const AbilityConfig* abilityConfig,
                   std::unordered_set<std::string>& popupAbility) override;
        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}
