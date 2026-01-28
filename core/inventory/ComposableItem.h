//
// Created by coldmint on 2025/12/23.
//

#ifndef GLIMMERWORKS_COMPOSABLEITEM_H
#define GLIMMERWORKS_COMPOSABLEITEM_H
#include <utility>
#include <vector>

#include "Item.h"
#include "ItemContainer.h"
#include "../log/LogCat.h"
#include "../mod/Resource.h"
#include "../scene/AppContext.h"
#include "../../core/mod/ResourceLocator.h"

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
                                std::shared_ptr<SDL_Texture> icon, size_t maxSize) : itemContainer_(
                std::make_unique<ItemContainer>(maxSize)),
            id_(std::move(id)),
            name_(std::move(name)),
            description_(std::move(description)), icon_(std::move(icon)), maxSlotSize_(maxSize) {
        }

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
        static int TryParseItemIndex(const std::string &name) {
            constexpr std::string_view prefix = "item_";
            if (!name.starts_with(prefix)) {
                return -1;
            }
            std::string_view numberPart{name};
            numberPart.remove_prefix(prefix.size());
            if (numberPart.empty()) {
                return -1;
            }
            int value = -1;
            auto [ptr, ec] = std::from_chars(
                numberPart.data(),
                numberPart.data() + numberPart.size(),
                value
            );
            if (ec != std::errc{} || ptr != numberPart.data() + numberPart.size()) {
                return -1;
            }
            return value;
        }

        static std::unique_ptr<ComposableItem> FromItemResource(AppContext *appContext,
                                                                const ComposableItemResource *itemResource,
                                                                const ResourceRef &resourceRef) {
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
            auto result = std::make_unique<ComposableItem>(
                Resource::GenerateId(*itemResource), nameRes.value()->value,
                descriptionRes.value()->value,
                appContext->GetResourcePackManager()->LoadTextureFromFile(appContext, itemResource->texture),
                itemResource->slotSize);
            //Filling ability.
            //填充能力。
            size_t argCount = resourceRef.GetArgCount();
            if (argCount == 0) {
                //If the capability is not specified within the resource reference, then the default capability will be loaded.
                //如果没有在资源引用内指定能力，那么加载默认能力。
                size_t defaultAbilitySize = itemResource->defaultAbilityList.size();
                for (int i = 0; i < defaultAbilitySize; i++) {
                    auto itemObj = appContext->GetResourceLocator()->FindItem(
                        appContext, itemResource->defaultAbilityList[i]);
                    if (itemObj.has_value()) {
                        (void) result->ReplaceItem(static_cast<size_t>(i), std::move(itemObj.value()));
                    }
                }
            } else {
                for (int i = 0; i < argCount; i++) {
                    auto refArg = resourceRef.GetArg(i);
                    if (refArg.has_value()) {
                        ResourceRefArg arg = refArg.value();
                        int index = TryParseItemIndex(arg.GetName());
                        if (index < 0) {
                            continue;
                        }
                        auto itemRef = arg.AsResourceRef();
                        if (!itemRef.has_value()) {
                            continue;
                        }
                        auto itemObj = appContext->GetResourceLocator()->FindItem(appContext, itemRef.value());
                        if (itemObj.has_value()) {
                            (void) result->ReplaceItem(static_cast<size_t>(index), std::move(itemObj.value()));
                        }
                    }
                }
            }
            return result;
        }

        [[nodiscard]] ItemContainer *GetItemContainer() const;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}

#endif //GLIMMERWORKS_COMPOSABLEITEM_H
