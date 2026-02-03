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
        std::string description_;
        std::shared_ptr<SDL_Texture> icon_;
        std::unique_ptr<ItemAbility> itemAbility_;
        bool canUseAlone_;

        [[nodiscard]] std::optional<ResourceRef> ActualToResourceRef() override;

    public:
        explicit AbilityItem(std::string id, std::string name, std::string description,
                             std::shared_ptr<SDL_Texture> icon,
                             std::unique_ptr<ItemAbility> itemAbility, bool canUseAlone);

        [[nodiscard]] std::string GetId() const override;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] std::string GetDescription() const override;

        [[nodiscard]] std::shared_ptr<SDL_Texture> GetIcon() const override;

        [[nodiscard]] ItemAbility *GetItemAbility() const;

        static std::unique_ptr<AbilityItem> FromItemResource(AppContext *appContext,
                                                             const AbilityItemResource *itemResource,
                                                             const ResourceRef &resourceRef);

        void OnUse(WorldContext *worldContext, GameEntity::ID user) override;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}

#endif //GLIMMERWORKS_ABILITYITEM_H
