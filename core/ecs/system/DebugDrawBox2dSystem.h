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
#include "box2d/types.h"

namespace glimmer
{
    constexpr float kScale = 30.0F; // 1 meter = 30 pixels （游戏单位缩放）
    constexpr int kCircleSegments = 24; // 圆形分段数
    constexpr float kPi = 3.14159265358979323846F;

    class DebugDrawBox2dSystem : public GameSystem
    {
        CameraComponent* cameraComponent_ = nullptr;
        Transform2DComponent* cameraTransform2DComponent_ = nullptr;
        uint32_t transform2DCount_ = 0;
        uint32_t rayCast2DCount_ = 0;
        std::vector<GameEntityID> entities_;
        bool displayBox2dShape_ = false;

    public:
        explicit DebugDrawBox2dSystem(WorldContext* worldContext);

        void OnConfigChanged(const Config* config) override;

        static void b2DrawPolygonFcn(const b2Vec2* b2_vec2, int i, b2HexColor b2_hex_color, void* p);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        static void b2DrawSolidPolygonFcn(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius,
                                          b2HexColor color, void* context);

        static void b2DrawCircleFcn(b2Vec2 center, float radius, b2HexColor color, void* context);

        static void b2DrawSolidCircleFcn(b2Transform transform, float radius, b2HexColor color, void* context);

        static void b2DrawSolidCapsuleFcn(b2Vec2 b2_vec2, b2Vec2 b2_vec3, float x, b2HexColor b2_hex_color, void* p);

        static void b2DrawSegmentFcn(b2Vec2 b2_vec2, b2Vec2 b2_vec3, b2HexColor b2_hex_color, void* p);

        static void b2DrawTransformFcn(b2Transform transform, void* context);

        static void b2DrawPointFcn(b2Vec2 p, float size, b2HexColor color, void* context);

        static void b2DrawStringFcn(b2Vec2 p, const char* s, b2HexColor color, void* context);

        bool CanActive() const override;

        void Render(SDL_Renderer* renderer) override;

        uint8_t GetRenderOrder() override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;
    };
}
#endif
