//
// Created by Cold-Mint on 2025/11/6.
//

#ifndef GLIMMERWORKS_DEBUGDRAWBOX2DSYSTEM_H
#define GLIMMERWORKS_DEBUGDRAWBOX2DSYSTEM_H
#include "../GameSystem.h"
#include "box2d/types.h"

namespace glimmer {
    constexpr float kScale = 30.0F; // 1 meter = 30 pixels （游戏单位缩放）
    constexpr int kCircleSegments = 24; // 圆形分段数
    constexpr float kPi = 3.14159265358979323846F;

    class DebugDrawBox2dSystem : public GameSystem {
        [[nodiscard]] bool ShouldActivate() override;

        void OnActivationChanged(bool activeStatus) override;

    public:
        DebugDrawBox2dSystem(AppContext *appContext, WorldContext *worldContext);

        static void b2DrawPolygonFcn(const b2Vec2 *b2_vec2, int i, b2HexColor b2_hex_color, void *p);

        static void b2DrawSolidPolygonFcn(b2Transform transform, const b2Vec2 *vertices, int vertexCount, float radius,
                                          b2HexColor color, void *context);

        static void b2DrawCircleFcn(b2Vec2 center, float radius, b2HexColor color, void *context);

        static void b2DrawSolidCircleFcn(b2Transform transform, float radius, b2HexColor color, void *context);

        static void b2DrawSolidCapsuleFcn(b2Vec2 b2_vec2, b2Vec2 b2_vec3, float x, b2HexColor b2_hex_color, void *p);

        static void b2DrawSegmentFcn(b2Vec2 b2_vec2, b2Vec2 b2_vec3, b2HexColor b2_hex_color, void *p);

        static void b2DrawTransformFcn(b2Transform transform, void *context);

        static void b2DrawPointFcn(b2Vec2 p, float size, b2HexColor color, void *context);

        static void b2DrawStringFcn(b2Vec2 p, const char *s, b2HexColor color, void *context);


        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_DEBUGDRAWBOX2DSYSTEM_H
