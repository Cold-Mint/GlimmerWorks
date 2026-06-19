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
#include "GUISystem.h"
#include "core/ecs/component/CraftPreviewSlotComponent.h"
#include "core/ecs/component/PlayerComponent.h"

namespace glimmer
{
    /**
     * InventoryGUI
     * 库存GUI
     */
    class InventoryCraftGUISystem : public GUISystem
    {
        std::vector<ItemSlotComponent*> inventoryItemSlot_;
        std::vector<CraftPreviewSlotComponent*> craftPreviewSlot_;
        float uiScale_ = 1.0F;
        RecipeManager* recipeManager_ = nullptr;
        ResourcePackManager* resourcePackManager_ = nullptr;
        PlayerComponent* playerComponent_ = nullptr;
        ItemContainer* itemContainer_ = nullptr;
        LangsResources* langsResources_ = nullptr;
        PreloadColors* preloadColors_ = nullptr;
        /**
         * The number of unlocked recipes
         * 解锁的配方数量
         */
        size_t unlockedRecipesSize_ = 0;
        uint64_t craftFingerprint_ = 0;
        std::shared_ptr<SDL_Texture> craftTexture_ = nullptr;

    public:
        explicit InventoryCraftGUISystem(WorldContext* worldContext);

        void Render(SDL_Renderer* renderer) override;

        void OnActivationChanged(bool activeStatus) override;

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;

        void OnConfigChanged(const Config* config) override;

        void OnWindowSizeChanged(int width, int height) override;
    };
}
