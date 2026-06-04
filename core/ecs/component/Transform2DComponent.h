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
#include "core/ecs/GameComponent.h"
#include "core/math/WorldVector2D.h"

namespace glimmer
{
    /**
     * The world coordinate component is used to record the absolute position of objects in the 2D game world
     * 世界坐标组件，用于记录物体在2D游戏世界中的绝对位置
     */
    class Transform2DComponent : public GameComponent
    {
        WorldVector2D position_;
        float rotation_ = 0.0F;

    public:
        Transform2DComponent() = default;

        explicit Transform2DComponent(WorldVector2D position);

        /**
         * SetPosition 设置世界坐标
         * @param newPosition newPosition 新的坐标
         */
        void SetPosition(WorldVector2D newPosition);

        /**
         * SetRotation 设置旋转角度
         * @param newRotation newRotation 新的旋转角度
         */
        void SetRotation(float newRotation);

        /**
         * GetRotation 获取旋转角度
         * @return 旋转角度
         */
        [[nodiscard]] float GetRotation() const;

        /**
         * Translate
         * 移动坐标
         * @param deltaPosition deltaPosition 移动量
         */
        void Translate(WorldVector2D deltaPosition);

        /**
         * GetPosition 获取世界坐标
         * @return 世界坐标
         */
        [[nodiscard]] WorldVector2D GetPosition() const;

        [[nodiscard]] bool IsSerializable() override;

        [[nodiscard]] std::string Serialize() override;

        void Deserialize(WorldContext* worldContext, const std::string& data) override;

        [[nodiscard]] static GameComponentTypeMessage GetComponentTypeStatic();

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
