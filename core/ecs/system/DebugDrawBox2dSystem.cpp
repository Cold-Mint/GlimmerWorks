//
// Created by Cold-Mint on 2025/11/6.
//

#include "DebugDrawBox2dSystem.h"

#include "../../utils/Box2DUtils.h"
#include "../../world/WorldContext.h"
#include "../component/CameraComponent.h"
#include "../component/Transform2DComponent.h"


bool glimmer::DebugDrawBox2dSystem::ShouldActivate()
{
    return appContext_->isDebugMode();
}

void glimmer::DebugDrawBox2dSystem::OnActivationChanged(bool activeStatus)
{
    LogCat::d("[DebugDrawBox2dSystem] Activation changed: ", activeStatus);
    if (activeStatus)
    {
        LogCat::d("[DebugDrawBox2dSystem] DebugDraw attached to world.");
    }
    else
    {
        renderer_ = nullptr;
        LogCat::d("[DebugDrawBox2dSystem] DebugDraw detached from world.");
    }
}

/**
 * Set the SDL Renderer Draw Color
 * 设置 SDL 渲染器绘制颜色
 * @param renderer SDL Renderer 渲染器
 * @param color b2HexColor color 颜色（b2HexColor 格式）
 * @param alpha Alpha value (default: 255) 透明度（默认：255）
 */
static void SetSDLColor(SDL_Renderer* renderer, b2HexColor color, Uint8 alpha = 255)
{
    Uint8 r = color >> 16 & 0xFF;
    Uint8 g = color >> 8 & 0xFF;
    Uint8 b = color & 0xFF;
    SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}

