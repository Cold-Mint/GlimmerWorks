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
#include "core/ecs/component/PlayerComponent.h"

namespace glimmer
{
    class RigidBody2DComponent;
    /**
     * The player control system processes the player's input control and realizes the WASD mobile camera function
     * 玩家控制系统，处理玩家的输入控制，实现WASD移动相机功能
     */
    class PlayerControlSystem final : public GameSystem
    {
        AudioManager* audioManager_ = nullptr;
        std::shared_ptr<AudioResourceResult> dropItemSFXResult_ = nullptr;
        CameraComponent* cameraComponent_ = nullptr;
        Transform2DComponent* cameraTransform2DComponent_ = nullptr;
        GameEntityID playerEntityID_ = GAME_ENTITY_ID_INVALID;
        std::unordered_set<std::string> popupAbility_;
        /**
        * Check if the player is on the ground
        * 检查玩家是否在地面上
        * @return If the player is on the ground, return true; otherwise, return false 如果玩家在地面上则返回true，否则返回false
        */
        bool OnGround(const PlayerComponent* playerControlComponent) const;

        /**
         * DropItem
         * 丢弃物品
         * @param itemContainer
         * @param index
         */
        void DropItem(const ItemContainer* itemContainer, uint8_t index) const;

        /**
         * UseItem
         * 使用物品
         * @param item
         */
        void UseItem(Item* item);

    public:
        explicit PlayerControlSystem(WorldContext* worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        void Update(float delta) override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;

        bool HandleEvent(const SDL_Event& event) override;
    };
}
