//
// Created by coldmint on 2025/12/23.
//

#ifndef GLIMMERWORKS_COMPOSABLEITEM_H
#define GLIMMERWORKS_COMPOSABLEITEM_H

#include "Item.h"
#include "ItemContainer.h"

namespace glimmer {
    class AbilityItem;
    /**
     * ComposableItem
     * 可组合的物品
     */
    class ComposableItem : public Item {
        std::unique_ptr<ItemContainer> itemContainer_;
        std::string id_;
        std::string name_;
        std::string description_;
        std::shared_ptr<SDL_Texture> icon_;
        size_t maxSlotSize_;

        [[nodiscard]] std::optional<ResourceRef> ActualToResourceRef() override;

    public:
        explicit ComposableItem(std::string id, std::string name, std::string description,
                                std::shared_ptr<SDL_Texture> icon, size_t maxSize);

        [[nodiscard]] std::string GetId() const override;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] std::string GetDescription() const override;

        [[nodiscard]] std::shared_ptr<SDL_Texture> GetIcon() const override;

        void SwapItem(size_t index,
                      ItemContainer *otherContainer,
                      size_t otherIndex) const;

        [[nodiscard]] std::unique_ptr<Item> ReplaceItem(size_t index, std::unique_ptr<Item> item) const;

        [[nodiscard]] size_t RemoveItemAbility(const std::string &id, size_t amount) const;

        void OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity::ID user) override;

        /**
         * TryParseItemIndex
         * 尝试从名称内解析下标
         * @param name
         * @return If the result is -1, it indicates that the parsing has failed. 如果返回-1,那么表示解析失败。
         */
        static int TryParseItemIndex(const std::string &name);

        static std::unique_ptr<ComposableItem> FromItemResource(AppContext *appContext,
                                                                const ComposableItemResource *itemResource,
                                                                const ResourceRef &resourceRef);

        [[nodiscard]] ItemContainer *GetItemContainer() const;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}

#endif //GLIMMERWORKS_COMPOSABLEITEM_H
