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
#include <memory>
#include <string>

#include "ItemDurabilityModule.h"
#include "ItemLockModule.h"
#include "ItemStackModule.h"
#include "ItemTagModule.h"
#include "ability/AbilityType.h"
#include "core/math/IAllocatable.h"
#include "core/world/TileResourceData.h"

namespace glimmer {
    class WorldContext;
    /**
     * 物品类
     */

    class Item : public IAllocatable<uint32_t> {
        friend class ItemContainer;
        friend class GiveCommand;
        friend class ItemEditorCommand;
        ResourceRef resourceRef_;
        ItemDurabilityModule itemDurabilityModule_;
        ItemLockModule itemLockModule_;
        ItemStackModule itemStackModule_;
        ItemTagModule itemTagModule_;

    protected:
        [[nodiscard]] const ResourceRef &GetResourceRef() const;

        void SetResourceRef(const ResourceRef &resourceRef);

        [[nodiscard]] ItemDurabilityModule *GetMutableDurabilityModule();

        [[nodiscard]] ItemStackModule *GetMutableStackModule();

        [[nodiscard]] ItemTagModule *GetMutableTagModule();

        [[nodiscard]] ItemLockModule *GetMutableLockModule();

        void SetTags(const std::vector<ItemTagResource> &tags);

        void SetMaxStack(uint8_t maxStack);

        void SetUnbreakable(bool unbreakable);

    public:
        ~Item() override = default;

        virtual void ReadItemMessage(WorldContext *worldContext, const ItemMessage &itemMessage);

        virtual void WriteItemMessage(ItemMessage &itemMessage) const;

        [[nodiscard]] const ItemDurabilityModule *GetDurabilityModule() const;

        [[nodiscard]] const ItemStackModule *GetStackModule() const;

        [[nodiscard]] const ItemTagModule *GetTagModule() const;

        [[nodiscard]] const ItemLockModule *GetLockModule() const;

        /**
          * GetId
          * 获取id
          * @return
          */
        [[nodiscard]] virtual const std::string &GetId() const = 0;

        /**
         * GetName
         * 获取物品名称
         * @return
         */
        [[nodiscard]] virtual const std::string &GetName() const = 0;

        /**
         * GetDescription
         * 获取描述
         * @return
         */
        [[nodiscard]] virtual const std::optional<std::string> &GetDescription() const = 0;

        /**
         * Variable configuration for obtaining items
         * 获取物品的变量配置
         * @return
         */
        [[nodiscard]] virtual const AbilityConfig *GetAbilityConfig() const = 0;

        /**
         * 获取图标
         * @return
         */
        [[nodiscard]] virtual TextureResourceResult *GetIcon() const = 0;

        [[nodiscard]] virtual const ResourceRef *GetIconResourceRef() const = 0;

        /**
         * OnUse
         * 使用物品
         * @param mouseLeft mouseLeft 是否为鼠标左键
         * @param worldContext worldContext 世界上下文
         * @param user user 使用者
         * @param abilityConfig abilityConfig 能力配置
         * @param popupAbility The ability to pop up 需要弹出的能力
         * @return  Whether this event has been consumed or not. If it returns true, it will not be passed on to the hands; if it returns false, it will be handed over to the hands for processing. 是否消费了此事件，如果返回true，那么不会传递到手上，如果返回false那么交给手处理。
         */
        virtual bool OnUse(bool mouseLeft, WorldContext *worldContext, uint32_t user,
                           const AbilityConfig *abilityConfig,
                           std::unordered_set<AbilityType> &popupAbility) = 0;

        /**
         * Get the remaining durability.
         * 获取剩余耐久度。
         * @return
         */
        unsigned GetRemaining() const override;

        /**
        * AddUsedDurability(Deducting durability)
        * 添加使用的耐久（扣除耐久）
        * @param value
        */
        void Reduce(unsigned value) override;

        /**
         * Clone
         * 克隆
         * @return
         */
        [[nodiscard]] virtual std::unique_ptr<Item> Clone() const = 0;
    };
}
