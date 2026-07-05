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
#include "core/SelectedItemRuntimeData.h"
#include "core/TagRuntimeData.h"
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/ButtonComponent.h"
#include "core/ecs/component/ItemSlotQuantityComponent.h"

namespace glimmer
{
    class MaterialSelectCraftUISystem : public GUISystem
    {
        AppContext* appContext_ = nullptr;
        RecipeResource* recipeResource_ = nullptr;
        std::unordered_map<uint64_t, std::unique_ptr<TagRuntimeData>> tagRuntimeDataMap_;
        std::vector<ItemSlotQuantityComponent*> itemSlotQuantityVector_;
        StringManager* stringManager_ = nullptr;
        ResourcePackManager* resourcePackManager_ = nullptr;
        PreloadColors* preloadColors_ = nullptr;
        ResourceLocator* resourceLocator_ = nullptr;
        float uiScale_ = 1.0F;
        int windowHeight_ = 0;
        int windowWidth_ = 0;
        std::shared_ptr<TextureResourceResult> panelBackGroundTextureResult_ = nullptr;
        std::shared_ptr<TextureResourceResult> subPanelBackGroundTextureResult_ = nullptr;
        //Key represents the index of the item, while Value indicates the selected quantity.
        //Key为物品下标，Value为选中的数量。
        std::vector<std::unique_ptr<SelectedItemRuntimeData>> selectedItemVector_;
        DesignDimension panelWidth_ = 0.0F;
        DesignDimension panelHeight_ = 0.0F;
        // Base inner padding between panel and sub-panel
        // 面板与子面板之间的基础内边距
        DesignDimension basePanelInnerPadding_ = 4.0F;
        // Actual inner padding (basePadding * uiScale)
        // 实际内边距（基础内边距 × UI缩放比例）
        float panelInnerPadding_ = 4.0F;
        ButtonComponent* buttonComponent_ = nullptr;
        // Layout calculation variables for position updates
        // 用于位置更新的布局计算变量
        DesignDimension maxTextureWidth_ = 0.0F;
        DesignDimension maxTextureHeight_ = 0.0F;
        uint8_t matchingCount_ = 0;
        LangsResources* langsResources_ = nullptr;

    public:
        explicit MaterialSelectCraftUISystem(WorldContext* worldContext);

        void OnActivationChanged(bool activeStatus) override;

        void OnConfigChanged(const Config* config) override;

        void OnWindowSizeChanged(const int& width, const int& height) override;

        void Render(SDL_Renderer* renderer) override;

        uint8_t GetRenderOrder() override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;

    private:
        void UpdateItemSlotPositions() const;
        void UpdateButtonPosition() const;
    };
}
