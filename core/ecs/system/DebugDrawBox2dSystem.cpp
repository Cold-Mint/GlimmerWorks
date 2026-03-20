//
// Created by Cold-Mint on 2025/11/6.
//

#include "DebugDrawBox2dSystem.h"

#include "Box2dSystemContext.h"
#include "../../utils/Box2DUtils.h"
#include "../../world/WorldContext.h"
#include "../component/CameraComponent.h"
#include "../component/Transform2DComponent.h"
#include "box2d/box2d.h"
#include "core/ecs/component/RayCast2DComponent.h"

bool glimmer::DebugDrawBox2dSystem::ShouldActivate() {
    return worldContext_->GetAppContext()->GetConfig()->debug.displayBox2dShape;
}

void glimmer::DebugDrawBox2dSystem::OnActivationChanged(bool activeStatus) {
    LogCat::d("[DebugDrawBox2dSystem] Activation changed: ", activeStatus);
    if (activeStatus) {
        LogCat::d("[DebugDrawBox2dSystem] DebugDraw attached to world.");
    } else {
        LogCat::d("[DebugDrawBox2dSystem] DebugDraw detached from world.");
    }
}

glimmer::DebugDrawBox2dSystem::DebugDrawBox2dSystem(WorldContext *worldContext) : GameSystem(worldContext) {
}

/**
 * Set the SDL Renderer Draw Color
 * 设置 SDL 渲染器绘制颜色
 * @param renderer SDL Renderer 渲染器
 * @param color b2HexColor color 颜色（b2HexColor 格式）
 * @param alpha Alpha value (default: 255) 透明度（默认：255）
 */
static void SetSDLColor(SDL_Renderer *renderer, b2HexColor color, Uint8 alpha = 255) {
    Uint8 r = color >> 16 & 0xFF;
    Uint8 g = color >> 8 & 0xFF;
    Uint8 b = color & 0xFF;
    SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}

/**
 * 通用坐标转换工具：Box2D世界坐标 -> 屏幕视口坐标
 * @param worldContext 世界上下文
 * @param worldPos Box2D世界坐标（米）
 * @return 屏幕视口坐标（像素）
 */
static WorldVector2D ConvertBox2DToViewport(const glimmer::WorldContext *worldContext, const b2Vec2 &worldPos) {
    const glimmer::Transform2DComponent *cameraTransform2D = worldContext->GetCameraTransform2D();
    const glimmer::CameraComponent *cameraComponent = worldContext->GetCameraComponent();
    if (!cameraTransform2D || !cameraComponent) {
        return {worldPos.x * glimmer::kScale, -worldPos.y * glimmer::kScale}; // 降级：无相机时直接缩放
    }
    // 米转像素 + 相机视口转换
    WorldVector2D pixelPos = glimmer::Box2DUtils::ToPixels(worldPos);
    return cameraComponent->GetViewPortPosition(cameraTransform2D->GetPosition(), pixelPos);
}

/**
 * 兼容SDL3：绘制实心圆（逐点扫描实现）
 * @param renderer SDL渲染器
 * @param centerX 圆心X坐标（像素）
 * @param centerY 圆心Y坐标（像素）
 * @param radius 半径（像素）
 */
