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
#if  !defined(NDEBUG)
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class CameraComponent;
    class Transform2DComponent;
    class PreloadColors;

    /**
     * Debug system that draws the custom AABB collision boxes of entities and the solid tile grid.
     * Replaces the removed Box2D debug draw (display_box2d_shape).
     * 绘制实体自定义 AABB 碰撞盒与实心瓦片网格的调试系统。取代了被移除的 Box2D 调试绘制（display_box2d_shape）。
     */
    class DebugDrawColliderSystem : public GameSystem {
        std::vector<GameEntityID> entities_;
        CameraComponent *cameraComponent_ = nullptr;
        Transform2DComponent *cameraTransform2DComponent_ = nullptr;
        PreloadColors *preloadColors_ = nullptr;
        bool displayCollisionBox_ = false;
        uint32_t colliderCount_ = 0;
        uint32_t transform2dCount_ = 0;

        void RenderEntityColliders(RenderQueue *queue) const;

        void RenderSolidTiles(RenderQueue *queue) const;

    public:
        explicit DebugDrawColliderSystem(WorldContext *worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        void OnConfigChanged(const Config *config) override;

        [[nodiscard]] bool CanActive() const override;

        void Render(RenderQueue *queue) override;

        uint8_t GetExecutionOrder() override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;
    };
}

#endif