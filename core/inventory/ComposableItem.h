/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once

#include "Item.h"
#include "ItemContainer.h"
#include "core/math/IAllocStrategy.h"

namespace glimmer
{
    class AbilityItem;
    /**
     * ComposableItem
     * 可组合的物品
     */
    class ComposableItem : public Item
    {
        std::shared_ptr<ItemContainer> itemContainer_ = nullptr;
        std::string id_;
        std::string name_;
        std::optional<std::string> description_;
        std::shared_ptr<SDL_Texture> icon_;
        AbilityConfig totalAbilityConfig_;
        uint32_t maxDurability_;
        bool isUnbreakable_;
        std::shared_ptr<std::function<void(size_t, Item*, ContainerChangeType)>> callback_ = nullptr;
        std::shared_ptr<IAllocStrategy<uint32_t>> allocStrategyPtr_ = nullptr;


        void AddCallback();

    public:
        explicit ComposableItem(const std::string& id, const std::string& name, const std::optional<std::string>& description,
                                const std::shared_ptr<SDL_Texture>& icon, size_t maxSize, uint32_t maxDurability,
                                bool isUnbreakable, const ResourceRef& resourceRef);

        void SetAllocStrategyType(AllocStrategyTypeMessage allocStrategyType);

        [[nodiscard]] AllocStrategyTypeMessage GetAllocStrategyType() const;

        void ReadItemMessage(WorldContext* worldContext, const ItemMessage& itemMessage) override;

        void WriteItemMessage(ItemMessage& itemMessage) const override;

        ~ComposableItem() override;

        [[nodiscard]] uint32_t GetMaxDurability() const override;

        [[nodiscard]] bool IsUnbreakable() const override;

        [[nodiscard]] const std::string& GetId() const override;

        [[nodiscard]] const std::string& GetName() const override;

        [[nodiscard]] const std::optional<std::string>& GetDescription() const override;

        [[nodiscard]] SDL_Texture* GetIcon() const override;

        unsigned GetRemaining() const override;

        void Reduce(unsigned value) override;

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
