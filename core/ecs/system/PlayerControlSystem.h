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
#include <unordered_set>
#include <src/core/place_source_message.pb.h>

#include "core/ecs/GameSystem.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/inventory/ItemUseContext.h"
#include "core/inventory/ability/AbilityType.h"

namespace glimmer {
    class ColliderComponent;
    class AudioManager;
    class CameraComponent;
    class Transform2DComponent;
    class ItemContainer;
    class Item;
    class HotBarComponent;
    class MainThreadDispatcher;
    class ResourceRef;
    enum class BreakSource;

    /**
     * The player control system processes the player's input control and realizes the WASD mobile camera function
     * 玩家控制系统，处理玩家的输入控制，实现WASD移动相机功能
     */
    class PlayerControlSystem final : public GameSystem {
        std::unordered_set<AbilityType> popupAbility_;
        ItemUseContext itemUseContext_;
        std::shared_ptr<AudioResourceResult> dropItemSFXResult_ = nullptr;
        AudioManager *audioManager_ = nullptr;
        CameraComponent *cameraComponent_ = nullptr;
        Transform2DComponent *cameraTransform2DComponent_ = nullptr;
        GameEntityID playerEntityID_ = GAME_ENTITY_ID_INVALID;

        /**
         * DropItem
         * 丢弃物品
         * @param itemContainer
         * @param index
         */
        void DropItem(ItemContainer *itemContainer, uint8_t index);

        static void UpdateFlying(const PlayerInputHandler *playerInputHandler,
                                 Transform2DComponent *transform2DComponent);

        /**
         * Compute the player's velocity from input (horizontal acceleration/deceleration and jump) and write it
         * into the collider. Gravity and tile collision are handled later by PhysicsSystem.
         * 根据输入计算玩家速度（水平加速/减速与跳跃），写入碰撞体。重力与瓦片碰撞由 PhysicsSystem 稍后处理。
         */
        static void UpdateGroundedMovement(PlayerInputHandler *playerInputHandler,
                                           const PlayerComponent *playerComponent,
                                           ColliderComponent *collider);

        void CheckDropItem(PlayerInputHandler *playerInputHandler, ItemContainer *itemContainer);

        /**
         * When the item does not handle the mouse click, treat it as a tile action event and apply the
         * primary/secondary action target of each focused tile.
         * 当物品未处理鼠标点击时，将其视为瓦片活动事件，并应用每个聚焦瓦片的主/次要活动目标。
         */
        void HandleFocusedTileActions() const;

        /**
         * Apply a single action target of the focused tile on the next main frame.
         * 在下一个主帧应用聚焦瓦片的单个活动目标。
         * @param mainThreadDispatcher mainThreadDispatcher 主线程调度器
         * @param worldContext worldContext 世界上下文
         * @param focusedTile focusedTile 聚焦瓦片
         * @param actionOrigin actionOrigin 活动目标的应用原点（多方块瓦片的原点格子）
         * @param actionTarget actionTarget 活动目标资源引用
         * @param breakSource breakSource 破坏来源
         * @param placeSource placeSource 放置源
         */
        void ApplyTileActionTarget(MainThreadDispatcher *mainThreadDispatcher, WorldContext *worldContext,
                                   const Tile *focusedTile, const TileVector2D &actionOrigin,
                                   const ResourceRef &actionTarget,
                                   BreakSource breakSource, PlaceSourceMessage placeSource) const;

        /**
         * Play the primary/secondary action sound effect of a tile if it is valid.
         * 播放瓦片的主/次要活动音效（若有效）。
         * @param tile tile 瓦片
         * @param primary primary true=主活动，false=副活动
         */
        void PlayTileActionSFX(const Tile *tile, bool primary) const;

        static void HandleMouseButton(const SDL_Event &event, PlayerInputHandler *playerInputHandler);

        void HandleHorizontalInput(const SDL_Event &event, PlayerComponent *playerComponent,
                                   PlayerInputHandler *playerInputHandler, bool pressed) const;

        static void HandleVerticalInput(const SDL_Event &event, PlayerInputHandler *playerInputHandler, bool pressed);

        void UpdatePlayerFacing(PlayerComponent *playerComponent, const PlayerInputHandler *playerInputHandler) const;

        static void HandleKeyAction(const SDL_Event &event, PlayerInputHandler *playerInputHandler, bool pressed);

    public:
        explicit PlayerControlSystem(WorldContext *worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        void OnTick(uint64_t tick) override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;

        bool HandleEvent(const SDL_Event &event) override;
    };
}
