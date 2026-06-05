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
#include "core/Constants.h"
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/ItemSlotComponent.h"
#include "SDL3_ttf/SDL_ttf.h"


namespace glimmer
{
    struct PreloadColors;
    class Item;

    class ItemSlotSystem : public GameSystem
    {
        std::shared_ptr<SDL_Texture> itemSlotTexture_ = nullptr;
        std::shared_ptr<SDL_Texture> itemSlotSelectedTexture_ = nullptr;
        std::shared_ptr<SDL_Texture> tooltipBgTexture_ = nullptr;
        PreloadColors* preloadColors_ = nullptr;
        TTF_Font* font_ = nullptr;
        size_t configChangedId_ = INVALID_CONFIG_CALL_BACK;
        AppContext* appContext_ = nullptr;
        float uiScale_ = 1.0F;
        std::vector<GameEntityID> entities_;
        uint32_t guiTransform2DCont_ = 0;
        uint32_t itemSlotCount_ = 0;


        void RenderQuantity(SDL_Renderer* renderer, const SDL_FRect& slotDest, int amount) const;

        /**
         *
         * @param renderer 渲染器
         * @param item 物品
         */
        void RenderTooltip(SDL_Renderer* renderer, const Item* item) const;

    public:
        explicit ItemSlotSystem(WorldContext* worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        ~ItemSlotSystem() override;

        void Render(SDL_Renderer* renderer) override;

        bool HandleEvent(const SDL_Event& event) override;

        uint8_t GetRenderOrder() override;

        [[nodiscard]] GameSystemType GetGameSystemType() override;
    };
}