static void SDL_RenderFillCircle_Compat(SDL_Renderer *renderer, float centerX, float centerY, float radius) {
    // 中点圆算法绘制实心圆
    int r = static_cast<int>(radius);
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                SDL_RenderPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

/**
 * 兼容SDL3：绘制带圆角的实心矩形（逐点扫描实现）
 * @param renderer SDL渲染器
 * @param rect 矩形区域（像素）
 * @param radius 圆角半径（像素）
 */
static void SDL_RenderFillRectRounded_Compat(SDL_Renderer *renderer, const SDL_FRect *rect, float radius) {
    if (radius <= 0) {
        // 无圆角时直接绘制普通矩形
        SDL_RenderFillRect(renderer, rect);
        return;
    }

    // 1. 绘制矩形主体（去掉四个角）
    SDL_FRect mainRect = {
        rect->x + radius,
        rect->y,
        rect->w - 2 * radius,
        rect->h
    };
    SDL_RenderFillRect(renderer, &mainRect);

    SDL_FRect sideRect1 = {
        rect->x,
        rect->y + radius,
        radius,
        rect->h - 2 * radius
    };
    SDL_RenderFillRect(renderer, &sideRect1);

    SDL_FRect sideRect2 = {
        rect->x + rect->w - radius,
        rect->y + radius,
        radius,
        rect->h - 2 * radius
    };
    SDL_RenderFillRect(renderer, &sideRect2);

    // 2. 绘制四个圆角（实心圆的1/4）
    SDL_RenderFillCircle_Compat(renderer, rect->x + radius, rect->y + radius, radius); // 左上
    SDL_RenderFillCircle_Compat(renderer, rect->x + rect->w - radius, rect->y + radius, radius); // 右上
    SDL_RenderFillCircle_Compat(renderer, rect->x + radius, rect->y + rect->h - radius, radius); // 左下
    SDL_RenderFillCircle_Compat(renderer, rect->x + rect->w - radius, rect->y + rect->h - radius, radius); // 右下
}

/**
 * 兼容SDL3：绘制带圆角的矩形边框（逐线绘制实现）
 * @param renderer SDL渲染器
 * @param rect 矩形区域（像素）
 * @param radius 圆角半径（像素）
 */
static void SDL_RenderRectRounded_Compat(SDL_Renderer *renderer, const SDL_FRect *rect, float radius) {
    if (radius <= 0) {
        SDL_RenderRect(renderer, rect);
        return;
    }

    // 绘制直线部分
    // 上下边
    SDL_RenderLine(renderer, rect->x + radius, rect->y, rect->x + rect->w - radius, rect->y);
    SDL_RenderLine(renderer, rect->x + radius, rect->y + rect->h, rect->x + rect->w - radius, rect->y + rect->h);
    // 左右边
    SDL_RenderLine(renderer, rect->x, rect->y + radius, rect->x, rect->y + rect->h - radius);
    SDL_RenderLine(renderer, rect->x + rect->w, rect->y + radius, rect->x + rect->w, rect->y + rect->h - radius);

    // 绘制圆角弧线（简化版：用线段模拟）
    constexpr int segments = 16;
    constexpr float step = glimmer::kPi / 2 / segments;

    // 左上圆角
    for (int i = 0; i < segments; i++) {
        float a1 = glimmer::kPi + step * i;
        float a2 = glimmer::kPi + step * (i + 1);
        float x1 = rect->x + radius + radius * cosf(a1);
        float y1 = rect->y + radius + radius * sinf(a1);
        float x2 = rect->x + radius + radius * cosf(a2);
        float y2 = rect->y + radius + radius * sinf(a2);
        SDL_RenderLine(renderer, x1, y1, x2, y2);
    }

    // 右上圆角
    for (int i = 0; i < segments; i++) {
        float a1 = 2 * glimmer::kPi - step * i;
        float a2 = 2 * glimmer::kPi - step * (i + 1);
        float x1 = rect->x + rect->w - radius + radius * cosf(a1);
        float y1 = rect->y + radius + radius * sinf(a1);
        float x2 = rect->x + rect->w - radius + radius * cosf(a2);
        float y2 = rect->y + radius + radius * sinf(a2);
        SDL_RenderLine(renderer, x1, y1, x2, y2);
    }

    // 左下圆角
    for (int i = 0; i < segments; i++) {
        float a1 = glimmer::kPi - step * i;
        float a2 = glimmer::kPi - step * (i + 1);
        float x1 = rect->x + radius + radius * cosf(a1);
        float y1 = rect->y + rect->h - radius + radius * sinf(a1);
        float x2 = rect->x + radius + radius * cosf(a2);
        float y2 = rect->y + rect->h - radius + radius * sinf(a2);
        SDL_RenderLine(renderer, x1, y1, x2, y2);
    }

    // 右下圆角
    for (int i = 0; i < segments; i++) {
        float a1 = step * i;
        float a2 = step * (i + 1);
        float x1 = rect->x + rect->w - radius + radius * cosf(a1);
        float y1 = rect->y + rect->h - radius + radius * sinf(a1);
        float x2 = rect->x + rect->w - radius + radius * cosf(a2);
        float y2 = rect->y + rect->h - radius + radius * sinf(a2);
        SDL_RenderLine(renderer, x1, y1, x2, y2);
    }
}

/**
 * Draw a Polygon
 * 绘制多边形（Polygon）
 * @param vertices Polygon vertices 多边形顶点
 * @param vertexCount Number of vertices 顶点数量
 * @param color Polygon color 多边形颜色
 * @param context Context pointer 上下文指针
 */
void glimmer::DebugDrawBox2dSystem::b2DrawPolygonFcn(
    const b2Vec2 *vertices, int vertexCount, b2HexColor color, void *context) {
    if (context == nullptr) {
        return;
    }
    const auto *box2dSystemContext = static_cast<Box2dSystemContext *>(context);
    const WorldContext *worldContext = box2dSystemContext->GetWorldContext();
    SDL_Renderer *sdlRenderer = box2dSystemContext->GetRenderer();
    SetSDLColor(sdlRenderer, color);

    // 保存原始颜色
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(sdlRenderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

    for (int i = 0; i < vertexCount; ++i) {
        const b2Vec2 &v1 = vertices[i];
        const b2Vec2 &v2 = vertices[(i + 1) % vertexCount];

        // 转换为屏幕视口坐标
        WorldVector2D p1 = ConvertBox2DToViewport(worldContext, v1);
        WorldVector2D p2 = ConvertBox2DToViewport(worldContext, v2);

        SDL_RenderLine(sdlRenderer, p1.x, p1.y, p2.x, p2.y);
        LogCat::d("  Edge ", i, ": (", v1.x, ",", v1.y, ") -> (", v2.x, ",", v2.y, ")");
    }

    // 恢复原始颜色
    SDL_SetRenderDrawColor(sdlRenderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

/**
 * Draw a Solid Polygon（核心修复：启用radius绘制圆角）
 * 绘制实心多边形（Solid Polygon）
 * @param transform Polygon transform 多边形变换
 * @param vertices Polygon vertices 多边形顶点
 * @param vertexCount Number of vertices 顶点数量
 * @param radius 圆角半径（启用）
 * @param color Polygon color 多边形颜色
 * @param context Context pointer 上下文指针
 */
void glimmer::DebugDrawBox2dSystem::b2DrawSolidPolygonFcn(
    b2Transform transform, const b2Vec2 *vertices, int vertexCount,
    float radius, b2HexColor color, void *context) {
    if (context == nullptr) {
        LogCat::w("DrawSolidPolygonFcn context= nullptr");
        return;
    }
    const auto *box2dSystemContext = static_cast<Box2dSystemContext *>(context);
    const WorldContext *worldContext = box2dSystemContext->GetWorldContext();
    SDL_Color box2dBorderColor = worldContext->GetAppContext()->GetPreloadColors()->debugColor.box2dBorderColor;
    SDL_Color box2dFullColor = worldContext->GetAppContext()->GetPreloadColors()->debugColor.box2dFullColor;
    SDL_Renderer *sdlRenderer = box2dSystemContext->GetRenderer();
    const Transform2DComponent *cameraTransform2D = worldContext->GetCameraTransform2D();
    const CameraComponent *cameraComponent = worldContext->GetCameraComponent();
    if (cameraTransform2D == nullptr || cameraComponent == nullptr) {
        LogCat::w("DrawSolidPolygonFcn cameraPosition or cameraComponent is nullptr");
        return;
    }
    if (vertexCount != 4) {
        LogCat::w("Only support solid rectangle (4 vertices) for now");
        return;
    }

    // 保存原始颜色
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(sdlRenderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

    // 转换世界坐标（米）-> 像素 -> 视口坐标
    const WorldVector2D upperLeft = Box2DUtils::ToPixels(transform.p + vertices[0]);
    const WorldVector2D lowerRight = Box2DUtils::ToPixels(transform.p + vertices[2]);
    const WorldVector2D upperLeftViewport = cameraComponent->GetViewPortPosition(
        cameraTransform2D->GetPosition(), upperLeft);
    const WorldVector2D lowerRightViewport = cameraComponent->GetViewPortPosition(
        cameraTransform2D->GetPosition(), lowerRight);

    // 计算矩形尺寸
    SDL_FRect renderQuad;
    renderQuad.x = upperLeftViewport.x;
    renderQuad.y = upperLeftViewport.y;
    renderQuad.w = lowerRightViewport.x - upperLeftViewport.x;
    renderQuad.h = lowerRightViewport.y - upperLeftViewport.y;

    // 1. 绘制实心矩形（带圆角）- 替换为兼容实现
    SDL_SetRenderDrawColor(sdlRenderer, box2dFullColor.r, box2dFullColor.g, box2dFullColor.b, box2dFullColor.a);
    // 转换radius为像素（米 -> 像素）
    float radiusPx = radius * kScale;
    SDL_RenderFillRectRounded_Compat(sdlRenderer, &renderQuad, radiusPx);

    // 2. 绘制边框（多层边框增强视觉）- 替换为兼容实现
    SDL_SetRenderDrawColor(sdlRenderer, box2dBorderColor.r, box2dBorderColor.g, box2dBorderColor.b, box2dBorderColor.a);
    for (int i = 0; i < 3; ++i) {
        SDL_FRect border = {
            renderQuad.x - static_cast<float>(i),
            renderQuad.y - static_cast<float>(i),
            renderQuad.w + static_cast<float>(i * 2),
            renderQuad.h + static_cast<float>(i * 2)
        };
        SDL_RenderRectRounded_Compat(sdlRenderer, &border, radiusPx);
    }

    // 恢复原绘制颜色
    SDL_SetRenderDrawColor(sdlRenderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

/**
 * Draw a Circle（对齐相机/视口逻辑）
 * 绘制圆（Circle）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawCircleFcn(
    b2Vec2 center, float radius, b2HexColor color, void *context) {
    if (context == nullptr) {
        LogCat::w("b2DrawCircleFcn context= nullptr");
        return;
    }
    const auto *box2dSystemContext = static_cast<Box2dSystemContext *>(context);
    const WorldContext *worldContext = box2dSystemContext->GetWorldContext();
    SDL_Renderer *sdlRenderer = box2dSystemContext->GetRenderer();

    // 保存原始颜色
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(sdlRenderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

    SetSDLColor(sdlRenderer, color);
    // 转换圆心到视口坐标
    WorldVector2D centerViewport = ConvertBox2DToViewport(worldContext, center);
    // 半径转换为像素
    float radiusPx = radius * kScale;

    constexpr float step = 2.0F * kPi / kCircleSegments;
    for (int i = 0; i < kCircleSegments; ++i) {
        const float a1 = static_cast<float>(i) * step;
        const float a2 = static_cast<float>(i + 1) * step;
        const float x1 = centerViewport.x + radiusPx * cosf(a1);
        const float y1 = centerViewport.y + radiusPx * sinf(a1);
        const float x2 = centerViewport.x + radiusPx * cosf(a2);
        const float y2 = centerViewport.y + radiusPx * sinf(a2);
        SDL_RenderLine(sdlRenderer, x1, y1, x2, y2);
    }

    // 恢复原始颜色
    SDL_SetRenderDrawColor(sdlRenderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

/**
 * Draw a Solid Circle（对齐相机/视口逻辑）
 * 绘制实心圆（Solid Circle）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawSolidCircleFcn(
    b2Transform transform, float radius, b2HexColor color, void *context) {
    if (context == nullptr) {
        LogCat::w("b2DrawSolidCircleFcn context= nullptr");
        return;
    }
    const auto *box2dSystemContext = static_cast<Box2dSystemContext *>(context);
    const WorldContext *worldContext = box2dSystemContext->GetWorldContext();
    SDL_Renderer *sdlRenderer = box2dSystemContext->GetRenderer();

    // 保存原始颜色
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(sdlRenderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

    // 设置半透明填充色
    SetSDLColor(sdlRenderer, color, 150);
    const b2Vec2 center = transform.p;
    // 转换圆心到视口坐标
    WorldVector2D centerViewport = ConvertBox2DToViewport(worldContext, center);
    // 半径转换为像素
    float radiusPx = radius * kScale;
    LogCat::d("[DrawSolidCircle] center=(", center.x, ",", center.y, ") radius=", radius);

    // 绘制实心圆 - 替换为兼容实现
    SDL_RenderFillCircle_Compat(sdlRenderer, centerViewport.x, centerViewport.y, radiusPx);

    // 绘制圆边框（增强视觉）
    SetSDLColor(sdlRenderer, color, 255);
    constexpr float step = 2.0F * kPi / kCircleSegments;
    for (int i = 0; i < kCircleSegments; ++i) {
        const float a1 = static_cast<float>(i) * step;
        const float a2 = static_cast<float>(i + 1) * step;
        const float x1 = centerViewport.x + radiusPx * cosf(a1);
        const float y1 = centerViewport.y + radiusPx * sinf(a1);
        const float x2 = centerViewport.x + radiusPx * cosf(a2);
        const float y2 = centerViewport.y + radiusPx * sinf(a2);
        SDL_RenderLine(sdlRenderer, x1, y1, x2, y2);
    }

    // 恢复原始颜色
    SDL_SetRenderDrawColor(sdlRenderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

/**
 * Draw a Solid Capsule（对齐相机/视口逻辑）
 * 绘制实心胶囊体（Solid Capsule）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawSolidCapsuleFcn(
    b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void *context) {
    if (context == nullptr) {
        LogCat::w("b2DrawSolidCapsuleFcn context= nullptr");
        return;
    }
    const auto *box2dSystemContext = static_cast<Box2dSystemContext *>(context);
    const WorldContext *worldContext = box2dSystemContext->GetWorldContext();
    SDL_Renderer *sdlRenderer = box2dSystemContext->GetRenderer();

    // 保存原始颜色
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(sdlRenderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

    SetSDLColor(sdlRenderer, color);
    LogCat::d("[DrawSolidCapsule] p1=(", p1.x, ",", p1.y, ") p2=(", p2.x, ",", p2.y, ") radius=", radius);

    // 1. 绘制胶囊中心线
    WorldVector2D vp1 = ConvertBox2DToViewport(worldContext, p1);
    WorldVector2D vp2 = ConvertBox2DToViewport(worldContext, p2);
    SDL_RenderLine(sdlRenderer, vp1.x, vp1.y, vp2.x, vp2.y);

    // 2. 绘制两端圆形（实心）- 替换为兼容实现
    float radiusPx = radius * kScale;
    for (int j = 0; j < 2; ++j) {
        b2Vec2 c = (j == 0 ? p1 : p2);
        WorldVector2D cvp = ConvertBox2DToViewport(worldContext, c);

        // 填充圆形
        SDL_RenderFillCircle_Compat(sdlRenderer, cvp.x, cvp.y, radiusPx);

        // 绘制圆形边框
        constexpr float step = 2.0F * kPi / kCircleSegments;
        for (int i = 0; i < kCircleSegments; ++i) {
            const float a1 = static_cast<float>(i) * step;
            const float a2 = static_cast<float>(i + 1) * step;
            const float x1 = cvp.x + radiusPx * cosf(a1);
            const float y1 = cvp.y + radiusPx * sinf(a1);
            const float x2 = cvp.x + radiusPx * cosf(a2);
            const float y2 = cvp.y + radiusPx * sinf(a2);
            SDL_RenderLine(sdlRenderer, x1, y1, x2, y2);
        }
    }

    // 恢复原始颜色
    SDL_SetRenderDrawColor(sdlRenderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

/**
 * Draw a Segment（对齐相机/视口逻辑）
 * 绘制线段（Segment）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawSegmentFcn(
    b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context) {
    if (context == nullptr) {
        LogCat::w("b2DrawSegmentFcn context= nullptr");
        return;
    }
    const auto *box2dSystemContext = static_cast<Box2dSystemContext *>(context);
    const WorldContext *worldContext = box2dSystemContext->GetWorldContext();
    SDL_Renderer *sdlRenderer = box2dSystemContext->GetRenderer();

    // 保存原始颜色
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(sdlRenderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

    SetSDLColor(sdlRenderer, color);
    // 转换线段端点到视口坐标
    WorldVector2D vp1 = ConvertBox2DToViewport(worldContext, p1);
    WorldVector2D vp2 = ConvertBox2DToViewport(worldContext, p2);
    SDL_RenderLine(sdlRenderer, vp1.x, vp1.y, vp2.x, vp2.y);
    LogCat::d("[DrawSegment] p1=(", p1.x, ",", p1.y, ") p2=(", p2.x, ",", p2.y, ")");

    // 恢复原始颜色
    SDL_SetRenderDrawColor(sdlRenderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

/**
 * Draw a Transform（对齐相机/视口逻辑）
 * 绘制变换坐标轴（Transform）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawTransformFcn(b2Transform transform, void *context) {
    if (context == nullptr) {
        LogCat::w("b2DrawTransformFcn context= nullptr");
        return;
    }
    const auto *box2dSystemContext = static_cast<Box2dSystemContext *>(context);
    const WorldContext *worldContext = box2dSystemContext->GetWorldContext();
    SDL_Renderer *sdlRenderer = box2dSystemContext->GetRenderer();

    // 保存原始颜色
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(sdlRenderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

    const b2Vec2 &p = transform.p;
    b2Vec2 xAxis = {p.x + 0.4f * transform.q.c, p.y + 0.4f * transform.q.s};
    b2Vec2 yAxis = {p.x - 0.4f * transform.q.s, p.y + 0.4f * transform.q.c};

    // 转换坐标到视口
    WorldVector2D pViewport = ConvertBox2DToViewport(worldContext, p);
    WorldVector2D xAxisViewport = ConvertBox2DToViewport(worldContext, xAxis);
    WorldVector2D yAxisViewport = ConvertBox2DToViewport(worldContext, yAxis);

    // X轴（红色）
    SDL_SetRenderDrawColor(sdlRenderer, 255, 0, 0, 255);
    SDL_RenderLine(sdlRenderer, pViewport.x, pViewport.y, xAxisViewport.x, xAxisViewport.y);

    // Y轴（绿色）
    SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 0, 255);
    SDL_RenderLine(sdlRenderer, pViewport.x, pViewport.y, yAxisViewport.x, yAxisViewport.y);

    LogCat::d("[DrawTransform] origin=(", p.x, ",", p.y, ") xAxis=(", xAxis.x, ",", xAxis.y, ") yAxis=(", yAxis.x, ",",
              yAxis.y, ")");

    // 恢复原始颜色
    SDL_SetRenderDrawColor(sdlRenderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

/**
 * Draw a Point（对齐相机/视口逻辑）
 * 绘制点（Point）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawPointFcn(
    b2Vec2 p, float size, b2HexColor color, void *context) {
    if (context == nullptr) {
        LogCat::w("b2DrawPointFcn context= nullptr");
        return;
    }
    const auto *box2dSystemContext = static_cast<Box2dSystemContext *>(context);
    const WorldContext *worldContext = box2dSystemContext->GetWorldContext();
    SDL_Renderer *sdlRenderer = box2dSystemContext->GetRenderer();

    // 保存原始颜色
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(sdlRenderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

    SetSDLColor(sdlRenderer, color);
    // 转换点坐标到视口
    WorldVector2D pViewport = ConvertBox2DToViewport(worldContext, p);
    // 点大小转换为像素
    float sizePx = size * kScale;
    const SDL_FRect rect = {(pViewport.x - sizePx / 2), (pViewport.y - sizePx / 2), sizePx, sizePx};
    SDL_RenderFillRect(sdlRenderer, &rect); // 填充点（更醒目）
    LogCat::d("[DrawPoint] pos=(", p.x, ",", p.y, ") size=", size);

    // 恢复原始颜色
    SDL_SetRenderDrawColor(sdlRenderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

/**
 * Draw a String
 * 绘制字符串（String）
 */
void glimmer::DebugDrawBox2dSystem::b2DrawStringFcn(
    b2Vec2 p, const char *s, b2HexColor color, void *context) {
    LogCat::d("Box2D Debug String (", p.x, ", ", p.y, "): ", s);
}

void glimmer::DebugDrawBox2dSystem::Render(SDL_Renderer *renderer) {
    auto box2dSystemContext = Box2dSystemContext(worldContext_, renderer);
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
    debugDraw.context = &box2dSystemContext;
    debugDraw.drawShapes = true;
    b2World_Draw(worldContext_->GetWorldId(), &debugDraw);
    CameraComponent *cameraComponent = worldContext_->GetCameraComponent();
    Transform2DComponent *cameraTransform2DComponent = worldContext_->GetCameraTransform2D();
    const std::vector<GameEntity::ID> rayCast2dEntity =
            worldContext_->GetEntityIDWithComponents<RayCast2DComponent>();
    for (const GameEntity::ID entity: rayCast2dEntity) {
        const auto rayComp =
                worldContext_->GetComponent<RayCast2DComponent>(entity);
        if (rayComp == nullptr) {
            continue;
        }
        const auto transform2dComponent =
                worldContext_->GetComponent<Transform2DComponent>(rayComp->GetTransform2DEntity());
        if (transform2dComponent == nullptr || !rayComp->IsContinuous()) {
            continue;
        }
        if (rayComp->IsHit()) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0,
                                   255);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255,
                                   255);
        }
        WorldVector2D startPosition = transform2dComponent->GetPosition() + rayComp->
                                      GetOrigin();
        const CameraVector2D origin = cameraComponent->GetViewPortPosition(cameraTransform2DComponent->GetPosition(),
                                                                           startPosition);
        const CameraVector2D end = cameraComponent->GetViewPortPosition(cameraTransform2DComponent->GetPosition(),
                                                                        startPosition + rayComp->GetTranslation());
        SDL_RenderLine(renderer, origin.x, origin.y, end.x, end.y);
    }
    AppContext::RestoreColorRenderer(renderer);
}

uint8_t glimmer::DebugDrawBox2dSystem::GetRenderOrder() {
    return RENDER_ORDER_DEBUG_BOX2D;
}

std::string glimmer::DebugDrawBox2dSystem::GetName() {
    return "glimmer.DebugDrawBox2dSystem";
}