/**
 * Draw a Segment
 * 绘制线段（Segment）
 * @param p1 Start point of the segment 线段的起始点
 * @param p2 End point of the segment 线段的结束点
 * @param color Color of the segment 线段的颜色
 * @param context Context pointer (unused) 上下文指针（未使用）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawSegmentFcn(
    b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context)
{
    if (!renderer_) return;

    if (context == nullptr)
    {
        LogCat::w("[DrawSegment] context is nullptr");
        return;
    }

    auto* worldContext = static_cast<WorldContext*>(context);
    Transform2DComponent* cameraTransform2D = worldContext->GetCameraTransform2D();
    CameraComponent* cameraComponent = worldContext->GetCameraComponent();
    if (!cameraTransform2D || !cameraComponent)
    {
        LogCat::w("[DrawSegment] cameraTransform2D or cameraComponent is nullptr");
        return;
    }

    const WorldVector2D p1Pixels = Box2DUtils::ToPixels(p1);
    const WorldVector2D p2Pixels = Box2DUtils::ToPixels(p2);

    const WorldVector2D p1Viewport = cameraComponent->GetViewPortPosition(cameraTransform2D->GetPosition(), p1Pixels);
    const WorldVector2D p2Viewport = cameraComponent->GetViewPortPosition(cameraTransform2D->GetPosition(), p2Pixels);

    SetSDLColor(renderer_, color);

    SDL_RenderLine(renderer_,
                   p1Viewport.x,
                   p1Viewport.y,
                   p2Viewport.x,
                   p2Viewport.y);

    LogCat::d("[DrawSegment] p1=(", p1.x, ",", p1.y, ") -> p2=(", p2.x, ",", p2.y, ")");
}

/**
 * Draw a Solid Polygon
 * 绘制实心多边形（Solid Polygon）
 * @param transform Polygon transform 多边形变换
 * @param vertices Polygon vertices 多边形顶点
 * @param vertexCount Number of vertices 顶点数量
 * @param radius Radius of the polygon (for capsule drawing) 多边形半径（用于胶囊绘制）
 * @param color Polygon color 多边形颜色
 * @param context Context pointer (unused) 上下文指针（未使用）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawSolidPolygonFcn(
    b2Transform transform, const b2Vec2* vertices, int vertexCount,
    float radius, b2HexColor color, void* context)
{
    if (context == nullptr)
    {
        LogCat::w("DrawSolidPolygonFcn context= nullptr");
        return;
    }
    auto* worldContext = static_cast<WorldContext*>(context);
    Transform2DComponent* cameraTransform2D = worldContext->GetCameraTransform2D();
    CameraComponent* cameraComponent = worldContext->GetCameraComponent();
    if (cameraTransform2D == nullptr || cameraComponent == nullptr)
    {
        LogCat::w("DrawSolidPolygonFcn cameraPosition or cameraComponent is nullptr");
        return;
    }
    if (vertexCount != 4)
    {
        LogCat::w("It's not a solid rectangle");
        return;
    }
    if (renderer_ == nullptr)
    {
        LogCat::w("DrawSolidPolygonFcn renderer_=nullptr");
        return;
    }
    //Convert meters to pixels
    //将米转换为像素
    const WorldVector2D upperLeftVector1 = Box2DUtils::ToPixels(transform.p + vertices[0]);
    const WorldVector2D lowerRightVector4 = Box2DUtils::ToPixels(transform.p + vertices[2]);
    //转换为视口坐标
    //Convert to viewport coordinates
    const WorldVector2D upperLeftViewportVector1 = cameraComponent->GetViewPortPosition(
        cameraTransform2D->GetPosition(), upperLeftVector1);
    const WorldVector2D lowerRightViewportVector4 = cameraComponent->GetViewPortPosition(
        cameraTransform2D->GetPosition(), lowerRightVector4);
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(renderer_, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
    const SDL_Color lightBlue = {100, 149, 237, 128}; // 浅蓝 (CornflowerBlue)
    const SDL_Color blue = {0, 0, 255, 255}; // 深蓝边框
    SDL_FRect renderQuad;
    renderQuad.x = upperLeftViewportVector1.x;
    renderQuad.y = upperLeftViewportVector1.y;
    renderQuad.w = lowerRightViewportVector4.x - upperLeftViewportVector1.x;
    renderQuad.h = lowerRightViewportVector4.y - upperLeftViewportVector1.y;

    SDL_SetRenderDrawColor(renderer_, lightBlue.r, lightBlue.g, lightBlue.b, lightBlue.a);
    SDL_RenderFillRect(renderer_, &renderQuad);

    SDL_SetRenderDrawColor(renderer_, blue.r, blue.g, blue.b, blue.a);
    for (int i = 0; i < 3; ++i)
    {
        SDL_FRect border = {
            renderQuad.x - static_cast<float>(i),
            renderQuad.y - static_cast<float>(i),
            renderQuad.w + static_cast<float>(i * 2),
            renderQuad.h + static_cast<float>(i * 2)
        };
        SDL_RenderRect(renderer_, &border);
    }

    // 恢复原绘制颜色
    SDL_SetRenderDrawColor(renderer_, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}


/**
 * Draw a Circle
 * 绘制圆（Circle）
 * @param center Circle center 圆中心
 * @param radius Circle radius 圆半径
 * @param color Circle color 圆颜色
 * @param context Context pointer (unused) 上下文指针（未使用）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawCircleFcn(
    b2Vec2 center, float radius, b2HexColor color, void* context)
{
    if (!renderer_) return;
    SetSDLColor(renderer_, color);
    LogCat::d("[DrawCircle] center=(", center.x, ",", center.y, ") radius=", radius);

    constexpr float step = 2.0f * kPi / kCircleSegments;
    for (int i = 0; i < kCircleSegments; ++i)
    {
        const float a1 = i * step;
        const float a2 = (i + 1) * step;
        const float x1 = center.x + radius * cosf(a1);
        const float y1 = center.y + radius * sinf(a1);
        const float x2 = center.x + radius * cosf(a2);
        const float y2 = center.y + radius * sinf(a2);
        SDL_RenderLine(renderer_,
                       static_cast<int>(x1 * kScale), static_cast<int>(-y1 * kScale),
                       static_cast<int>(x2 * kScale), static_cast<int>(-y2 * kScale));
    }
}

/**
 * Draw a Solid Circle
 * 绘制实心圆（Solid Circle）
 * @param transform Circle transform 圆变换
 * @param radius Circle radius 圆半径
 * @param color Circle color 圆颜色
 * @param context Context pointer (unused) 上下文指针（未使用）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawSolidCircleFcn(
    b2Transform transform, float radius, b2HexColor color, void* context)
{
    if (!renderer_) return;
    SetSDLColor(renderer_, color, 150);
    const b2Vec2 center = transform.p;
    LogCat::d("[DrawSolidCircle] center=(", center.x, ",", center.y, ") radius=", radius);

    constexpr float step = 2.0f * kPi / kCircleSegments;
    for (int i = 0; i < kCircleSegments; ++i)
    {
        const float a1 = i * step;
        const float a2 = (i + 1) * step;
        const float x1 = center.x + radius * cosf(a1);
        const float y1 = center.y + radius * sinf(a1);
        const float x2 = center.x + radius * cosf(a2);
        const float y2 = center.y + radius * sinf(a2);
        SDL_RenderLine(renderer_,
                       static_cast<int>(x1 * kScale), static_cast<int>(-y1 * kScale),
                       static_cast<int>(x2 * kScale), static_cast<int>(-y2 * kScale));
    }
}

/**
 * Draw a Solid Capsule
 * 绘制实心胶囊体（Solid Capsule）
 * @param p1 Capsule start point 胶囊体起点
 * @param p2 Capsule end point 胶囊体终点
 * @param radius Capsule radius 胶囊体半径
 * @param color Capsule color 胶囊体颜色
 * @param context Context pointer (unused) 上下文指针（未使用）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawSolidCapsuleFcn(
    b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context)
{
    if (!renderer_) return;
    SetSDLColor(renderer_, color);
    LogCat::d("[DrawSolidCapsule] p1=(", p1.x, ",", p1.y, ") p2=(", p2.x, ",", p2.y, ") radius=", radius);

    SDL_RenderLine(renderer_,
                   static_cast<int>(p1.x * kScale), static_cast<int>(-p1.y * kScale),
                   static_cast<int>(p2.x * kScale), static_cast<int>(-p2.y * kScale));

    constexpr float step = 2.0f * kPi / kCircleSegments;
    for (int j = 0; j < 2; ++j)
    {
        b2Vec2 c = (j == 0 ? p1 : p2);
        for (int i = 0; i < kCircleSegments; ++i)
        {
            const float a1 = i * step;
            const float a2 = (i + 1) * step;
            const float x1 = c.x + radius * cosf(a1);
            const float y1 = c.y + radius * sinf(a1);
            const float x2 = c.x + radius * cosf(a2);
            const float y2 = c.y + radius * sinf(a2);
            SDL_RenderLine(renderer_,
                           static_cast<int>(x1 * kScale), static_cast<int>(-y1 * kScale),
                           static_cast<int>(x2 * kScale), static_cast<int>(-y2 * kScale));
        }
    }
}

/**
 * Draw a Segment
 * 绘制线段（Segment）
 * @param p1 Segment start point 线段起点
 * @param p2 Segment end point 线段终点
 * @param color Segment color 线段颜色
 * @param context Context pointer (unused) 上下文指针（未使用）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawSegmentFcn(
    b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context)
{
    if (!renderer_) return;
    SetSDLColor(renderer_, color);
    SDL_RenderLine(renderer_,
                   static_cast<int>(p1.x * kScale), static_cast<int>(-p1.y * kScale),
                   static_cast<int>(p2.x * kScale), static_cast<int>(-p2.y * kScale));
    LogCat::d("[DrawSegment] p1=(", p1.x, ",", p1.y, ") p2=(", p2.x, ",", p2.y, ")");
}

/**
 * Draw a Transform
 * 绘制变换坐标轴（Transform）
 * @param transform Transform to draw 要绘制的变换
 * @param context Context pointer (unused) 上下文指针（未使用）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawTransformFcn(b2Transform transform, void* context)
{
    if (!renderer_) return;

    const b2Vec2& p = transform.p;
    b2Vec2 xAxis = {p.x + 0.4f * transform.q.c, p.y + 0.4f * transform.q.s};
    b2Vec2 yAxis = {p.x - 0.4f * transform.q.s, p.y + 0.4f * transform.q.c};

    SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
    SDL_RenderLine(renderer_,
                   static_cast<int>(p.x * kScale), static_cast<int>(-p.y * kScale),
                   static_cast<int>(xAxis.x * kScale), static_cast<int>(-xAxis.y * kScale));

    SDL_SetRenderDrawColor(renderer_, 0, 255, 0, 255);
    SDL_RenderLine(renderer_,
                   static_cast<int>(p.x * kScale), static_cast<int>(-p.y * kScale),
                   static_cast<int>(yAxis.x * kScale), static_cast<int>(-yAxis.y * kScale));

    LogCat::d("[DrawTransform] origin=(", p.x, ",", p.y, ") xAxis=(", xAxis.x, ",", xAxis.y, ") yAxis=(", yAxis.x, ",",
              yAxis.y, ")");
}

/**
 * Draw a Point
 * 绘制点（Point）
 * @param p Point position 点位置
 * @param size Point size 点大小
 * @param color Point color 点颜色
 * @param context Context pointer (unused) 上下文指针（未使用）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawPointFcn(
    b2Vec2 p, float size, b2HexColor color, void* context)
{
    if (!renderer_) return;
    SetSDLColor(renderer_, color);
    const int px = static_cast<int>(p.x * kScale);
    const int py = static_cast<int>(-p.y * kScale);
    const SDL_FRect rect = {(px - size / 2), (py - size / 2), size, size};
    SDL_RenderRect(renderer_, &rect);
    LogCat::d("[DrawPoint] pos=(", p.x, ",", p.y, ") size=", size);
}

/**
 * Draw a String
 * 绘制字符串（String）
 * @param p String position 字符串位置
 * @param s String content 字符串内容
 * @param color String color 字符串颜色
 * @param context Context pointer (unused) 上下文指针（未使用）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawStringFcn(
    b2Vec2 p, const char* s, b2HexColor color, void* context)
{
    LogCat::d("Box2D Debug String (", p.x, ", ", p.y, "): ", s);
}

void glimmer::DebugDrawBox2dSystem::Render(SDL_Renderer* renderer)
{
    renderer_ = renderer;
    b2DebugDraw debugDraw = b2DefaultDebugDraw();
    debugDraw.DrawPolygonFcn = b2DrawPolygonFcn;
    debugDraw.DrawSolidPolygonFcn = b2DrawSolidPolygonFcn;
    debugDraw.DrawCircleFcn = b2DrawCircleFcn;
    debugDraw.DrawSolidCircleFcn = b2DrawSolidCircleFcn;
    debugDraw.DrawSolidCapsuleFcn = b2DrawSolidCapsuleFcn;
    debugDraw.DrawSegmentFcn = b2DrawSegmentFcn;
    debugDraw.DrawTransformFcn = b2DrawTransformFcn;
    debugDraw.DrawPointFcn = b2DrawPointFcn;
    debugDraw.DrawStringFcn = b2DrawStringFcn;
    debugDraw.context = worldContext_;
    debugDraw.drawShapes = true;
    b2World_Draw(worldContext_->GetWorldId(), &debugDraw);
}

uint8_t glimmer::DebugDrawBox2dSystem::GetRenderOrder()
{
    return GameSystem::GetRenderOrder();
}

std::string glimmer::DebugDrawBox2dSystem::GetName()
{
    return "glimmer.DebugDrawBox2dSystem";
}
