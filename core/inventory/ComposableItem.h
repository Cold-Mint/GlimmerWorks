//
// Created by Cold-Mint on 2025/12/23.
//

#pragma once

#include "Item.h"
#include "ItemContainer.h"

namespace glimmer
{
    class AbilityItem;
    /**
     * ComposableItem
     * 可组合的物品
     */
    class ComposableItem : public Item
    {
        std::shared_ptr<ItemContainer> itemContainer_;
        std::string id_;
        std::string name_;
        std::optional<std::string> description_;
        std::shared_ptr<SDL_Texture> icon_;
        AbilityConfig totalAbilityConfig_;
        size_t maxSlotSize_;
        std::shared_ptr<std::function<void(size_t, Item*, ContainerChangeType)>> callback_;


        void AddCallback();

    public:
        explicit ComposableItem(std::string id, std::string name, std::optional<std::string> description,
                                std::shared_ptr<SDL_Texture> icon, size_t maxSize, const ResourceRef& resourceRef);

        void ReadItemMessage(WorldContext* worldContext, const ItemMessage& itemMessage) override;

        void WriteItemMessage(ItemMessage& itemMessage) const override;

        ~ComposableItem() override;

        [[nodiscard]] const std::string& GetId() const override;

        [[nodiscard]] const std::string& GetName() const override;

        [[nodiscard]] const std::optional<std::string>& GetDescription() const override;

        [[nodiscard]] SDL_Texture* GetIcon() const override;

        void SwapItem(size_t index,
                      ItemContainer* otherContainer,
                      size_t otherIndex) const;

        void RefreshAttributes();

        [[nodiscard]] std::unique_ptr<Item> ReplaceItem(size_t index, std::unique_ptr<Item> item) const;

        [[nodiscard]] size_t RemoveItemAbility(const std::string& id, size_t amount) const;


        static std::unique_ptr<ComposableItem> FromItemResource(WorldContext* worldContext,
                                                                const ComposableItemResource* itemResource,
                                                                const ResourceRef& resourceRef);
        [[nodiscard]] const AbilityConfig* GetAbilityConfig() const override;

        void OnUse(WorldContext* worldContext, GameEntity::ID user, const AbilityConfig* abilityConfig,
                   std::unordered_set<std::string>& popupAbility) override;
        [[nodiscard]] ItemContainer* GetItemContainer() const;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}
