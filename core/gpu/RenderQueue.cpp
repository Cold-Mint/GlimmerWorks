/*
 * Copyright (C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
 * 版权(C) 2025-2026  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "RenderQueue.h"

#include <algorithm>
#include <cmath>

#include "GpuTexture.h"

void glimmer::RenderQueue::AppendQuad(const RenderLayer layer, const float depth, const GpuTexture *texture,
                                      const SDL_FPoint positions[4], const SDL_FPoint uvs[4],
                                      const SDL_Color &color) {
    RenderCommand &command = commands_.emplace_back();
    command.texture = texture;
    command.layer = layer;
    command.depth = depth;
    for (int i = 0; i < 4; ++i) {
        command.corners[i] = {
            positions[i].x, positions[i].y, uvs[i].x, uvs[i].y, color.r, color.g, color.b, color.a
        };
    }
}

void glimmer::RenderQueue::Clear() {
    commands_.clear();
}

void glimmer::RenderQueue::Reserve(const size_t commandCount) {
    commands_.reserve(commandCount);
}

size_t glimmer::RenderQueue::GetCommandCount() const {
    return commands_.size();
}

bool glimmer::RenderQueue::IsEmpty() const {
    return commands_.empty();
}

void glimmer::RenderQueue::Sort() {
    std::ranges::stable_sort(commands_, [](const RenderCommand &commandA, const RenderCommand &commandB) {
        if (commandA.layer != commandB.layer) {
            return commandA.layer < commandB.layer;
        }
        return commandA.depth < commandB.depth;
    });
}

const std::vector<glimmer::RenderCommand> &glimmer::RenderQueue::GetCommands() const {
    return commands_;
}

void glimmer::RenderQueue::DrawTexture(const RenderLayer layer, const float depth, const GpuTexture *texture,
                                       const SDL_FRect *src, const SDL_FRect *dst, const SDL_Color &mod) {
    if (texture == nullptr || !texture->IsValid()) {
        return;
    }
    const float textureWidth = static_cast<float>(texture->w);
    const float textureHeight = static_cast<float>(texture->h);
    SDL_FRect dstRect;
    if (dst == nullptr) {
        dstRect = {0.0F, 0.0F, textureWidth, textureHeight};
    } else {
        dstRect = *dst;
    }
    if (dstRect.w <= 0.0F || dstRect.h <= 0.0F) {
        return;
    }
    float u0 = 0.0F;
    float v0 = 0.0F;
    float u1 = 1.0F;
    float v1 = 1.0F;
    if (src != nullptr) {
        u0 = src->x / textureWidth;
        v0 = src->y / textureHeight;
        u1 = (src->x + src->w) / textureWidth;
        v1 = (src->y + src->h) / textureHeight;
    }
    const SDL_FPoint positions[4] = {
        {dstRect.x, dstRect.y},
        {dstRect.x + dstRect.w, dstRect.y},
        {dstRect.x, dstRect.y + dstRect.h},
        {dstRect.x + dstRect.w, dstRect.y + dstRect.h}
    };
    const SDL_FPoint uvs[4] = {
        {u0, v0},
        {u1, v0},
        {u0, v1},
        {u1, v1}
    };
    AppendQuad(layer, depth, texture, positions, uvs, mod);
}

void glimmer::RenderQueue::DrawTextureRotated(const RenderLayer layer, const float depth, const GpuTexture *texture,
                                              const SDL_FRect *src, const SDL_FRect *dst, const double angleDegrees,
                                              const SDL_FPoint *center, const Uint8 flip, const SDL_Color &mod) {
    if (texture == nullptr || !texture->IsValid() || dst == nullptr) {
        return;
    }
    if (dst->w <= 0.0F || dst->h <= 0.0F) {
        return;
    }
    const float textureWidth = static_cast<float>(texture->w);
    const float textureHeight = static_cast<float>(texture->h);
    float u0 = 0.0F;
    float v0 = 0.0F;
    float u1 = 1.0F;
    float v1 = 1.0F;
    if (src != nullptr) {
        u0 = src->x / textureWidth;
        v0 = src->y / textureHeight;
        u1 = (src->x + src->w) / textureWidth;
        v1 = (src->y + src->h) / textureHeight;
    }
    SDL_FPoint positions[4] = {
        {dst->x, dst->y},
        {dst->x + dst->w, dst->y},
        {dst->x, dst->y + dst->h},
        {dst->x + dst->w, dst->y + dst->h}
    };
    if (angleDegrees != 0.0) {
        SDL_FPoint rotationCenter;
        if (center == nullptr) {
            rotationCenter = {dst->x + dst->w * 0.5F, dst->y + dst->h * 0.5F};
        } else {
            rotationCenter = {dst->x + center->x, dst->y + center->y};
        }
        //Positive angles rotate clockwise (SDL_RenderTextureRotated semantics,
        //+Y points down in screen space).
        //正角度顺时针旋转（SDL_RenderTextureRotated 语义，屏幕空间 +Y 向下）。
        const double radians = angleDegrees * (3.14159265358979323846 / 180.0);
        const auto cosValue = static_cast<float>(std::cos(radians));
        const auto sinValue = static_cast<float>(std::sin(radians));
        for (auto &position: positions) {
            const float dx = position.x - rotationCenter.x;
            const float dy = position.y - rotationCenter.y;
            position.x = rotationCenter.x + dx * cosValue - dy * sinValue;
            position.y = rotationCenter.y + dx * sinValue + dy * cosValue;
        }
    }
    SDL_FPoint uvs[4] = {
        {u0, v0},
        {u1, v0},
        {u0, v1},
        {u1, v1}
    };
    if ((flip & FLIP_HORIZONTAL) != 0) {
        std::swap(uvs[0].x, uvs[1].x);
        std::swap(uvs[2].x, uvs[3].x);
    }
    if ((flip & FLIP_VERTICAL) != 0) {
        std::swap(uvs[0].y, uvs[2].y);
        std::swap(uvs[1].y, uvs[3].y);
    }
    AppendQuad(layer, depth, texture, positions, uvs, mod);
}

void glimmer::RenderQueue::FillRect(const RenderLayer layer, const float depth, const SDL_FRect *rect,
                                    const SDL_Color &color) {
    if (rect == nullptr) {
        return;
    }
    if (rect->w <= 0.0F || rect->h <= 0.0F) {
        return;
    }
    const SDL_FPoint positions[4] = {
        {rect->x, rect->y},
        {rect->x + rect->w, rect->y},
        {rect->x, rect->y + rect->h},
        {rect->x + rect->w, rect->y + rect->h}
    };
    const SDL_FPoint uvs[4] = {
        {0.0F, 0.0F},
        {1.0F, 0.0F},
        {0.0F, 1.0F},
        {1.0F, 1.0F}
    };
    AppendQuad(layer, depth, nullptr, positions, uvs, color);
}

void glimmer::RenderQueue::DrawRect(const RenderLayer layer, const float depth, const SDL_FRect *rect,
                                    const SDL_Color &color) {
    if (rect == nullptr) {
        return;
    }
    const SDL_FRect top = {rect->x, rect->y, rect->w, 1.0F};
    const SDL_FRect bottom = {rect->x, rect->y + rect->h - 1.0F, rect->w, 1.0F};
    const SDL_FRect left = {rect->x, rect->y + 1.0F, 1.0F, rect->h - 2.0F};
    const SDL_FRect right = {rect->x + rect->w - 1.0F, rect->y + 1.0F, 1.0F, rect->h - 2.0F};
    FillRect(layer, depth, &top, color);
    FillRect(layer, depth, &bottom, color);
    FillRect(layer, depth, &left, color);
    FillRect(layer, depth, &right, color);
}

void glimmer::RenderQueue::DrawLine(const RenderLayer layer, const float depth, const float x1, const float y1,
                                    const float x2, const float y2, const SDL_Color &color) {
    const float dx = x2 - x1;
    const float dy = y2 - y1;
    const float length = std::sqrt(dx * dx + dy * dy);
    if (length <= 0.0F) {
        DrawPoint(layer, depth, x1, y1, color);
        return;
    }
    //A 1-pixel thick quad perpendicular to the line direction.
    //垂直于线段方向的 1 像素宽四边形。
    const float nx = -dy / length * 0.5F;
    const float ny = dx / length * 0.5F;
    const SDL_FPoint positions[4] = {
        {x1 + nx, y1 + ny},
        {x1 - nx, y1 - ny},
        {x2 + nx, y2 + ny},
        {x2 - nx, y2 - ny}
    };
    const SDL_FPoint uvs[4] = {
        {0.0F, 0.0F},
        {1.0F, 0.0F},
        {0.0F, 1.0F},
        {1.0F, 1.0F}
    };
    AppendQuad(layer, depth, nullptr, positions, uvs, color);
}

void glimmer::RenderQueue::DrawPoint(const RenderLayer layer, const float depth, const float x, const float y,
                                     const SDL_Color &color) {
    const SDL_FRect rect = {x, y, 1.0F, 1.0F};
    FillRect(layer, depth, &rect, color);
}
