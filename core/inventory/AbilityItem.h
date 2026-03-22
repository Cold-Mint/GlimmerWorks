//
// Created by coldmint on 2025/12/28.
//

#ifndef GLIMMERWORKS_ABILITYITEM_H
#define GLIMMERWORKS_ABILITYITEM_H
#include "Item.h"
#include "ability/ItemAbility.h"

namespace glimmer {
    class AbilityItem : public Item {
        std::string id_;
        std::string name_;
        std::optional<std::string> description_;
        std::shared_ptr<SDL_Texture> icon_;
        std::shared_ptr<ItemAbility> itemAbility_;
        bool canUseAlone_;

    public:
        explicit AbilityItem(std::string id, std::string name, std::optional<std::string> description,
                             std::shared_ptr<SDL_Texture> icon,
                             std::shared_ptr<ItemAbility> itemAbility, bool canUseAlone);

        [[nodiscard]] const std::string &GetId() const override;

        [[nodiscard]] const std::string &GetName() const override;

        [[nodiscard]] const std::optional<std::string> &GetDescription() const override;

        [[nodiscard]] SDL_Texture *GetIcon() const override;

        [[nodiscard]] ItemAbility *GetItemAbility() const;

        static std::unique_ptr<AbilityItem> FromItemResource(const AppContext *appContext,
                                                             const AbilityItemResource *itemResource,
                                                             const ResourceRef &resourceRef);

        [[nodiscard]] const VariableConfig &GetVariableConfig() const override;

        void OnUse(WorldContext *worldContext, GameEntity::ID user, const VariableConfig &abilityData,
                   std::unordered_set<std::string> &popupAbility) override;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}

#endif //GLIMMERWORKS_ABILITYITEM_H
