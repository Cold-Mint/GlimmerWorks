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
#include "core/ecs/GameSystem.h"

namespace glimmer
{
    class ItemToolTipComponent;
    class AppContext;
    class ResourcePackManager;
    class PreloadColors;
    class Item;
    struct AbilityConfig;
    struct LangsResources;

    class ItemToolTipSystem : public GameSystem
    {
        struct TextureCache
        {
            std::shared_ptr<SDL_Texture> texture = nullptr;
            uint64_t fingerprint = 0;
        };

        struct AbilityTipCache
        {
            TextureCache cache;
            bool positive = false;
        };

        float uiScale_ = 1.0F;
        std::shared_ptr<TextureResourceResult> tooltipBgTextureResult_ = nullptr;
        ItemToolTipComponent* itemToolTipComponent_ = nullptr;
        AppContext* appContext_ = nullptr;
        TextureCache itemNameCache_;
        TextureCache itemDescriptionCache_;
        TextureCache itemLockedCache_;
        ResourcePackManager* resourcePackManager_ = nullptr;
        TextureCache canMineBlockTipCache_;
        TextureCache canMineWallTipCache_;
        TextureCache precisionMiningTipCache_;
        AbilityTipCache efficiencyTipCache_;
        AbilityTipCache chainMiningTipCache_;
        PreloadColors* preloadColors_ = nullptr;

        void CollectTooltipTextures(const Item* item, std::vector<SDL_Texture*>& textureToDraw);

        void RenderTooltipBackground(SDL_Renderer* renderer, const SDL_FRect& backgroundRect);

        void RenderTooltipTextures(SDL_Renderer* renderer, const std::vector<SDL_Texture*>& textureToDraw,
                                   float x, float y, float lineSpacing);

        void UpdateItemNameTexture(const Item* item);

        void UpdateItemDescriptionTexture(const Item* item);

        void UpdateAbilityTextures(const AbilityConfig* abilityConfig, const LangsResources* langsResources,
                                   std::vector<SDL_Texture*>& textureToDraw);

        void UpdateMineBlockTexture(const AbilityConfig* abilityConfig, const LangsResources* langsResources,
                                    std::vector<SDL_Texture*>& textureToDraw);

        void UpdateMineWallTexture(const AbilityConfig* abilityConfig, const LangsResources* langsResources,
                                   std::vector<SDL_Texture*>& textureToDraw);

        void UpdatePrecisionMiningTexture(const LangsResources* langsResources,
                                          std::vector<SDL_Texture*>& textureToDraw);

        void UpdateMiningEfficiencyTexture(const AbilityConfig* abilityConfig, const LangsResources* langsResources,
                                           std::vector<SDL_Texture*>& textureToDraw);

        void UpdateChainMiningTexture(const AbilityConfig* abilityConfig, const LangsResources* langsResources,
                                      std::vector<SDL_Texture*>& textureToDraw);

        void UpdateLockedTexture(const LangsResources* langsResources, std::vector<SDL_Texture*>& textureToDraw);

    public:
        explicit ItemToolTipSystem(WorldContext* worldContext);

        void OnFrameStart() override;

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        void OnConfigChanged(const Config* config) override;

        void Render(SDL_Renderer* renderer) override;

        uint8_t GetRenderOrder() override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;
    };
}
