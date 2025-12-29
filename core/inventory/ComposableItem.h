//
// Created by coldmint on 2025/12/23.
//

#ifndef GLIMMERWORKS_COMPOSABLEITEM_H
#define GLIMMERWORKS_COMPOSABLEITEM_H
#include <utility>
#include <vector>

#include "Item.h"
#include "../log/LogCat.h"
#include "../mod/Resource.h"
#include "../scene/AppContext.h"
#include "../../core/mod/ResourceLocator.h"
#include "ability/ItemAbility.h"

namespace glimmer {
    /**
     * ComposableItem
     * 可组合的物品
     */
    class ComposableItem : public Item {
        std::vector<ItemAbility *> itemAbilityList_;
        std::string id_;
        std::string name_;
        std::string description_;
        std::shared_ptr<SDL_Texture> icon_;
        size_t maxSlotSize_;

    public:
        explicit ComposableItem(std::string id, std::string name, std::string description,
                                std::shared_ptr<SDL_Texture> icon, size_t maxSize) : id_(std::move(id)),
            name_(std::move(name)),
            description_(std::move(description)),
            icon_(std::move(icon)), maxSlotSize_(maxSize) {
        }

        [[nodiscard]] std::string GetId() const override;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] std::string GetDescription() const override;

        [[nodiscard]] std::shared_ptr<SDL_Texture> GetIcon() const override;

        void AddItemAbility(ItemAbility * ability);

        void RemoveItemAbility(const ItemAbility *ability);

        void OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) override;

        void OnDrop() override;

        static std::unique_ptr<ComposableItem> FromItemResource(AppContext *appContext,
                                                                const ComposableItemResource *itemResource) {
            const auto nameRes = appContext->GetResourceLocator()->FindString(itemResource->name);
            if (!nameRes.has_value()) {
                LogCat::e("An error occurred when constructing composable items, and the name is empty.");
                return nullptr;
            }
            auto descriptionRes = appContext->GetResourceLocator()->FindString(itemResource->description);
            if (!descriptionRes.has_value()) {
                LogCat::e("An error occurred when constructing composable items, and the description is empty.");
                return nullptr;
            }
            auto texture = appContext->GetResourcePackManager()->LoadTextureFromFile(appContext, itemResource->texture);
            if (texture == nullptr) {
                LogCat::e("An error occurred when constructing composable items, and the texture is empty.");
                return nullptr;
            }
            return std::make_unique<ComposableItem>(Resource::GenerateId(*itemResource), nameRes.value()->value,
                                                    descriptionRes.value()->value, texture, itemResource->slotSize);
        }

        [[nodiscard]] size_t GetMaxSlotSize() const;

        [[nodiscard]] const std::vector<ItemAbility *> &GetAbilityList() const;
    };
}

#endif //GLIMMERWORKS_COMPOSABLEITEM_H
