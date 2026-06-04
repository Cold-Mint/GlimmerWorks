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
#include "ability/ItemAbility.h"
#include "core/world/WorldContext.h"

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
                             bool canUseAlone, std::unordered_set<uint64_t> tags,
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

        void OnUse(WorldContext* worldContext, GameEntityID user, const AbilityConfig* abilityConfig,
                   std::unordered_set<std::string>& popupAbility) override;

        [[nodiscard]] std::unique_ptr<Item> Clone() const override;
    };
}
